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

#include "yoctopuce.h"

using namespace std;
using namespace v8;
using namespace node;

namespace node_yoctopuce 
{

	Persistent<Object> Yoctopuce::event_context;
	Persistent<String> Yoctopuce::events_symbol;
	Persistent<String> Yoctopuce::log_symbol;
	Persistent<String> Yoctopuce::devicelog_symbol;
	Persistent<String> Yoctopuce::devicearrival_symbol;
	Persistent<String> Yoctopuce::deviceremoval_symbol;
	Persistent<String> Yoctopuce::devicechange_symbol;

	void Yoctopuce::Initialize(Handle<Object> target)
	{
		
		node::AtExit(Yoctopuce::Deinitialize, 0);

		char errmsg[YOCTO_ERRMSG_LEN];
		if(yapiInitAPI(Y_DETECT_USB,errmsg) != YAPI_SUCCESS
			|| yapiUpdateDeviceList(true, errmsg) != YAPI_SUCCESS)
		{
			cerr << "Unable to initialize yapi. " << errmsg << endl;
			abort();
		}

		HandleScope scope;

		events_symbol = NODE_PSYMBOL("events");
		log_symbol = NODE_PSYMBOL("onLog");
		devicelog_symbol = NODE_PSYMBOL("onDeviceLog");
		devicearrival_symbol = NODE_PSYMBOL("onDeviceArrival");
		deviceremoval_symbol = NODE_PSYMBOL("onDeviceRemoval");
		devicechange_symbol = NODE_PSYMBOL("onDeviceChange");

		event_context = Persistent<Object>::New(Object::New());
		event_context->Set(devicelog_symbol, Object::New()); 
		event_context->Set(devicearrival_symbol, Object::New()); 
		event_context->Set(deviceremoval_symbol, Object::New()); 
		event_context->Set(devicechange_symbol, Object::New()); 

		target->Set(events_symbol, event_context);
		
		yapiRegisterLogFunction(Yoctopuce::LogCallback );
		yapiRegisterDeviceLogCallback(Yoctopuce::DeviceLogCallback);
		yapiRegisterDeviceArrivalCallback(Yoctopuce::DeviceArrivalCallback);
		yapiRegisterDeviceRemovalCallback(Yoctopuce::DeviceRemovalCallback);
		yapiRegisterDeviceChangeCallback(Yoctopuce::DeviceChangeCallback);

		NODE_SET_METHOD(target, "updateDeviceList", Yoctopuce::UpdateDeviceList);
		NODE_SET_METHOD(target, "handleEvents", Yoctopuce::HandleEvents);
		NODE_SET_METHOD(target, "getDeviceInfo", Yoctopuce::GetDeviceInfo);

	}

	void Yoctopuce::Deinitialize(void*)
	{
		yapiFreeAPI();
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

	void Yoctopuce::LogCallback(const char *log, u32 loglen)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {String::New(log)};
		EmitEvent(log_symbol, 1, argv);
	}

	void Yoctopuce::DeviceLogCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(devicelog_symbol, 1, argv);
	}

	void Yoctopuce::DeviceArrivalCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(devicearrival_symbol, 1, argv);
	}

	void Yoctopuce::DeviceRemovalCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(deviceremoval_symbol, 1, argv);
	}

	void Yoctopuce::DeviceChangeCallback(YAPI_DEVICE device)
	{
		HandleScope scope;
		Handle<Value> argv[1] = {Integer::New(device)};
		EmitEvent(devicechange_symbol, 1, argv);
	}
}