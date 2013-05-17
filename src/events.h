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

#ifndef SRC_EVENTS_H_
#define SRC_EVENTS_H_

#include <v8.h>
#include <node.h>
#include <yapi.h>
#include <algorithm>
#include <string>

using v8::Handle;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Integer;
using v8::Value;
using v8::Function;
using v8::TryCatch;
using v8::HandleScope;

using node::FatalException;

using std::string;

namespace node_yoctopuce {

    struct Event {
        uv_mutex_t dispatch_mutex;
        uv_cond_t dispatch_cond;

        explicit inline Event() {
            uv_mutex_init(&dispatch_mutex);
            uv_cond_init(&dispatch_cond); 
        }

        virtual void dispatch(Handle<Object> context)=0;

        void dispatchToV8(Handle<Object> context, int argc, Handle<Value> argv[]) {
            HandleScope scope;
            if(!context.IsEmpty())
            {
                Local<Value> dispatchValue = context->Get(String::NewSymbol("emit"));

                // If the emit function has been bound call it; otherwise
                // drop the events.
                if(!dispatchValue.IsEmpty() && dispatchValue->IsFunction())
                {
                    Local<Function> dispatchFunction = Local<Function>::Cast(dispatchValue);
                    TryCatch try_catch;
                    dispatchFunction->Call(context, argc, argv);
                    if (try_catch.HasCaught()) {
                        FatalException(try_catch);
                    }
                }
            }
            signalDispatch();
        }

        void signalDispatch() {
            uv_mutex_lock(&dispatch_mutex);
            uv_cond_signal(&dispatch_cond); 
            uv_mutex_unlock(&dispatch_mutex);
        }

        void waitOnDispatch() {
            uv_mutex_lock(&dispatch_mutex);
            uv_cond_wait(&dispatch_cond, &dispatch_mutex); 
            uv_mutex_unlock(&dispatch_mutex);
        }

        virtual ~Event() {
            uv_cond_destroy(&dispatch_cond);
            uv_mutex_destroy(&dispatch_mutex);
        }
    };

    struct CharDataEvent : Event {
        explicit inline CharDataEvent(const char *_data) : Event() {
            data = _data ? string(_data) : string();
        }
        string data;
    };

    struct LogEvent : CharDataEvent {
        explicit inline LogEvent(const char *data)
            : CharDataEvent(data) {}
        inline virtual void dispatch(Handle<Object> context) {
            int argc = 2;
            string log = string(data);
            log.erase(std::remove(log.begin(), log.end(), '\n'), log.end());
            log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());
            Handle<Value> argv[2] = {String::NewSymbol("log"), String::New(log.c_str())};
            dispatchToV8(context, argc, argv);
        }
    };

    struct FunctionUpdateEvent : CharDataEvent {
        explicit inline FunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *data)
            : CharDataEvent(data), fundescr(fundescr) {}
        YAPI_FUNCTION fundescr;
        inline virtual void dispatch(Handle<Object> context) {
            int argc = 3;
            Handle<Value> argv[3] =
            {String::NewSymbol("functionUpdate"), Integer::New(fundescr), String::New(data.c_str())};
            dispatchToV8(context, argc, argv);
        }
    };

    struct DeviceEvent : Event {
        explicit inline DeviceEvent(const char* name, YAPI_DEVICE device)
            : Event(), name(name), device(device) {}
        const char* name;
        YAPI_DEVICE device;
        inline virtual void dispatch(Handle<Object> context) {
            int argc = 2;
            Handle<Value> argv[2] = {String::NewSymbol(name), Integer::New(device)};
            dispatchToV8(context, argc, argv);
        }
    };

    struct DeviceLogEvent : DeviceEvent {
        explicit inline DeviceLogEvent(YAPI_DEVICE device)
            : DeviceEvent("deviceLog", device) {}
    };

    struct DeviceArrivalEvent : DeviceEvent {
        explicit inline DeviceArrivalEvent(YAPI_DEVICE device)
            : DeviceEvent("deviceArrival", device) {}
    };

    struct DeviceChangeEvent : DeviceEvent {
        explicit inline DeviceChangeEvent(YAPI_DEVICE device)
            : DeviceEvent("deviceChange", device) {}
    };

    struct DeviceRemovalEvent : DeviceEvent {
        explicit inline DeviceRemovalEvent(YAPI_DEVICE device)
            : DeviceEvent("deviceRemoval", device) {}
    };



}  // namespace node_yoctopuce

#endif  // SRC_EVENTS_H_
