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

#include "./yoctopuce.h"

using v8::HandleScope;
using v8::Persistent;
using v8::Exception;
using v8::String;
using v8::Number;
using v8::Array;
using v8::Boolean;
using v8::Local;
using v8::Object;
using v8::Handle;
using v8::Arguments;
using v8::Value;
using v8::Undefined;
using v8::ThrowException;

using node::SetMethod;

using std::queue;
using std::swap;

namespace node_yoctopuce {

    Persistent<Object> Yoctopuce::target_handle;
    queue<Event*> Yoctopuce::event_queue;
    uv_mutex_t Yoctopuce::event_queue_mutex;
    uv_async_t Yoctopuce::event_async;
    uv_mutex_t Yoctopuce::http_request_mutex;

    void Yoctopuce::Initialize(Handle<Object> target) {
        HandleScope scope;

        target_handle = Persistent<Object>::New(target);

        // Expose API methods
        SetMethod(target_handle, "updateDeviceList", UpdateDeviceList);
        SetMethod(target_handle, "handleEvents", HandleEvents);
        SetMethod(target_handle, "registerHub", RegisterHub);
        SetMethod(target_handle, "preregisterHub", PreregisterHub);
        SetMethod(target_handle, "unregisterHub", UnregisterHub);
        SetMethod(target_handle, "checkLogicalName", CheckLogicalName);
        SetMethod(target_handle, "getApiVersion", GetApiVersion);
        SetMethod(target_handle, "getDevice", GetDevice);
        SetMethod(target_handle, "getAllDevices", GetAllDevices);
        SetMethod(target_handle, "getDeviceInfo", GetDeviceInfo);
        SetMethod(target_handle, "getDevicePath", GetDevicePath);
        SetMethod(target_handle, "getFunction", GetFunction);
        SetMethod(target_handle, "getFunctionsByClass", GetFunctionsByClass);
        SetMethod(target_handle, "getFunctionsByDevice", GetFunctionsByDevice);
        SetMethod(target_handle, "getFunctionInfo", GetFunctionInfo);
        SetMethod(target_handle, "httpRequest", HttpRequest);
        SetMethod(target_handle, "httpRequestAsync", HttpRequestAsync);

        usb_symbol = NODE_PSYMBOL("usb");
        target_handle->Set(usb_symbol, usb_symbol);

        // Set up the event queue
        uv_mutex_init(&event_queue_mutex);
        uv_async_init(uv_default_loop(), &event_async, OnEvent);
        uv_unref(reinterpret_cast<uv_handle_t*>(&event_async));

        // Set up for http requests
        uv_mutex_init(&http_request_mutex);

        // Initialise YAPI.
        char errmsg[YOCTO_ERRMSG_LEN];
        if (yapiInitAPI(Y_DETECT_USB, errmsg) != YAPI_SUCCESS
            || yapiUpdateDeviceList(true, errmsg) != YAPI_SUCCESS) {
                THROW("Unable to initialize yapi. ", errmsg, ex);
                scope.Close(ex);
                return;
        }

        // Set up to handle device events
        yapiRegisterLogFunction(FwdLogEvent);
        yapiRegisterDeviceLogCallback(FwdDeviceLogEvent);
        yapiRegisterDeviceArrivalCallback(FwdDeviceArrivalEvent);
        yapiRegisterDeviceRemovalCallback(FwdDeviceRemovalEvent);
        yapiRegisterDeviceChangeCallback(FwdDeviceChangeEvent);
        yapiRegisterFunctionUpdateCallback(FwdFunctionUpdateEvent);
    }

    void Yoctopuce::Close() {
        // Stop receiving events
        yapiRegisterLogFunction(NULL);
        yapiRegisterDeviceLogCallback(NULL);
        yapiRegisterDeviceArrivalCallback(NULL);
        yapiRegisterDeviceRemovalCallback(NULL);
        yapiRegisterDeviceChangeCallback(NULL);
        yapiRegisterFunctionUpdateCallback(NULL);

        // Clean up
        uv_close(reinterpret_cast<uv_handle_t*>(&event_async), NULL);
        uv_mutex_destroy(&event_queue_mutex);
        uv_mutex_destroy(&http_request_mutex);
        target_handle.Clear();

        // Free the API.
        yapiFreeAPI();
    }

