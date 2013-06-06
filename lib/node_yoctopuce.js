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

var bindings = require('bindings')('node_yoctopuce.node');
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var deviceListMillis = 5000;
var eventsMillis = 500;

// Make the bindings singleton *instance* inherit
// from EventEmitter
var k;
for (k in EventEmitter.prototype) {
  if (EventEmitter.prototype.hasOwnProperty(k)) {
    bindings[k] = EventEmitter.prototype[k];
  }
}

// bindings.httpRequestAsync
var HttpRequest = function (options) {
  var self = this;
  self.device = options.device;
  self.message = options.message;
};
util.inherits(HttpRequest, EventEmitter);

bindings.request = function (options, callback) {
  var request = new HttpRequest(options);
  bindings.httpRequestAsync(request, callback || function () {
  });
  return request;
};

// Pump the yocotpuce device list events
bindings.updateDeviceList();
setInterval(function () {
  bindings.updateDeviceList();
}, deviceListMillis).unref();

// Pump the yocotpuce device events
bindings.handleEvents();
setInterval(function () {
  bindings.handleEvents();
}, eventsMillis).unref();

module.exports = bindings;