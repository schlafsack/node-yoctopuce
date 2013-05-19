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

var util = require('util');

if (process.argv.length < 4) {
  util.log("Use: node getFunction.js functionclass function");
  process.exit();
}

var yoctopuce = require('../');
util.log("Yoctopuce Initialized:\n" + util.inspect(yoctopuce, { showHidden:true, depth:null }));

var functionClass = process.argv[2];
var functionName = process.argv[3];
try {
  var fnct = yoctopuce.getFunction(functionClass, functionName);
  util.log(util.format("Function %s:%s found with id %d.", functionClass, functionName, fnct));
}
catch (ex) {
  util.log(ex);
  util.log(util.format("Error getting function %s:%s", functionClass, functionName));
}