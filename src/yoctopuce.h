
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

#include <node.h>
#include <v8.h>
#include <yapi.h>

#include <queue>
#include <string>

#include "./events.h"

namespace node_yoctopuce {

#define THROW(msg, err, ex) \
    v8::Local<v8::String> m1 = v8::String::NewSymbol(msg); \
    v8::Local<v8::String> m2 = v8::String::Concat(m1, v8::String::New(err)); \
    v8::Handle<v8::Value> ex = v8::ThrowException(v8::Exception::Error(m2));

    struct HttpRequestBaton {
        uv_work_t* work;
        v8::Persistent<v8::Function> callback;
        v8::Persistent<v8::Object> request;
        std::string device;
        std::string message;
        std::string response;
        std::string error;
        YRETCODE result;
    };

    class Yoctopuce : public node::ObjectWrap {
    public:
        static void Initialize(v8::Handle<v8::Object> target);
        static void Close();

    protected:
        //  Sync API Calls
        static v8::Handle<v8::Value> UpdateDeviceList(const v8::Arguments& args);
        static v8::Handle<v8::Value> HandleEvents(const v8::Arguments& args);
        static v8::Handle<v8::Value> RegisterHub(const v8::Arguments& args);
        static v8::Handle<v8::Value> PreregisterHub(const v8::Arguments& args);
        static v8::Handle<v8::Value> UnregisterHub(const v8::Arguments& args);
        static v8::Handle<v8::Value> CheckLogicalName(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetApiVersion(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetDevice(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetAllDevices(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetDeviceInfo(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetDevicePath(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetFunction(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetFunctionsByClass(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetFunctionsByDevice(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetFunctionInfo(const v8::Arguments& args);
        static v8::Handle<v8::Value> HttpRequest(const v8::Arguments& args);

        //  Async API Calls
        static v8::Handle<v8::Value> HttpRequestAsync(const v8::Arguments& args);
        static void OnHttpRequest(uv_work_t* req);
        static void OnAfterHttpRequest(uv_work_t* req);

        // Utils
        static void EmitEvent(v8::Handle<v8::Object> context, v8::Handle<v8::String> event_name, v8::Handle<v8::Value> data);

        //  Events
        static void FwdLogEvent(const char* log, u32 loglen);
        static void FwdDeviceLogEvent(YAPI_DEVICE device);
        static void FwdDeviceArrivalEvent(YAPI_DEVICE device);
        static void FwdDeviceRemovalEvent(YAPI_DEVICE device);
        static void FwdDeviceChangeEvent(YAPI_DEVICE device);
        static void FwdFunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *value);
        static void FwdEvent(Event* event);
        static void OnEvent(uv_async_t *async, int status);

    private:
        static v8::Persistent<v8::Object> target_handle;
        static uv_mutex_t event_queue_mutex;
        static std::queue<Event*> event_queue;
        static uv_async_t event_async;
        static uv_mutex_t http_request_mutex;
    };

}  // namespace node_yoctopuce

#endif  // SRC_YOCTOPUCE_H_
