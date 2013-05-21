
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

#include <node.h>
#include <v8.h>
#include <yapi.h>
#include <string>
#include <algorithm>

using v8::Handle;
using v8::Object;
using v8::Value;

using std::string;

namespace node_yoctopuce {

    class Event {
    public:
        virtual void dispatch(Handle<Object> context)=0;
        virtual ~Event();
    protected:
        virtual void dispatchToV8(Handle<Object> context, int argc, Handle<Value> argv[]);
    };

    class CharDataEvent : public Event {
    public:
        explicit CharDataEvent(const char *_data);
    protected:
        string data;
    };

    class LogEvent : public CharDataEvent {
    public:
        explicit LogEvent(const char *data);
        virtual void dispatch(Handle<Object> context);
    };

    class FunctionUpdateEvent : public CharDataEvent {
    public:
        FunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *data);
        virtual void dispatch(Handle<Object> context);
    protected:
        YAPI_FUNCTION fundescr;
    };

    class DeviceEvent : public Event {
    public:
        explicit DeviceEvent(const char* name, YAPI_DEVICE device);
        virtual void dispatch(Handle<Object> context);
    protected:
        YAPI_DEVICE device;
        const char* name;
    };

    class DeviceLogEvent : public DeviceEvent {
    public:
        explicit DeviceLogEvent(YAPI_DEVICE device);
    };

    class DeviceArrivalEvent : public DeviceEvent {
    public:
        explicit DeviceArrivalEvent(YAPI_DEVICE device);
    };

    class DeviceChangeEvent : public DeviceEvent {
    public:
        explicit DeviceChangeEvent(YAPI_DEVICE device);
    };

    class DeviceRemovalEvent : public DeviceEvent {
    public:
        explicit DeviceRemovalEvent(YAPI_DEVICE device);
    };

}  // namespace node_yoctopuce

#endif  // SRC_EVENTS_H_
