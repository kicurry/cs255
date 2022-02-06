"use strict";

/**
 * getRandomValues is on window.crypto that means it works on browser.
 * To make it work on Node.js: 
 * 1. Install get-random-values
 *    npm i get-random-values
 * 2. Add the following in the module
 *    const getRandomValues = require('get-random-values');
 */
// const { getRandomValues } = require('crypto');
const getRandomValues = require('get-random-values')

let encoder = new TextEncoder();
let decoder = new TextDecoder();

let stringToByteArray = function(str) {
  return encoder.encode(str)
}

let byteArrayToString = function(arr) {
  return decoder.decode(arr);
}

let genRandomSalt = function(len=16) {
  return byteArrayToString(getRandomValues(new Uint8Array(len)));
}

let untypedToTypedArray = function(arr) {
  return new Uint8Array(arr);
}

let bufferToUntypedArray = function(arr) {
  return Array.from(new Uint8Array(arr));
}

module.exports = {
  stringToByteArray: stringToByteArray,
  byteArrayToString: byteArrayToString,
  genRandomSalt: genRandomSalt,
  untypedToTypedArray: untypedToTypedArray,
  bufferToUntypedArray: bufferToUntypedArray,
};
