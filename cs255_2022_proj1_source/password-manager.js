"use strict";

/********* External Imports ********/
const { byteArrayToString, genRandomSalt, untypedToTypedArray, bufferToUntypedArray, stringToByteArray } = require("./lib");
const { subtle } = require('crypto').webcrypto;

/********* Implementation ********/
function objectHasKey(obj, key) {
  return obj.hasOwnProperty(key);
};

function arrayEqual(a1, a2) {
  if (a1.length != a1.length) {
    return false;
  }

  for (let i = 0; i < a1.length; i++) {
    if (a1[i] !== a2[i]) {
      return false;
    }
  }

  return true;
}

class Keychain {
  /**
   * Initializes the keychain using the provided information. Note that external
   * users should likely never invoke the constructor directly and instead use
   * either Keychain.init or Keychain.load. 
   * Arguments:
   *  You may design the constructor with any parameters you would like. 
   * Return Type: void
   */
  constructor(kvs, salt, rk, rv, kk, kv, HMACKey, AESKey) {
    this.data = {
      /* Store member variables that you intend to be public here
         (i.e. information that will not compromise security if an adversary sees) */
    };
    this.secrets = {
      /* Store member variables that you intend to be private here
         (information that an adversary should NOT see). */
    };

    this.secrets.kvs = kvs;
    this.secrets.salt = salt; // string
    this.secrets.rk = rk; // string
    this.secrets.rv = rv; // string
    this.secrets.kk = bufferToUntypedArray(kk); // ArrayBuffer
    this.secrets.kv = bufferToUntypedArray(kv); // ArrayBuffer
    this.secrets.HMACKey = HMACKey; // CryptoKey
    this.secrets.AESKey = AESKey; // CryptoKey

    this.data.version = "CS 255 Password Manager v1.0";
    // Flag to indicate whether password manager is "ready" or not
    this.ready = true;

    // throw "Not Implemented!";
  };

  /** 
    * Creates an empty keychain with the given password. Once the constructor
    * has finished, the password manager should be in a ready state.
    *
    * Arguments:
    *   password: string
    * Return Type: void
    */
  static async init(password) {
    // throw "Not Implemented!";

    /**
     * proj1.pdf:
     * The Node implementation of subtle will automatically convert Strings 
     * specified as input parameters into their corresponding ArrayBuffer forms
     */

    /**
     * Key derived from password by using PBKDF2 
     * Example: https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/deriveKey#pbkdf2
     * */
    let /* CryptoKey */ keyMaterial = await subtle.importKey(
      "raw",
      password,
      "PBKDF2",
      false,
      ["deriveKey"]
    );

    let /* string */ salt = genRandomSalt();
    let /* CryptoKey */ key = await subtle.deriveKey(
      {
        name: "PBKDF2",
        salt: salt,
        iterations: Keychain.PBKDF2_ITERATIONS,
        hash: "SHA-256"
      },
      keyMaterial,
      { name: "HMAC", hash: "SHA-256", length: 256 },
      true,
      ["sign", "verify"]
    );

    /**
     * Secret Key for HAMCing KVS-key(i.e. domain) 
     * Example: https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/sign#hmac
     * */
    let /* string */ rk = genRandomSalt();
    let /* ArrayBuffer */ kk = await subtle.sign(
      "HMAC",
      key,
      rk
    );
    let /* CryptoKey */ HMACKey = await subtle.importKey(
      "raw",
      kk,
      { name: "HMAC", hash: "SHA-256" },
      false,
      ["sign"]
    );

    /**
     * Secret Key for AES-GCM encrypting KVS-value(i.e. password for 
     * corresponsing domain) 
     * as above */
    let /* string */ rv = genRandomSalt();
    let /* ArrayBuffer */ kv = await subtle.sign(
      "HMAC",
      key,
      rv
    );
    let AESKey = await subtle.importKey(
      "raw",
      kv,
      { name: "AES-GCM", length: 256 },
      true,
      ["encrypt", "decrypt"]
    );

    return new Keychain({},
      salt, rk, rv,
      kk, kv,
      HMACKey, AESKey
    );
  }

