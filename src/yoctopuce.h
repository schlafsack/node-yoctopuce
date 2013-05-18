
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

#include <queue>

#include "./events.h"

using v8::Handle;
using v8::Persistent;
using v8::Arguments;

using std::queue;

using node::ObjectWrap;

namespace node_yoctopuce {

    class Yoctopuce : public ObjectWrap {
    public:
        static void Initialize(Handle<Object> target);
        static void Uninitialize();

    protected:
        //  API Calls
        static Handle<Value> UpdateDeviceList(const Arguments& args);
        static Handle<Value> HandleEvents(const Arguments& args);
        static Handle<Value> GetDevice(const Arguments& args);
        static Handle<Value> GetAllDevices(const Arguments& args);
        static Handle<Value> GetDeviceInfo(const Arguments& args);
        static Handle<Value> GetDevicePath(const Arguments& args);
        static Handle<Value> GetFunction(const Arguments& args);

        //  Events
        static void fwdLogEvent(const char* log, u32 loglen);
        static void fwdDeviceLogEvent(YAPI_DEVICE device);
        static void fwdDeviceArrivalEvent(YAPI_DEVICE device);
        static void fwdDeviceRemovalEvent(YAPI_DEVICE device);
        static void fwdDeviceChangeEvent(YAPI_DEVICE device);
        static void fwdFunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *value);
        static void fwdEvent(Event* event);
        static void onEvent(uv_async_t *async, int status);

    private:
        static uint64_t g_main_thread_id;
        static Persistent<Object> g_target_handle;
        static uv_mutex_t g_event_queue_mutex;
        static queue<Event*> g_event_queue;
        static uv_async_t g_event_async;
    };

}  // namespace node_yoctopuce

#endif  // SRC_YOCTOPUCE_H_
