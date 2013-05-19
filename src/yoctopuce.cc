
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
using v8::Number;
using v8::Array;
using v8::Undefined;
using v8::ThrowException;

using std::queue;
using std::swap;

namespace node_yoctopuce {

#define THROW(msg, err) \
    Local<String> m1 = String::NewSymbol(msg); \
    Local<String> m2 = String::Concat(m1, String::New(err)); \
    return ThrowException(Exception::Error(m2));

    uint64_t Yoctopuce::g_main_thread_id;
    Persistent<Object> Yoctopuce::g_target_handle;
    queue<Event*> Yoctopuce::g_event_queue;
    uv_mutex_t Yoctopuce::g_event_queue_mutex;
    uv_async_t Yoctopuce::g_event_async;

    void Yoctopuce::Initialize(Handle<Object> target) {
        HandleScope scope;

        g_target_handle = Persistent<Object>::New(target);

        // Expose API methods
        NODE_SET_METHOD(g_target_handle, "updateDeviceList", UpdateDeviceList);
        NODE_SET_METHOD(g_target_handle, "handleEvents", HandleEvents);
        NODE_SET_METHOD(g_target_handle, "getDevice", GetDevice);
        NODE_SET_METHOD(g_target_handle, "getAllDevices", GetAllDevices);
        NODE_SET_METHOD(g_target_handle, "getDeviceInfo", GetDeviceInfo);
        NODE_SET_METHOD(g_target_handle, "getDevicePath", GetDevicePath);
        NODE_SET_METHOD(g_target_handle, "getFunction", GetFunction);
        NODE_SET_METHOD(g_target_handle, "getFunctionsByClass", GetFunctionsByClass);
        NODE_SET_METHOD(g_target_handle, "getFunctionsByDevice", GetFunctionsByDevice);
        NODE_SET_METHOD(g_target_handle, "getFunctionInfo", GetFunctionInfo);

        // Set up the event queue
        g_main_thread_id = uv_thread_self();
        uv_mutex_init(&g_event_queue_mutex);
        uv_async_init(uv_default_loop(), &g_event_async, onEvent);
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
        g_target_handle.Clear();
    }