    Handle<Value> Yoctopuce::UpdateDeviceList(const Arguments& args) {
        HandleScope scope;
        char errmsg[YOCTO_ERRMSG_LEN];
        if (YISERR(yapiUpdateDeviceList(false, errmsg))) {
            THROW("UpdateDeviceList failed: ", errmsg, ex);
            return scope.Close(ex);
        }
        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::HandleEvents(const Arguments& args) {
        HandleScope scope;
        char errmsg[YOCTO_ERRMSG_LEN];
        if (YISERR(yapiHandleEvents(errmsg))) {
            THROW("HandleEvents failed: ", errmsg, ex);
            return scope.Close(ex);
        }
        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::RegisterHub(const Arguments& args) {
        HandleScope scope;

        if (args.Length() != 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value hub_arg(args[0]->ToString());

        const char *hub = *hub_arg;

        char errmsg[YOCTO_ERRMSG_LEN];
        if (YISERR(yapiRegisterHub(hub, errmsg))) {
            THROW("RegisterHub failed: ", errmsg, ex);
            return scope.Close(ex);
        }

        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::PreregisterHub(const Arguments& args) {
        HandleScope scope;

        if (args.Length() != 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value hub_arg(args[0]->ToString());

        const char *hub = *hub_arg;

        char errmsg[YOCTO_ERRMSG_LEN];
        if (YISERR(yapiPreregisterHub(hub, errmsg))) {
            THROW("PreregisterHub failed: ", errmsg, ex);
            return scope.Close(ex);
        }
        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::UnregisterHub(const Arguments& args) {
        HandleScope scope;

        if (args.Length() != 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value hub_arg(args[0]->ToString());

        const char *hub = *hub_arg;
        yapiUnregisterHub(hub);

        return scope.Close(Undefined());
    }

    Handle<Value> Yoctopuce::CheckLogicalName(const Arguments& args) {
        HandleScope scope;

        if (args.Length() != 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value name_arg(args[0]->ToString());

        const char* name= *name_arg;
        bool ret = !!yapiCheckLogicalName(name);

        return scope.Close(Boolean::New(ret));
    }

    Handle<Value> Yoctopuce::GetApiVersion(const Arguments& args) {
        HandleScope scope;

        const char *version, *date;
        u16 ret = yapiGetAPIVersion(&version, &date);

        Local<Object> result = Object::New();
        result->Set(String::NewSymbol("apiBcdVersion"), Number::New(ret));
        result->Set(String::NewSymbol("apiVersion"), String::New(version));
        result->Set(String::NewSymbol("apiDate"), String::New(date));

        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetDevice(const Arguments& args) {
        HandleScope scope;

        if (args.Length() != 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value device_arg(args[0]->ToString());

        char errmsg[YOCTO_ERRMSG_LEN];
        const char* device = *device_arg;
        YAPI_DEVICE device_handle;

        if (YISERR(device_handle = yapiGetDevice(device, errmsg))) {
            THROW("GetDevice failed: ", errmsg, ex);
            return scope.Close(ex);
        }

        Local<Number> result = Number::New(device_handle);
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetAllDevices(const Arguments& args) {
        HandleScope scope;

        char errmsg[YOCTO_ERRMSG_LEN];
        int elements = 32;
        int init_size = elements * sizeof(YAPI_DEVICE);
        YAPI_DEVICE *devices = new YAPI_DEVICE[elements];
        int ret, required_size;

        if (YISERR(ret = yapiGetAllDevices(devices, elements,
            &required_size, errmsg))) {
                delete[] devices;
                THROW("GetAllDevices failed: ", errmsg, ex);
                return scope.Close(ex);
        }
        if (required_size > init_size) {
            delete [] devices;
            elements = required_size / sizeof(YAPI_DEVICE);
            init_size = elements * sizeof(YAPI_DEVICE);
            devices = new YAPI_DEVICE[elements];
            if (YISERR(ret = yapiGetAllDevices(devices, elements,
                &required_size, errmsg))) {
                    delete[] devices;
                    THROW("GetAllDevices failed: ", errmsg, ex);
                    return scope.Close(ex);
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
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer"))));
        }

        char errmsg[YOCTO_ERRMSG_LEN];
        if (YISERR(yapiGetDeviceInfo(deviceId, &infos, errmsg))) {
            THROW("GetDeviceInfo failed: ", errmsg, ex);
            return scope.Close(ex);
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
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer"))));
        }

        char errmsg[YOCTO_ERRMSG_LEN];
        char root_device[YOCTO_SERIAL_LEN];
        int required_size;

        if (YISERR(yapiGetDevicePath(device_id, root_device, NULL,
            0, &required_size, errmsg))) {
                THROW("GetDevicePath failed: ", errmsg, ex);
                return scope.Close(ex);
        }

        char *sub_path = new char[required_size];

        if (YISERR(yapiGetDevicePath(device_id, root_device, sub_path,
            required_size, NULL, errmsg))) {
                delete sub_path;
                THROW("GetDevicePath failed: ", errmsg, ex);
                return scope.Close(ex);
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
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value function_class_arg(args[0]->ToString());

        if (args.Length() != 2 || !args[1]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 2 must be a string"))));
        }
        String::Utf8Value function_arg(args[1]->ToString());

        char errmsg[YOCTO_ERRMSG_LEN];
        const char* function_class = *function_class_arg;
        const char* function = *function_arg;
        YAPI_FUNCTION function_handle;

        if (YISERR(function_handle = yapiGetFunction(function_class, function, errmsg))) {
            THROW("GetFunction failed: ", errmsg, ex);
            return scope.Close(ex);
        }

        Local<Number> result = Number::New(function_handle);
        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::GetFunctionsByClass(const Arguments& args) {
        HandleScope scope;

        YAPI_FUNCTION prev_function = 0;

        if (args.Length() < 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value function_class_arg(args[0]->ToString());

        if (args.Length() == 2 && args[1]->IsInt32()) {
            prev_function = args[1]->Int32Value();
        } else if (args.Length() == 2) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 2 must be a number"))));
        }

        const char* function_class= *function_class_arg;

        // TODO(tjg) the yoctopuce wrapper for this method accepts a maxsize
        // arg for paging through the data, however it's not used.
        // Raise this as we bug with them.  Also YAPI_DEVICE is used
        // to calculate init_size rather than YAPI_FUNCTION.

        char errmsg[YOCTO_ERRMSG_LEN];
        int elements = 32;
        int init_size = elements * sizeof(YAPI_FUNCTION);
        YAPI_FUNCTION *functions = new YAPI_FUNCTION[elements];
        int ret, required_size;

        if (YISERR(ret = yapiGetFunctionsByClass(function_class, prev_function,
            functions, init_size, &required_size, errmsg))) {
                delete[] functions;
                THROW("GetFunctionsByClass failed: ", errmsg, ex);
                return scope.Close(ex);
        }
        if (required_size > init_size) {
            delete [] functions;
            elements = required_size / sizeof(YAPI_FUNCTION);
            init_size = elements * sizeof(YAPI_FUNCTION);
            functions = new YAPI_FUNCTION[elements];
            if (YISERR(ret = yapiGetFunctionsByClass(function_class, prev_function,
                functions, init_size, NULL, errmsg))) {
                    delete[] functions;
                    THROW("GetFunctionsByClass failed: ", errmsg, ex);
                    return scope.Close(ex);
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
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer"))));
        }

        if (args.Length() == 2 && args[1]->IsInt32()) {
            prev_function = args[1]->Int32Value();
        } else if (args.Length() == 2) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 2 must be an integer"))));
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

        if (YISERR(ret = yapiGetFunctionsByDevice(device_id, prev_function,
            functions, init_size, &required_size, errmsg))) {
                delete[] functions;
                THROW("GetFunctionsByDevice failed: ", errmsg, ex);
                return scope.Close(ex);
        }
        if (required_size > init_size) {
            delete [] functions;
            elements = required_size / sizeof(YAPI_FUNCTION);
            init_size = elements * sizeof(YAPI_FUNCTION);
            functions = new YAPI_FUNCTION[elements];
            if (YISERR(ret = yapiGetFunctionsByDevice(device_id, prev_function,
                functions, init_size, NULL, errmsg))) {
                    delete[] functions;
                    THROW("GetFunctionsByDevice failed: ", errmsg, ex);
                    return scope.Close(ex);
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
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be an integer"))));
        }

        YAPI_DEVICE device;
        char errmsg[YOCTO_ERRMSG_LEN];
        char function_serial[YOCTO_SERIAL_LEN];
        char function_id[YOCTO_FUNCTION_LEN];
        char function_logical_name[YOCTO_LOGICAL_LEN];
        char function_value[YOCTO_PUBVAL_LEN];

        if (YISERR(yapiGetFunctionInfo(arg, &device, function_serial, function_id,
            function_logical_name, function_value, errmsg))) {
                THROW("GetFunctionInfo failed: ", errmsg, ex);
                return scope.Close(ex);
        }

        Local<Object> result = Object::New();
        result->Set(String::NewSymbol("device"), Number::New(device));
        result->Set(String::NewSymbol("serial"), String::New(function_serial));
        result->Set(String::NewSymbol("functionId"), String::New(function_id));
        result->Set(String::NewSymbol("logicalName"), String::New(function_logical_name));
        result->Set(String::NewSymbol("value"), String::New(function_value));

        return scope.Close(result);
    }

    Handle<Value> Yoctopuce::HttpRequest(const Arguments& args) {
        HandleScope scope;

        if (args.Length() < 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value device_arg(args[0]->ToString());

        if (args.Length() < 2 || !args[1]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 2 must be a string"))));
        }
        String::Utf8Value request_arg(args[1]->ToString());

        char errmsg[YOCTO_ERRMSG_LEN];
        const char *device = *device_arg;
        const char *request = *request_arg;
        int request_size = request_arg.length();
        char *reply;
        int reply_size;
        YIOHDL request_handle;

        uv_mutex_lock(&http_request_mutex);
        if (YISERR(yapiHTTPRequestSyncStartEx(&request_handle, device,
            request, request_size, &reply, &reply_size, errmsg))) {
                THROW("HttpRequest failed: ", errmsg, ex);
                uv_mutex_unlock(&http_request_mutex);
                return scope.Close(ex);
        }
        Local<String> reply_arg = String::New(reply, reply_size);
        if (YISERR(yapiHTTPRequestSyncDone(&request_handle, errmsg))) {
            THROW("HttpRequest failed: ", errmsg, ex);
            uv_mutex_unlock(&http_request_mutex);
            return scope.Close(ex);
        }
        uv_mutex_unlock(&http_request_mutex);

        return scope.Close(reply_arg);
    }

    void Yoctopuce::FwdLogEvent(const char* log, u32 loglen) {
        FwdEvent(new LogEvent(log));
    }

    void Yoctopuce::FwdDeviceLogEvent(YAPI_DEVICE device) {
        FwdEvent(new DeviceLogEvent(device));
    }

    void Yoctopuce::FwdDeviceArrivalEvent(YAPI_DEVICE device) {
        FwdEvent(new DeviceArrivalEvent(device));
    }

    void Yoctopuce::FwdDeviceRemovalEvent(YAPI_DEVICE device) {
        FwdEvent(new DeviceRemovalEvent(device));
    }

    void Yoctopuce::FwdDeviceChangeEvent(YAPI_DEVICE device) {
        FwdEvent(new DeviceChangeEvent(device));
    }

    void Yoctopuce::FwdFunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *value) {
        FwdEvent(new FunctionUpdateEvent(fundescr, value));
    }

    void Yoctopuce::FwdEvent(Event* event) {
        // Push the event to the queue
        uv_mutex_lock(&event_queue_mutex);
        event_queue.push(event);
        uv_mutex_unlock(&event_queue_mutex);

        // Hold the event loop open while this is executing
        uv_ref(reinterpret_cast<uv_handle_t*>(&event_async));

        // Send a message to our main thread to wake up the loop
        uv_async_send(&event_async);
    }

    void Yoctopuce::OnEvent(uv_async_t *async, int status) {
        // Snapshot the queued events.
        queue<Event*> events;
        uv_mutex_lock(&event_queue_mutex);
        swap(event_queue, events);
        uv_mutex_unlock(&event_queue_mutex);

        // Dispatch & delete the events
        while (!events.empty()) {
            Event *event = events.front();
            events.pop();
            event->Dispatch(target_handle);
            delete event;
            uv_unref(reinterpret_cast<uv_handle_t*>(&event_async));
        }
    }

}  // namespace node_yoctopuce
