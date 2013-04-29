﻿
// -*- C++ -*-
//
// Copyright (c) 2013, Tom Greasley <tom@greasley.com>
//
// Copyright Tom Greasley and contributors. All rights reserved.
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef SRC_YOCTOPUCE_H_
#define SRC_YOCTOPUCE_H_

#include <v8.h>
#include <node.h>
#include <yapi.h>

#include "./async.h"
#include "./events.h"

using v8::Handle;
using v8::Persistent;
using v8::Arguments;

using node::ObjectWrap;

namespace node_yoctopuce {

    class Yoctopuce : public ObjectWrap {
    public:
        static void Initialize(Handle<Object> target);
        static void Uninitialize();

    protected:
        static Persistent<Object> targetHandle;

        //  API Calls
        static Handle<Value> UpdateDeviceList(const Arguments& args);
        static Handle<Value> HandleEvents(const Arguments& args);
        static Handle<Value> GetDeviceInfo(const Arguments& args);

        //  Events
        typedef Async<Event> AsyncEventHandler;
        static AsyncEventHandler* eventHandler;
        static void EventCallback(Event *event);
        static void fwdLogEvent(const char* log, u32 loglen);
        static void fwdDeviceLogEvent(YAPI_DEVICE device);
        static void fwdDeviceArrivalEvent(YAPI_DEVICE device);
        static void fwdDeviceRemovalEvent(YAPI_DEVICE device);
        static void fwdDeviceChangeEvent(YAPI_DEVICE device);
        static void fwdFunctionUpdateEvent(YAPI_FUNCTION fundescr,
                                           const char *value);
    };

}  // namespace node_yoctopuce

#endif  // SRC_YOCTOPUCE_H_
