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

#ifndef NODE_YOCTOPUCE_H
#define NODE_YOCTOPUCE_H

#include <v8.h>
#include <node.h>
#include <yapi.h>

#include "async.h"

using namespace v8;
using namespace node;

namespace node_yoctopuce 
{
	class Yoctopuce : public ObjectWrap
	{

	public:

		static void Initialize(Handle<Object> target);
		static void Uninitialize();

		typedef Async<std::string> AsyncLogCallback;
		typedef Async<YAPI_DEVICE> AsyncDeviceUpdateCallback;
		typedef Async<std::string> AsyncFunctionChangeCallback;

		static AsyncLogCallback* log_event;
		static AsyncDeviceUpdateCallback* device_log_event;
		static AsyncDeviceUpdateCallback* device_arrival_event;
		static AsyncDeviceUpdateCallback* device_removal_event;
		static AsyncDeviceUpdateCallback* device_change_event;
		static AsyncFunctionChangeCallback* function_change_event;

	protected:	

		static Persistent<Object> event_context;

		static Persistent<String> events_symbol;
		static Persistent<String> log_symbol;
		static Persistent<String> device_log_symbol;
		static Persistent<String> device_arrival_symbol;
		static Persistent<String> device_removal_symbol;
		static Persistent<String> device_change_symbol;
		static Persistent<String> function_change_symbol;

		static Handle<Value> UpdateDeviceList(const Arguments& args);
		static Handle<Value> HandleEvents(const Arguments& args);
		static Handle<Value> GetDeviceInfo(const Arguments& args);

		static void EmitEvent(Handle<String>eventName, int argc, Handle<Value> args[]);

		static void LogCallback(std::string log);
		static void DeviceLogCallback(YAPI_DEVICE device);
		static void DeviceArrivalCallback(YAPI_DEVICE device);
		static void DeviceRemovalCallback(YAPI_DEVICE device);
		static void DeviceChangeCallback(YAPI_DEVICE device);
		static void FunctionChangeCallback(std::string value);

		Yoctopuce() {};

	};
}

#endif