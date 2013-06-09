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

var yoctopuce = require('../');
var util = require('util');

var CRLF = '\r\n';

if (process.argv.length < 3) {
  util.log("Use: node httpRequest.js devicename");
  process.exit();
}

var options = { device: process.argv[2], message: "GET /api.json HTTP/1.1" + CRLF + CRLF };
yoctopuce.request(options, function (response) {
  // You can handle the response in the callback or in the data event of the request.
  util.log(util.format("Raw:\n%s", response));
}).on("error", function (err) {
  util.log(util.format("Error: %s", err));
}).on("headers", function (headers) {
  util.log(util.format("Headers:\n%s", util.inspect(headers, { showHidden: true, depth: null })));
}).on("body", function (body) {
  var json = JSON.parse(body);
  util.log(util.format("Body:\n%s", util.inspect(json, { showHidden: true, depth: null })));
});