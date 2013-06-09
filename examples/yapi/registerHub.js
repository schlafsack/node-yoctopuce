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
var root, devices, deviceId;

if (process.argv.length < 3) {
  util.log("Use: node registerHub.js hubRoot");
  process.exit();
}

function displayDevices() {
  var deviceInfo, i, devices = yapi.getAllDevices();
  if (Array.isArray(devices)) {
    util.log(util.format("%d devices found:", devices.length));
    for (i = 0; i < devices.length; i++) {
      deviceId = devices[i];
      util.log(util.format("Device found with id %d.", devices[i]));
      try {
        deviceInfo = yapi.getDeviceInfo(deviceId);
        util.log(util.format("Device Info:\n%s", util.inspect(deviceInfo, { showHidden: true, depth: null })));
      } catch (ex) {
        util.log(ex);
        util.log(util.format("Error getting info for device %d.", deviceId));
      }
    }
  }
}

// e.g. yoctopuce.registerHub("http://peevay:4444/");
root = process.argv[2];

util.log("With no hub registered:");
displayDevices();

util.log(util.format("With %s registered:", root));
yapi.registerHub(root);
displayDevices();

util.log(util.format("With %s unregistered:", root));
yapi.unregisterHub(root);
displayDevices();
