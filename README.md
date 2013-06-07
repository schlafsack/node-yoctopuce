node-yoctopuce [![Build Status](https://secure.travis-ci.org/schlafsack/node-yoctopuce.png)](http://travis-ci.org/schlafsack/node-yoctopuce)
===

###### Access Yoctopuce devices using [node.js](http://nodejs.org)

Installing the module
---

This is a native binding to the low level yoctopuce api. Installation requires an envrionment suitable for compiling C/C++ using [node-gyp](https://github.com/TooTallNate/node-gyp).
Please see the node-gyp [readme](https://github.com/TooTallNate/node-gyp/blob/master/README.md) for more information.

With [npm](http://npmjs.org/):

    npm install node-yoctopuce

From source:

    node-gyp configure
    node-gyp build

Viewing the examples
---

Clone the node-yoctopuce repo, then install:

    $ git clone git://github.com/schlafsack/node-yoctopuce.git --depth 1
    $ cd node-yoctopuce
    $ npm install

then run the examples:

    $ node examples/httpRequest MyDevice

API
---

###### Coming soon...

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
