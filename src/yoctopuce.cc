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
#include "events.h"

using namespace std;
using namespace v8;
using namespace node;

namespace node_yoctopuce 
{

	Persistent<Object> Yoctopuce::targetHandle;

	Yoctopuce::AsyncEventHandler* Yoctopuce::eventHandler;

	void Yoctopuce::Initialize(Handle<Object> target)
	{

		HandleScope scope;

		targetHandle = Persistent<Object>::New(target);

		NODE_SET_METHOD(targetHandle, "updateDeviceList", UpdateDeviceList);
		NODE_SET_METHOD(targetHandle, "handleEvents", HandleEvents);
		NODE_SET_METHOD(targetHandle, "getDeviceInfo", GetDeviceInfo);

		eventHandler = new AsyncEventHandler(EventCallback);

	}

	void Yoctopuce::Uninitialize()
	{
		if(eventHandler)
		{
			eventHandler -> finish();
			eventHandler = NULL;
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

	void Yoctopuce::EventCallback(Event *event) 
	{
		HandleScope scope;
		if(!targetHandle.IsEmpty())
		{
			event->send(targetHandle);
		}
	}
	
}