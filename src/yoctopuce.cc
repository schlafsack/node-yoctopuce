
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
    queue<Event*> Yoctopuce::g_event_queue;
    unsigned long Yoctopuce::g_main_thread_id;
    uv_mutex_t Yoctopuce::g_event_queue_mutex;
    uv_async_t Yoctopuce::g_event_async;

    void Yoctopuce::Initialize(Handle<Object> target) {
        HandleScope scope;

        // Expose API methods
        targetHandle = Persistent<Object>::New(target);
        NODE_SET_METHOD(targetHandle, "updateDeviceList", UpdateDeviceList);
        NODE_SET_METHOD(targetHandle, "handleEvents", HandleEvents);
        NODE_SET_METHOD(targetHandle, "getDeviceInfo", GetDeviceInfo);

        // Set up the event queue
        g_main_thread_id = uv_thread_self();
        uv_mutex_init(&g_event_queue_mutex);
        uv_async_init(uv_default_loop(), &g_event_async, onEventCallback);
        uv_unref(reinterpret_cast<uv_handle_t*>(&g_event_async));

        // Set up to handle device events
        yapiRegisterLogFunction(fwdLogEvent);
        yapiRegisterDeviceLogCallback(fwdDeviceLogEvent);
        yapiRegisterDeviceArrivalCallback(fwdDeviceArrivalEvent);
        yapiRegisterDeviceRemovalCallback(fwdDeviceRemovalEvent);
        yapiRegisterDeviceChangeCallback(fwdDeviceChangeEvent);
        yapiRegisterFunctionUpdateCallback(fwdFunctionUpdateEvent);
    }

    void Yoctopuce::Uninitialize() {
        HandleScope scope;

        // Stop receiving events
        yapiRegisterLogFunction(NULL);
        yapiRegisterDeviceLogCallback(NULL);
        yapiRegisterDeviceArrivalCallback(NULL);
        yapiRegisterDeviceRemovalCallback(NULL);
        yapiRegisterDeviceChangeCallback(NULL);
        yapiRegisterFunctionUpdateCallback(NULL);

        // Clean up
        uv_close(reinterpret_cast<uv_handle_t*>(&g_event_async), NULL);
        uv_mutex_destroy(&g_event_queue_mutex);
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

    void Yoctopuce::fwdLogEvent(const char* log, u32 loglen) {
        fwdEvent(new LogEvent(log));
    }

    void Yoctopuce::fwdDeviceLogEvent(YAPI_DEVICE device) {
        fwdEvent(new DeviceLogEvent(device));
    }

    void Yoctopuce::fwdDeviceArrivalEvent(YAPI_DEVICE device) {
        fwdEvent(new DeviceArrivalEvent(device));
    }

    void Yoctopuce::fwdDeviceRemovalEvent(YAPI_DEVICE device) {
        fwdEvent(new DeviceRemovalEvent(device));
    }

    void Yoctopuce::fwdDeviceChangeEvent(YAPI_DEVICE device) {
        fwdEvent(new DeviceChangeEvent(device));
    }

    void Yoctopuce::fwdFunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *value) {
        fwdEvent(new FunctionUpdateEvent(fundescr, value));
    }

    void Yoctopuce::fwdEvent(Event* event) {
        // Dispatch the event if we are already on the main thread
        if(g_main_thread_id == uv_thread_self()) {
            if(!targetHandle.IsEmpty()) {
                event->dispatch(targetHandle);
            }
        } else {
            // Otherwise push it to the queue
            uv_mutex_lock(&g_event_queue_mutex);
            g_event_queue.push(event);
            uv_mutex_unlock(&g_event_queue_mutex);

            // Hold the event loop open while this is executing
            uv_ref(reinterpret_cast<uv_handle_t*>(&g_event_async));

            // Send a message to our main thread to wake up the loop
            uv_async_send(&g_event_async);

            // Wait for the event to be dispatched to v8;
            event->waitOnDispatch();

            // Free the event loop and clean up;
            uv_unref(reinterpret_cast<uv_handle_t*>(&g_event_async));
        }
        delete event;
    }

    void Yoctopuce::onEventCallback(uv_async_t *async, int status) {
        dispatchEvents();
    }

    void Yoctopuce::dispatchEvents() {
        HandleScope scope;

        // Dequeue the events.
        queue<Event*> events;
        uv_mutex_lock(&g_event_queue_mutex);
        swap(g_event_queue, events);
        uv_mutex_unlock(&g_event_queue_mutex);

        // Dispatch the events and signal any waiting threads to continue
        while(!events.empty()) {
            Event *event = events.front();
            events.pop();
            if(!targetHandle.IsEmpty()) {
                event->dispatch(targetHandle);
            }
            event->signalDispatch();
        }
    }

}  // namespace node_yoctopuce
