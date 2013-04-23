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

#include <iostream>
#include <sstream>
#include <node.h>
#include <v8.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <yapi.h>
#include <algorithm>

#include "yoctopuce.h"
#include "async.h"

using namespace std;
using namespace v8;
using namespace node;

namespace node_yoctopuce 
{

	Persistent<Object> Yoctopuce::event_context;
	Persistent<String> Yoctopuce::events_symbol;
	Persistent<String> Yoctopuce::log_symbol;
	Persistent<String> Yoctopuce::device_log_symbol;
	Persistent<String> Yoctopuce::device_arrival_symbol;
	Persistent<String> Yoctopuce::device_removal_symbol;
	Persistent<String> Yoctopuce::device_change_symbol;
	Persistent<String> Yoctopuce::function_change_symbol;

	Yoctopuce::AsyncLogCallback* Yoctopuce::log_event;
	Yoctopuce::AsyncDeviceUpdateCallback* Yoctopuce::device_log_event;
	Yoctopuce::AsyncDeviceUpdateCallback* Yoctopuce::device_arrival_event;
	Yoctopuce::AsyncDeviceUpdateCallback* Yoctopuce::device_removal_event;
	Yoctopuce::AsyncDeviceUpdateCallback* Yoctopuce::device_change_event;
	Yoctopuce::AsyncFunctionChangeCallback* Yoctopuce::function_change_event;

	void Yoctopuce::Initialize(Handle<Object> target)
	{

		HandleScope scope;

		events_symbol = NODE_PSYMBOL("events");
		log_symbol = NODE_PSYMBOL("onLog");
		device_log_symbol = NODE_PSYMBOL("onDeviceLog");
		device_arrival_symbol = NODE_PSYMBOL("onDeviceArrival");
		device_removal_symbol = NODE_PSYMBOL("onDeviceRemoval");
		device_change_symbol = NODE_PSYMBOL("onDeviceChange");
		function_change_symbol = NODE_PSYMBOL("onFunctionChange");

		event_context = Persistent<Object>::New(Object::New());
		event_context->Set(log_symbol, Function::New());
		event_context->Set(device_log_symbol, Function::New()); 
		event_context->Set(device_arrival_symbol, Function::New()); 
		event_context->Set(device_removal_symbol, Function::New()); 
		event_context->Set(device_change_symbol, Function::New()); 
		event_context->Set(function_change_symbol, Function::New());

		target->Set(events_symbol, event_context);

		NODE_SET_METHOD(target, "updateDeviceList", UpdateDeviceList);
		NODE_SET_METHOD(target, "handleEvents", HandleEvents);
		NODE_SET_METHOD(target, "getDeviceInfo", GetDeviceInfo);

		log_event = new AsyncLogCallback(LogCallback);
		device_log_event = new AsyncDeviceUpdateCallback(DeviceLogCallback);
		device_arrival_event = new AsyncDeviceUpdateCallback(DeviceArrivalCallback);
		device_removal_event = new AsyncDeviceUpdateCallback(DeviceRemovalCallback);
		device_change_event = new AsyncDeviceUpdateCallback(DeviceChangeCallback);
		function_change_event = new AsyncFunctionChangeCallback(FunctionChangeCallback);

	}

	void Yoctopuce::Uninitialize()
	{
		if(log_event)
		{
			log_event -> finish();
			log_event = NULL;
		}
		if(device_log_event)
		{
			device_log_event -> finish();
			device_log_event = NULL;
		}
		if(device_arrival_event)
		{
			device_arrival_event -> finish();
			device_arrival_event = NULL;
		}
		if(device_removal_event)
		{
			device_removal_event -> finish();
			device_removal_event = NULL;
		}
		if(device_change_event)
		{
			device_change_event -> finish();
			device_change_event = NULL;
		}
		if(function_change_event)
		{
			function_change_event -> finish();
			function_change_event = NULL;
		}
	}

	Handle<Value> Yoctopuce::UpdateDeviceList(const Arguments& args)
	{
		HandleScope scope;

		char errmsg[YOCTO_ERRMSG_LEN];
		if(yapiUpdateDeviceList(false, errmsg) != YAPI_SUCCESS)
		{
			ostringstream os;
			os << "Unable to update the device list. yapiUpdateDeviceList failed: " << errmsg;
			return ThrowException(Exception::Error(String::New(os.str().c_str())));
		}
		return scope.Close(Undefined());
	}

	Handle<Value> Yoctopuce::HandleEvents(const Arguments& args)
	{
		HandleScope scope;

		char errmsg[YOCTO_ERRMSG_LEN];
		if(yapiHandleEvents(errmsg) != YAPI_SUCCESS)
		{
			ostringstream os;
			os << "Unable to handle events. yapiHandleEvents failed: " << errmsg;
			return ThrowException(Exception::Error(String::New(os.str().c_str())));
		}
		return scope.Close(Undefined());
	}

	Handle<Value> Yoctopuce::GetDeviceInfo(const Arguments& args)
	{
		HandleScope scope;
		yDeviceSt infos;
		char errmsg[YOCTO_ERRMSG_LEN];
		if(yapiGetDeviceInfo(239, &infos, errmsg) != YAPI_SUCCESS)
		{
			ostringstream os;
			os << "Unable to get device info. yapiHandleEvents failed: " << errmsg;
			return ThrowException(Exception::Error(String::New(os.str().c_str())));
		}
		cout << infos.logicalname;
		return scope.Close(Undefined());
	}

	void Yoctopuce::EmitEvent(Handle<String> event, int argc, Handle<Value> argv[]) 
	{
		HandleScope scope;
		if(event_context->Get(event)->IsFunction())
		{
			MakeCallback(event_context, event, argc, argv);
		}
	}

	void Yoctopuce::LogCallback(std::string log)
	{
		log.erase(std::remove(log.begin(), log.end(), '\n'), log.end());
		log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());
		HandleScope scope;
		Handle<Value> argv[1] = {String::New(log.c_str())};
		EmitEvent(log_symbol, 1, argv);
	}

	void Yoctopuce::DeviceLogCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(device_log_symbol, 1, argv);
	}

	void Yoctopuce::DeviceArrivalCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(device_arrival_symbol, 1, argv);
	}

	void Yoctopuce::DeviceRemovalCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(device_removal_symbol, 1, argv);
	}

	void Yoctopuce::DeviceChangeCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(device_change_symbol, 1, argv);
	}

	void Yoctopuce::FunctionChangeCallback(std::string value)
	{
		value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
		value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
		HandleScope scope;
		Handle<Value> argv[1] = {String::New(value.c_str())};
		EmitEvent(function_change_symbol, 1, argv);
	}
}