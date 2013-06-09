node-yoctopuce [![Build Status](https://secure.travis-ci.org/schlafsack/node-yoctopuce.png)](http://travis-ci.org/schlafsack/node-yoctopuce)
===

###### Access Yoctopuce devices using [node.js](http://nodejs.org)

Installing the module
---

This is a native binding to the low level yoctopuce api. Installation requires an envrionment suitable for compiling C/C++ using [node-gyp](https://github.com/TooTallNate/node-gyp).
Please see the node-gyp [readme](https://github.com/TooTallNate/node-gyp/blob/master/README.md) for more information.

With [npm](http://npmjs.org/):

```bash
$ npm install node-yoctopuce
```

From source:

```bash
$ node-gyp configure
$ node-gyp build
```

Viewing the examples
---

Clone the node-yoctopuce repo, then install:

```bash
$ git clone git://github.com/schlafsack/node-yoctopuce.git --depth 1
$ cd node-yoctopuce
$ npm install
```

then run the examples:

```bash
$ node examples/httpRequest MyDevice
```

API
---

The API is split into two sections. Firstly a high level, node-like, asynchronous API and secondly the synchronous bindings to the native YAPI library.

####node-yocotpuce API

###### Coming soon...

####YAPI

#####yapi.checkLogicalName(name)

Verifies if a given logical name is valid or not for the yocotpuce API.

```javascript
var valid = yapi.checkLogicalName(name);
```

#####yapi.getAllDevices()

Returns an array of descriptors for the devices visible to the API.

```javascript
var descriptors = yapi.getAllDevices();
```

#####yapi.getApiVersion()

Returns version information for the YAPI library.

```javascript
var apiVersion = yapi.getApiVersion();
```

#####yapi.getDevice(identifier)

Returns the device descriptor for a device identified by a serial number, logical name or URL.

```javascript
var identifier = 'THRMCPL1-0A420';
var descriptor = yapi.getDevice(identifier);
```

#####yapi.getDeviceInfo(descriptor)

Returns the device information for a device identified by a device descriptor.

```javascript
var descriptor = 239;
var info = yapi.getDeviceInfo(descriptor);
```

#####yapi.getDevicePath(descriptor)

Returns the device's serial number and path to use when making an intial http request.

```javascript
var descriptor = 239;
var path = yapi.getDevicePath(descriptor);
```

#####yapi.getFunction(functionClass, functionName);

Returns a function descriptor for a function identified by a class and a full hardware id or logical name.

A function hardware id takes the form <device>.<function>.

```javascript
var functionClass = 'Temperature';
var functionName = 'THRMCPL1-0A420.temperature1';
var descriptor = yapi.getFunction(functionClass, functionName);
```

#####yapi.getFunctionInfo(descriptor)

Returns the function information for a function identified by a function descriptor.

```javascript
var descriptor = 5374191;
var info = yapi.getFunctionInfo(descriptor);
```

#####yapi.getFunctionsByClass(functionClass)

Returns a list of descriptors for functions in the given class.

```javascript
var functionClass = 'Temperature';
var descriptors = yapi.getFunctionsByClass(functionClass);
```

#####yapi.getFunctionsByDevice(descriptor)

Returns a list of descriptors for the functions of the device identified by the descriptor.

```javascript
var descriptor = 239;
var descriptors = yapi.getFunctionsByClass(functionClass);
```

#####yapi.httpRequest(identifier, message)

Makes an http request to the device identified by a serial number, logical name or URL. 

```javascript
var identifier = 'THRMCPL1-0A420';
var response = yapi.httpRequest(identifier, "GET /api.json HTTP/1.1" + CRLF + CRLF);
```

#####yapi.registerHub(url)

Registers a network URL to be scanned for devices. 

```javascript
var url = 'http://remote:4444/';
yapi.registerHub(url);
```

#####yapi.unregisterHub(url)

Unregisters a network URL. 

```javascript
var url = 'http://remote:4444/';
yapi.unregisterHub(url);
```

#####Event: log

The `log` event is emitted when the YAPI library needs to log debug messages. 

```javascript
yapi.on("log", function (message) {
  util.log(util.format("Log %s.", message));
});
```

#####Event: deviceLog

The `deviceLog` event is emitted when the YAPI library detects a device event. 

```javascript
yapi.on("deviceLog", function (descriptor) {
  util.log(util.format("device log %d.", descriptor));
});
```

#####Event: deviceArrival

The `deviceArrival` event is emitted when the YAPI library detects a device has arrived. 

```javascript
yapi.on("deviceArrival", function (descriptor) {
  util.log(util.format("device arrived %d.", descriptor));
});
```

#####Event: deviceRemoval

The `deviceRemoval` event is emitted when the YAPI library detects a device has been removed. 

```javascript
yapi.on("deviceRemoval", function (descriptor) {
  util.log(util.format("device removed %d.", descriptor));
});
```

#####Event: deviceChange

The `deviceChange` event is emitted when the YAPI library detects that the logical name of a device has changed. 

```javascript
yapi.on("deviceChange", function (descriptor) {
  util.log(util.format("device changed %d.", descriptor));
});
```

#####Event: functionUpdate

The `functionUpdate` event is emitted when the YAPI library detects that the value of a function has changed. 

```javascript
yapi.on("functionUpdate", function (descriptor, value) {
  util.log(util.format("function: %d value: %s.", descriptor, value));
});
```

Licence
---
Copyright (c) 2013, Tom Greasley <tom@greasley.com>

Copyright Tom Greasley and contributors. All rights reserved.
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

See Also:

    yoctopuce
     https://github.com/yoctopuce/yoctolib_cpp/blob/master/README.txt