  /**
    * Loads the keychain state from the provided representation (repr). The
    * repr variable will contain a JSON encoded serialization of the contents
    * of the KVS (as returned by the dump function). The trustedDataCheck
    * is an *optional* SHA-256 checksum that can be used to validate the 
    * integrity of the contents of the KVS. If the checksum is provided and the
    * integrity check fails, an exception should be thrown. You can assume that
    * the representation passed to load is well-formed (i.e., it will be
    * a valid JSON object).Returns a Keychain object that contains the data
    * from repr. 
    *
    * Arguments:
    *   password:           string
    *   repr:               string
    *   trustedDataCheck: string
    * Return Type: Keychain
    */
  static async load(password, repr, trustedDataCheck) {
    // throw "Not Implemented!";
    if (trustedDataCheck !== undefined) {
      // check integerity
      let digestRes = await subtle.digest('SHA-256', repr);
      let now = byteArrayToString(digestRes),
        prev = trustedDataCheck;
      if (now === prev) {
        ready = false;
        throw "Tampering is detected.";
      }
    }

    let contents = JSON.parse(repr);
    let salt = contents["salt"],
      rk = contents["rk"],
      rv = contents["rv"],
      trueKk = contents["kk"],
      trueKv = contents["kv"];
    // check master-password
    let /* CryptoKey */ keyMaterial = await subtle.importKey(
      "raw",
      password,
      "PBKDF2",
      false,
      ["deriveKey"]
    );
    let /* CryptoKey */ key = await subtle.deriveKey(
      {
        name: "PBKDF2",
        salt: salt,
        iterations: Keychain.PBKDF2_ITERATIONS,
        hash: "SHA-256"
      },
      keyMaterial,
      { name: "HMAC", hash: "SHA-256", length: 256 },
      true,
      ["sign", "verify"]
    );

    let /* ArrayBuffer */ kk = await subtle.sign(
      "HMAC",
      key,
      rk
    );
    if (!arrayEqual(bufferToUntypedArray(kk), trueKk)) {
      throw "Keychain not initialized.";
    }
    let /* CryptoKey */ HMACKey = await subtle.importKey(
      "raw",
      kk,
      { name: "HMAC", hash: "SHA-256" },
      false,
      ["sign"]
    );

    let /* ArrayBuffer */ kv = await subtle.sign(
      "HMAC",
      key,
      rv
    );
    if (!arrayEqual(bufferToUntypedArray(kv), trueKv)) {
      throw "Keychain not initialized.";
    }
    let AESKey = await subtle.importKey(
      "raw",
      kv,
      { name: "AES-GCM", length: 256 },
      true,
      ["encrypt", "decrypt"]
    );

    return new Keychain(contents["kvs"], salt, rk, rv, kk, kv, HMACKey, AESKey);
  };

  /**
    * Returns a JSON serialization of the contents of the keychain that can be 
    * loaded back using the load function. The return value should consist of
    * an array of two strings:
    *   arr[0] = JSON encoding of password manager
    *   arr[1] = SHA-256 checksum (as a string)
    * As discussed in the handout, the first element of the array should contain
    * all of the data in the password manager. The second element is a SHA-256
    * checksum computed over the password manager to preserve integrity. If the
    * password manager is not in a ready-state, return null.
    *
    * Return Type: array
    */
  async dump() {
    // throw "Not Implemented!";
    if (this.ready !== true) {
      return null;
    }

    /**
     * proj1.pdf:
     * you should use an untyped array to serialize entries in the KVS 
     * as the data will NOT correctly persist after a dump as either 
     * a String or an ArrayBuffer. 
     *
     *  contents[0] = kvs
     *  contents[1] = salt
     *  contents[2] = rk
     *  contents[3] = rv
     *  contents[4] = kk
     *  contents[5] = kv
     */
    let /* unTypedArray */ contents = this.secrets;

    let /* string */ json = JSON.stringify(contents);
    let /* bitArray */ checksum = await subtle.digest('SHA-256', json);
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
    * Return Type: Promise<string>
    */
  async get(name) {
    // throw "Not Implemented!";
    if (this.ready !== true) {
      throw "Keychain not initialized.";
    }

    let res = null;
    // 1. compute KVS-key
    let key = await subtle.sign(
      "HMAC",
      this.secrets.HMACKey,
      name
    );
    key = bufferToUntypedArray(key);
    // 2. decrypt
    if (objectHasKey(this.secrets.kvs, key)) {
      let value = this.secrets.kvs[key];
      let iv = value[0],
        ciphertext = untypedToTypedArray(value[1]);
      res = await subtle.decrypt(
        {
          name: "AES-GCM",
          iv: stringToByteArray(iv)
        },
        this.secrets.AESKey,
        ciphertext
      );
      res = byteArrayToString(res);
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
  async set(name, value) {
    // throw "Not Implemented!";
    if (this.ready !== true) {
      throw "Keychain not initialized.";
    }

    // 1. compute KVS-key
    let key = await subtle.sign(
      "HMAC",
      this.secrets.HMACKey,
      stringToByteArray(name)
    );
    key = bufferToUntypedArray(key);
    // 2. compute KVS-value
    let iv = genRandomSalt(12);
    let ciphertext = await subtle.encrypt(
      {
        name: "AES-GCM",
        iv: stringToByteArray(iv)
      },
      this.secrets.AESKey,
      value
    );
    // 3. insert or update KVS
    this.secrets.kvs[(key)] = [iv, bufferToUntypedArray(ciphertext)];
  };

  /**
    * Removes the record with name from the password manager. Returns true
    * if the record with the specified name is removed, false otherwise. If
    * the password manager is not in a ready state, throws an exception.
    *
    * Arguments:
    *   name: string
    * Return Type: Promise<boolean>
  */
  async remove(name) {
    // throw "Not Implemented!";
    if (this.ready !== true) {
      throw "Keychain not initialized.";
    }

    // 1. compute KVS-key
    let key = await subtle.sign(
      "HMAC",
      this.secrets.HMACKey,
      name
    );
    key = bufferToUntypedArray(key);

    // 2. remove
    if (objectHasKey(this.secrets.kvs, key)) {
      delete this.secrets.kvs[key];
      return true;
    }

    return false;
  };

  static get PBKDF2_ITERATIONS() { return 100000; }
};

module.exports = {
  Keychain: Keychain
}
