/*
 * Copyright (c) 2013, Tom Greasley <tom@greasley.com>
 *
 * Copyright Tom Greasley and contributors. All rights reserved.
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*jshint globalstrict: true*/
"use strict";

var yapi = require('../../.').yapi;
var util = require('util');
var functionClass, functionId, functions;

if (process.argv.length < 3) {
  util.log("Use: node getFunctionsByClass.js functionclass <functionId>");
  process.exit();
}

functionClass = process.argv[2];
functionId = parseInt(process.argv[3], 0);

if (functionId) {
  functions = yapi.getFunctionsByClass(functionClass, functionId);
} else {
  functions = yapi.getFunctionsByClass(functionClass);
}

if (Array.isArray(functions)) {
  var i;
  util.log(util.format("%d functions found:", functions.length));
  for (i = 0; i < functions.length; i++) {
    util.log(util.format("Function found with id %d.", functions[i]));
  }
}