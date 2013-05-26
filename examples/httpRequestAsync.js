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
var yoctopuce, deviceName, json, data;

if (process.argv.length < 3) {
  util.log("Use: node httpRequest.js devicename");
  process.exit();
}

yoctopuce = require('../');
util.log("Yoctopuce Initialized:\n" + util.inspect(yoctopuce, { showHidden:true, depth:null }));

deviceName = process.argv[2];
yoctopuce.httpRequestAsync(deviceName, "GET /api.json \r\n\r\n", function (error, response) {
  if (error) {
    util.log("Error: " + error);
  }
  if (response) {
    // Dirty parsing, response begins with OK\r\n\r\n, so drop it and parse it as JSON.
    json = response.substring(6);
    data = JSON.parse(json);
    util.log("Response:\n" + util.inspect(data, { showHidden:true, depth:null }));
  }
});



