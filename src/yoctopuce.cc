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

#include <yapi.h>
#include <node.h>
#include <v8.h>

#include <queue>

#include "./yoctopuce.h"
#include "./events.h"

using v8::HandleScope;
using v8::Persistent;
using v8::Exception;
using v8::String;
using v8::Undefined;
using v8::ThrowException;

using std::queue;
using std::swap;

namespace node_yoctopuce {

    Persistent<Object> Yoctopuce::targetHandle;

    void Yoctopuce::Initialize(Handle<Object> target) {
        HandleScope scope;

        // Expose API methods
        targetHandle = Persistent<Object>::New(target);
        NODE_SET_METHOD(targetHandle, "updateDeviceList", UpdateDeviceList);
        NODE_SET_METHOD(targetHandle, "handleEvents", HandleEvents);
        NODE_SET_METHOD(targetHandle, "getDeviceInfo", GetDeviceInfo);

        // Set up to handle device events
        yapiRegisterLogFunction(fwdLogEvent);
        yapiRegisterDeviceLogCallback(fwdDeviceLogEvent);
        yapiRegisterDeviceArrivalCallback(fwdDeviceArrivalEvent);
        yapiRegisterDeviceRemovalCallback(fwdDeviceRemovalEvent);
        yapiRegisterDeviceChangeCallback(fwdDeviceChangeEvent);
        yapiRegisterFunctionUpdateCallback(fwdFunctionUpdateEvent);
    }

    void Yoctopuce::Uninitialize() {
        yapiRegisterLogFunction(NULL);
        yapiRegisterDeviceLogCallback(NULL);
        yapiRegisterDeviceArrivalCallback(NULL);
        yapiRegisterDeviceRemovalCallback(NULL);
        yapiRegisterDeviceChangeCallback(NULL);
        yapiRegisterFunctionUpdateCallback(NULL);
        targetHandle.Clear();
    }

    Handle<Value> Yoctopuce::UpdateDeviceList(const Arguments& args) {
        HandleScope scope;
        char errmsg[YOCTO_ERRMSG_LEN];
        if (yapiUpdateDeviceList(false, errmsg) != YAPI_SUCCESS) {
            Local<String> m1 = String::NewSymbol("UpdateDeviceList failed: ");
            Local<String> m2 = String::Concat(m1, String::New(errmsg));
            return ThrowException(Exception::Error(m2));
        }
        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::HandleEvents(const Arguments& args) {
        HandleScope scope;
        char errmsg[YOCTO_ERRMSG_LEN];
        if (yapiHandleEvents(errmsg) != YAPI_SUCCESS) {
            Local<String> m1 = String::NewSymbol("HandleEvents failed: ");
            Local<String> m2 = String::Concat(m1, String::New(errmsg));
            return ThrowException(Exception::Error(m2));
        }
        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::GetDeviceInfo(const Arguments& args) {
        HandleScope scope;
        yDeviceSt infos;
        char errmsg[YOCTO_ERRMSG_LEN];
        if (yapiGetDeviceInfo(239, &infos, errmsg) != YAPI_SUCCESS) {
            Local<String> m1 = String::NewSymbol("GetDeviceInfo failed: ");
            Local<String> m2 = String::Concat(m1, String::New(errmsg));
            return ThrowException(Exception::Error(m2));
        }
        return scope.Close(Undefined());
    }

    void Yoctopuce::fwdEvent(Event* event) {
        EventBaton* baton = new EventBaton();
        baton->event = event;
        baton->async.data = baton;
        uv_async_init(uv_default_loop(), &baton->async, onEventCallback);
        uv_async_send(&baton->async);
    }

    void Yoctopuce::onEventCallback(uv_async_t *async, int status) {
        EventBaton *baton = static_cast<EventBaton*>(async->data);
        if (!targetHandle.IsEmpty()) {
            baton->event->send(targetHandle);
        }
        uv_handle_t* handle = reinterpret_cast<uv_handle_t*>(&baton->async);
        uv_close(handle, afterEventCallback);
    }

    void Yoctopuce::afterEventCallback(uv_handle_t* handle) {
        EventBaton *baton = static_cast<EventBaton*>(handle->data);
        delete baton->event;
        delete baton;
    }

    void Yoctopuce::fwdLogEvent(const char* log, u32 loglen) {
        Event* ev = new LogEvent(log);
        fwdEvent(ev);
    }

    void Yoctopuce::fwdDeviceLogEvent(YAPI_DEVICE device) {
        Event* ev = new DeviceLogEvent(device);
        fwdEvent(ev);
    }

    void Yoctopuce::fwdDeviceArrivalEvent(YAPI_DEVICE device) {
        Event* ev = new DeviceArrivalEvent(device);
        fwdEvent(ev);
    }

    void Yoctopuce::fwdDeviceRemovalEvent(YAPI_DEVICE device) {
        Event* ev = new DeviceRemovalEvent(device);
        fwdEvent(ev);
    }

    void Yoctopuce::fwdDeviceChangeEvent(YAPI_DEVICE device) {
        Event* ev = new DeviceChangeEvent(device);
        fwdEvent(ev);
    }

    void Yoctopuce::fwdFunctionUpdateEvent(YAPI_FUNCTION fundescr,
        const char *value) {
            Event* ev = new FunctionUpdateEvent(fundescr, value);
            fwdEvent(ev);
    }

}  // namespace node_yoctopuce
