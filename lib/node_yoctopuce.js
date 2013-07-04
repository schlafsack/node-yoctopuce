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

var yapi = require('bindings')('node_yoctopuce.node');
var EventEmitter = require('events').EventEmitter;
var HTTPParser = process.binding('http_parser').HTTPParser;
var http = require('http');
var parsers = http.parsers;
var util = require('util');
var deviceListMillis = 5000;
var eventsMillis = 500;

// Set up YAPI

// Make the yapi singleton *instance* inherit
// from EventEmitter
var k;
for (k in EventEmitter.prototype) {
  if (EventEmitter.prototype.hasOwnProperty(k)) {
    yapi[k] = EventEmitter.prototype[k];
  }
}

// Pump the yocotpuce device list events
yapi.updateDeviceList();
yapi.updateDeviceListInterval = setInterval(function () {
  yapi.updateDeviceList();
}, deviceListMillis);
yapi.updateDeviceListInterval.unref();

// Pump the yocotpuce device events
yapi.handleEvents();
yapi.handleEventsInterval = setInterval(function () {
  yapi.handleEvents();
}, eventsMillis);
yapi.handleEventsInterval.unref();

// Export YAPI as a property of the module.
exports.yapi = yapi;

// Set up node-yoctopuce

exports.enableUsb = function () {
  yapi.registerHub(yapi.usb);
};

exports.disableUsb = function () {
  yapi.unregisterHub(yapi.usb);
};

function parseHttpResponse(data, request) {
  var buffer, parser, ret;
  buffer = new Buffer(data);
  parser = parsers.alloc();
  parser.reinitialize(HTTPParser.RESPONSE);
  parser.socket = {};
  parser.onIncoming = function (response) {
    request.emit("incoming", response);
  };
  parser.onBody = function (b, start, len) {
    var body = b.slice(start, start + len);
    request.emit("body", body);
  };
  parser.onHeadersComplete = function (headers) {
    request.emit("headers", headers);
  };
  ret = parser.execute(buffer, 0, buffer.length);
  if (ret instanceof Error) {
    request.emit("error", ret);
  }
  parser.finish();
  parser._headers = [];
  parser.onIncoming = null;
  parser.incoming = null;
  parsers.free(parser);
  parser = null;
}

function YoctopuceRequest(options) {
  var self = this;
  self.device = options.device;
  self.message = options.message;
}
util.inherits(YoctopuceRequest, EventEmitter);
exports.YoctopuceRequest = YoctopuceRequest;

exports.request = function (options, cb) {
  var request = new YoctopuceRequest(options);
  request.on("data", function (data) {
    parseHttpResponse(data, request);
  });
  yapi.httpRequestAsync(request, cb || function () {});
  return request;
};

module.exports = exports;