    Handle<Value> Yoctopuce::UpdateDeviceList(const Arguments& args) {
        HandleScope scope;
        char errmsg[YOCTO_ERRMSG_LEN];
        if (yapiUpdateDeviceList(false, errmsg) != YAPI_SUCCESS) {
            THROW("UpdateDeviceList failed: ", errmsg)
        }
        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::HandleEvents(const Arguments& args) {
        HandleScope scope;
        char errmsg[YOCTO_ERRMSG_LEN];
        if (yapiHandleEvents(errmsg) != YAPI_SUCCESS) {
            THROW("HandleEvents failed: ", errmsg)
        }
        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::GetDevice(const Arguments& args) {
        HandleScope scope;

        if (args.Length() != 1 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(String::New("Argument 1 must be a string")));
        }
        String::Utf8Value arg(args[0]->ToString());

        char errmsg[YOCTO_ERRMSG_LEN];
        const char* c_arg = *arg;
        YAPI_DEVICE device = yapiGetDevice(c_arg, errmsg);

        if (YISERR(device)) {
            THROW("GetDevice failed: ", errmsg)
        }

        Local<Number> result = Number::New(device);
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetAllDevices(const Arguments& args) {
        HandleScope scope;

        char errmsg[YOCTO_ERRMSG_LEN];
        int elements = 32;
        int init_size = elements * sizeof(YAPI_DEVICE);
        YAPI_DEVICE *devices = new YAPI_DEVICE[elements];
        int ret, required_size;

        ret = yapiGetAllDevices(devices, elements, &required_size, errmsg);
        if (YISERR(ret)) {
            delete[] devices;
            THROW("GetAllDevices failed: ", errmsg);
        }
        if (required_size > init_size) {
            delete [] devices;
            elements = required_size / sizeof(YAPI_DEVICE);
            init_size = elements * sizeof(YAPI_DEVICE);
            devices = new YAPI_DEVICE[elements];
            ret = yapiGetAllDevices(devices, elements, &required_size, errmsg);
            if (YISERR(ret)) {
                delete[] devices;
                THROW("GetAllDevices failed: ", errmsg);
            }
        }

        Local<Array> result = Array::New();
        for (int x = 0; x < ret; x++) {
            result->Set(x, Number::New(devices[x]));
        }
        delete[] devices;
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetDeviceInfo(const Arguments& args) {
        HandleScope scope;

        YAPI_DEVICE deviceId;
        yDeviceSt infos;

        if (args.Length() == 1 && args[0]->IsInt32()) {
            deviceId = args[0]->Int32Value();
        } else {
            return ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer")));
        }

        char errmsg[YOCTO_ERRMSG_LEN];
        if (YISERR(yapiGetDeviceInfo(deviceId, &infos, errmsg))) {
            THROW("GetDeviceInfo failed: ", errmsg);
        }

        Local<Object> result = Object::New();
        result->Set(String::NewSymbol("beacon"), Number::New(infos.beacon));
        result->Set(String::NewSymbol("deviceId"), Number::New(infos.deviceid));
        result->Set(String::NewSymbol("devRelease"), Number::New(infos.devrelease));
        result->Set(String::NewSymbol("firmware"), String::New(infos.firmware));
        result->Set(String::NewSymbol("logicalName"), String::New(infos.logicalname));
        result->Set(String::NewSymbol("manufacturer"), String::New(infos.manufacturer));
        result->Set(String::NewSymbol("nbInterfaces"), Number::New(infos.nbinbterfaces));
        result->Set(String::NewSymbol("productName"), String::New(infos.productname));
        result->Set(String::NewSymbol("serial"), String::New(infos.serial));
        result->Set(String::NewSymbol("vendorId"), Number::New(infos.vendorid));

        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetDevicePath(const Arguments& args) {
        HandleScope scope;

        YAPI_DEVICE device_id;

        if (args.Length() == 1 && args[0]->IsInt32()) {
            device_id = args[0]->Int32Value();
        } else {
            return ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer")));
        }

        char errmsg[YOCTO_ERRMSG_LEN];
        char root_device[YOCTO_SERIAL_LEN];
        int ret, required_size;

        ret = yapiGetDevicePath(device_id, root_device, NULL, 0, &required_size, errmsg);
        if (YISERR(ret)) {
            THROW("GetDevicePath failed: ", errmsg);
        }

        char *sub_path = new char[required_size];
        ret = yapiGetDevicePath(device_id, root_device, sub_path, required_size, NULL, errmsg);
        if (YISERR(ret)) {
            delete sub_path;
            THROW("GetDevicePath failed: ", errmsg);
        }

        Local<Object> result = Object::New();
        result->Set(String::NewSymbol("rootDevice"), String::New(root_device));
        result->Set(String::NewSymbol("subPath"), String::New(sub_path));

        delete sub_path;
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetFunction(const Arguments& args) {
        HandleScope scope;

        if (args.Length() < 1 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(String::New("Argument 1 must be a string")));
        }
        String::Utf8Value class_arg(args[0]->ToString());

        if (args.Length() != 2 || !args[1]->IsString()) {
            return ThrowException(Exception::TypeError(String::New("Argument 2 must be a string")));
        }
        String::Utf8Value fnct_arg(args[1]->ToString());

        char errmsg[YOCTO_ERRMSG_LEN];
        const char* c_class_arg = *class_arg;
        const char* c_fnct_arg = *fnct_arg;

        YAPI_FUNCTION function = yapiGetFunction(c_class_arg, c_fnct_arg, errmsg);

        if (YISERR(function)) {
            THROW("GetDevice failed: ", errmsg)
        }

        Local<Number> result = Number::New(function);
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetFunctionsByClass(const Arguments& args) {
        HandleScope scope;

        YAPI_FUNCTION prev_function = 0;

        if (args.Length() < 1 || !args[0]->IsString()) {
            return ThrowException(Exception::TypeError(String::New("Argument 1 must be a string")));
        }
        String::Utf8Value class_arg(args[0]->ToString());

        if (args.Length() == 2 && args[1]->IsInt32()) {
            prev_function = args[1]->Int32Value();
        } else if (args.Length() == 2) {
            return ThrowException(Exception::TypeError(String::New("Argument 2 must be a number")));
        }

        const char* c_class_arg = *class_arg;

        // TODO(tjg) the yoctopuce wrapper for this method accepts a maxsize
        // arg for paging through the data, however it's not used.
        // Raise this as we bug with them.  Also YAPI_DEVICE is used
        // to calculate init_size rather than YAPI_FUNCTION.

        char errmsg[YOCTO_ERRMSG_LEN];
        int elements = 32;
        int init_size = elements * sizeof(YAPI_FUNCTION);
        YAPI_FUNCTION *functions = new YAPI_FUNCTION[elements];
        int ret, required_size;

        ret = yapiGetFunctionsByClass(c_class_arg, prev_function, functions, init_size, &required_size, errmsg);
        if (YISERR(ret)) {
            delete[] functions;
            THROW("GetFunctionsByClass failed: ", errmsg);
        }
        if (required_size > init_size) {
            delete [] functions;
            elements = required_size / sizeof(YAPI_FUNCTION);
            init_size = elements * sizeof(YAPI_FUNCTION);
            functions = new YAPI_FUNCTION[elements];
            ret = yapiGetFunctionsByClass(c_class_arg, prev_function, functions, init_size, NULL, errmsg);
            if (YISERR(ret)) {
                delete[] functions;
                THROW("GetFunctionsByClass failed: ", errmsg);
            }
        }

        Local<Array> result = Array::New();
        for (int x = 0; x < ret; x++) {
            result->Set(x, Number::New(functions[x]));
        }
        delete[] functions;
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetFunctionsByDevice(const Arguments& args) {
        HandleScope scope;

        YAPI_DEVICE device_id;
        YAPI_FUNCTION prev_function = 0;

        if (args.Length() > 0  && args[0]->IsInt32()) {
            device_id = args[0]->Int32Value();
        } else {
            return ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer")));
        }

        if (args.Length() == 2 && args[1]->IsInt32()) {
            prev_function = args[1]->Int32Value();
        } else if (args.Length() == 2) {
            return ThrowException(Exception::TypeError(String::New("Argument 2 must be an integer")));
        }

        // TODO(tjg) the yoctopuce wrapper for this method accepts a maxsize
        // arg for paging through the data, however it's not used.
        // Raise this as we bug with them.  Also YAPI_DEVICE is used
        // to calculate init_size rather than YAPI_FUNCTION.

        char errmsg[YOCTO_ERRMSG_LEN];
        int elements = 32;
        int init_size = elements * sizeof(YAPI_FUNCTION);
        YAPI_FUNCTION *functions = new YAPI_FUNCTION[elements];
        int ret, required_size;

        ret = yapiGetFunctionsByDevice(device_id, prev_function, functions, init_size, &required_size, errmsg);
        if (YISERR(ret)) {
            delete[] functions;
            THROW("GetFunctionsByDevice failed: ", errmsg);
        }
        if (required_size > init_size) {
            delete [] functions;
            elements = required_size / sizeof(YAPI_FUNCTION);
            init_size = elements * sizeof(YAPI_FUNCTION);
            functions = new YAPI_FUNCTION[elements];
            ret = yapiGetFunctionsByDevice(device_id, prev_function, functions, init_size, NULL, errmsg);
            if (YISERR(ret)) {
                delete[] functions;
                THROW("GetFunctionsByDevice failed: ", errmsg);
            }
        }

        Local<Array> result = Array::New();
        for (int x = 0; x < ret; x++) {
            result->Set(x, Number::New(functions[x]));
        }
        delete[] functions;
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetFunctionInfo(const Arguments& args) {
        HandleScope scope;

        YAPI_FUNCTION arg;

        if (args.Length() == 1 && args[0]->IsInt32()) {
            arg = args[0]->Int32Value();
        } else {
            return ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer")));
        }

        YAPI_DEVICE device;
        char errmsg[YOCTO_ERRMSG_LEN];
        char function_serial[YOCTO_SERIAL_LEN];
        char function_id[YOCTO_FUNCTION_LEN];
        char function_logical_name[YOCTO_LOGICAL_LEN];
        char function_value[YOCTO_PUBVAL_LEN];

        if (YISERR(yapiGetFunctionInfo(arg, &device, function_serial, function_id,
            function_logical_name, function_value, errmsg))) {
                THROW("GetDeviceInfo failed: ", errmsg);
        }

        Local<Object> result = Object::New();
        result->Set(String::NewSymbol("device"), Number::New(device));
        result->Set(String::NewSymbol("serial"), String::New(function_serial));
        result->Set(String::NewSymbol("functionId"), String::New(function_id));
        result->Set(String::NewSymbol("logicalName"), String::New(function_logical_name));
        result->Set(String::NewSymbol("value"), String::New(function_value));

        return scope.Close(result);
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
        if (g_main_thread_id == uv_thread_self()) {
            // Dispatch the event if we are already on the main thread
            event->dispatch(g_target_handle);
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

            // Free the event loop;
            uv_unref(reinterpret_cast<uv_handle_t*>(&g_event_async));
        }
        delete event;
    }

    void Yoctopuce::onEvent(uv_async_t *async, int status) {
        // Snapshot the queued events.
        queue<Event*> events;
        uv_mutex_lock(&g_event_queue_mutex);
        swap(g_event_queue, events);
        uv_mutex_unlock(&g_event_queue_mutex);

        // Dispatch the events
        while (!events.empty()) {
            events.front()->dispatch(g_target_handle);
            events.pop();
        }
    }

}  // namespace node_yoctopuce
