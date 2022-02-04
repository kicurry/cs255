"use strict";


/********* External Imports ********/

var lib = require("./lib");

var KDF = lib.KDF,
    HMAC = lib.HMAC,
    SHA256 = lib.SHA256,
    /**
     * See more details in http://bitwiseshiftleft.github.io/sjcl/doc/sjcl.hash.sha256.html 
     * SHA256: string|bitArray -> bitArray */
    setupCipher = lib.setupCipher,
    encryptwithGCM = lib.encryptwithGCM,
    decryptWithGCM = lib.decryptWithGCM,
    bitarraySlice = lib.bitarraySlice,
    bitarrayToString = lib.bitarrayToString,
    stringToBitarray = lib.stringToBitarray,
    bitarrayToBase64 = lib.bitarrayToBase64,
    base64ToBitarray = lib.base64ToBitarray,
    stringToPaddedBitarray = lib.stringToPaddedBitarray,
    paddedBitarrayToString = lib.paddedBitarrayToString,
    randomBitarray = lib.randomBitarray,
    bitarrayEqual = lib.bitarrayEqual,
    bitarrayLen = lib.bitarrayLen,
    bitarrayConcat = lib.bitarrayConcat,
    objectHasKey = lib.objectHasKey;


/********* Implementation ********/


var keychainClass = function() {

  // Private instance variables.
    
  // Use this variable to store everything you need to.
  var priv = {
    secrets: { /* Your secrets here */ },
    data: { /* Non-secret data here */ }
  };

  // Maximum length of each record in bytes
  var MAX_PW_LEN_BYTES = 64;
  
  // Flag to indicate whether password manager is "ready" or not
  var ready = false;

  var keychain = {};

  /** 
    * Creates an empty keychain with the given password. Once init is called,
    * the password manager should be in a ready state.
    *
    * Arguments:
    *   password: string
    * Return Type: void
    */
  keychain.init = function(password) {
    priv.data.version = "CS 255 Password Manager v1.0";
    
    priv.kvs = {};

    /* Key derived from password by using PBKDF2 */
    var/* bitArray */ salt = randomBitarray(64); // recommended length is 64 bits
    priv.data.salt = salt;
    
    var/* bitArray */ key = KDF(password, salt);
    priv.secrets.key = key;

    /* Secret Key for HAMCing KVS-key(i.e. domain) */
    var/* bitArray */ rk = randomBitarray(128);
    priv.secrets.kk = HMAC(key, rk);

    /**
     * Secret Key for AES-GCM encrypting KVS-value(i.e. password for 
     * corresponsing domain) */
    var/* bitArray */ rv = randomBitarray(128); // AES-128
    priv.secrets.kv = bitarraySlice(HMAC(key, rv), 0, 128);

    /* Set the state of password manager */
    ready = true;
  };

  /**
    * Loads the keychain state from the provided representation (repr). The
    * repr variable will contain a JSON encoded serialization of the contents
    * of the KVS (as returned by the save function). The trustedDataCheck
    * is an *optional* SHA-256 checksum that can be used to validate the 
    * integrity of the contents of the KVS. If the checksum is provided and the
    * integrity check fails, an exception should be thrown. You can assume that
    * the representation passed to load is well-formed (i.e., it will be
    * a valid JSON object). Returns true if the data is successfully loaded
    * and the provided password is correct. Returns false otherwise.
    *
    * Arguments:
    *   password:           string
    *   repr:               string
    *   trustedDataCheck: string
    * Return Type: boolean
    */
  keychain.load = function(password, repr, trustedDataCheck) {
      // throw "Not implemented!";
      if (trustedDataCheck !== undefined) {
        // check integerity
        var now = SHA256(stringToBitarray(repr)),
            prev = trustedDataCheck;
        if (!bitarrayEqual(now, prev)) {
          ready = false;
          throw "Tampering is detected.";
        }
      }

      priv = JSON.parse(repr);
      
      // check master-password
      var trueKey = priv.secrets.key,
          salt = priv.data.salt;
      var key = KDF(password, salt);
      if (!bitarrayEqual(trueKey, key)) {
        ready = false;
        return false;
      }

      return true;
  };

  /**
    * Returns a JSON serialization of the contents of the keychain that can be 
    * loaded back using the load function. The return value should consist of
    * an array of two strings:
    *   arr[0] = JSON encoding of password manager
    *   arr[1] = SHA-256 checksum
    * As discussed in the handout, the first element of the array should contain
    * all of the data in the password manager. The second element is a SHA-256
    * checksum computed over the password manager to preserve integrity. If the
    * password manager is not in a ready-state, return null.
    *
    * Return Type: array
    */ 
  keychain.dump = function() {
      // throw "Not implemented!";
      if (ready !== true) {
        return null;
      }
      
      var/* string */ json = JSON.stringify(priv);
      var/* bitArray */ checksum = SHA256(stringToBitarray(json));
      return [json, checksum];
  };

  /**
    * Fetches the data (as a string) corresponding to the given domain from the KVS.
    * If there is no entry in the KVS that matches the given domain, then return
    * null. If the password manager is not in a ready state, throw an exception. If
    * tampering has been detected with the records, throw an exception.
    *
    * Arguments:
    *   name: string
    * Return Type: string
    */
  keychain.get = function(name) {
      // throw "Not implemented!";
      if (ready !== true) {
        throw "Keychain not initialized.";
      }

      var res = null;
      var key = HMAC(priv.secrets.kk, name);
      if (objectHasKey(priv.kvs, key)) {
        var cipher = setupCipher(priv.secrets.kv);
        var ciphertext = priv.kvs[key];
        res = paddedBitarrayToString(decryptWithGCM(cipher, ciphertext), MAX_PW_LEN_BYTES);
      }
      return res;
  };

  /** 
  * Inserts the domain and associated data into the KVS. If the domain is
  * already in the password manager, this method should update its value. If
  * not, create a new entry in the password manager. If the password manager is
  * not in a ready state, throw an exception.
  *
  * Arguments:
  *   name: string
  *   value: string
  * Return Type: void
  */
  keychain.set = function(name, value) {
      // throw "Not implemented!";
      if (ready !== true) {
        throw "Keychain not initialized.";
      }
      
      // 1. compute KVS-key
      var key = HMAC(priv.secrets.kk, name);
      // 2. compute KVS-value
      var cipher = setupCipher(priv.secrets.kv);
      var plaintext = stringToPaddedBitarray(value, MAX_PW_LEN_BYTES); // padding
      var ciphertext = encryptwithGCM(cipher, plaintext);
      // 3. insert or update KVS
      priv.kvs[key] = ciphertext;
    };

  /**
    * Removes the record with name from the password manager. Returns true
    * if the record with the specified name is removed, false otherwise. If
    * the password manager is not in a ready state, throws an exception.
    *
    * Arguments:
    *   name: string
    * Return Type: boolean
  */
  keychain.remove = function(name) {
      // throw "Not implemented!";
      if (ready !== true) {
        throw "Keychain not initialized.";
      }

      // 1. compute KVS-key
      var key = HMAC(priv.secrets.kk, name);
      // 2. remove
      if (objectHasKey(priv.kvs, key)) {
        delete priv.kvs[key];
        return true;
      }

      return false;
  };

  return keychain;
};

module.exports.keychain = keychainClass;
