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

#include <v8.h>
#include <node.h>
#include <string>
#include <stdlib.h>

#include "yoctopuce.h"
#include "events.h"

namespace node_yoctopuce
{
	extern "C" {

		void fwdLogEvent(const char* log, u32 loglen)
		{
			if(Yoctopuce::eventHandler)
			{
				Event* ev = new LogEvent(log);
				Yoctopuce::eventHandler->send(ev);
			}
		}

		void fwdDeviceLogEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::eventHandler)
			{
				Event* ev = new DeviceLogEvent(device);
				Yoctopuce::eventHandler->send(ev);
			}
		}

		void fwdDeviceArrivalEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::eventHandler)
			{
				Event* ev = new DeviceArrivalEvent(device);
				Yoctopuce::eventHandler->send(ev);
			}
		}

		void fwdDeviceRemovalEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::eventHandler)
			{
				Event* ev = new DeviceRemovalEvent(device);
				Yoctopuce::eventHandler->send(ev);
			}
		}

		void fwdDeviceChangeEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::eventHandler)
			{
				Event* ev = new DeviceChangeEvent(device);
				Yoctopuce::eventHandler->send(ev);
			}
		}

		void fwdFunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *value)
		{
			if(Yoctopuce::eventHandler)
			{
				Event* ev = new FunctionUpdateEvent(fundescr, value);
				Yoctopuce::eventHandler->send(ev);
			}
		}

		static void uninit(void) 
		{
			Yoctopuce::Uninitialize();
			yapiFreeAPI();
		}

		static void init (Handle<Object> target)
		{
			atexit(uninit);

			char errmsg[YOCTO_ERRMSG_LEN];
			if(yapiInitAPI(Y_DETECT_USB,errmsg) != YAPI_SUCCESS
				|| yapiUpdateDeviceList(true, errmsg) != YAPI_SUCCESS)
			{
				fprintf(stderr, "Unable to initialize yapi.%s\n", errmsg);
				abort();
			}

			
			Yoctopuce::Initialize(target);

			yapiRegisterLogFunction(fwdLogEvent);
			yapiRegisterDeviceLogCallback(fwdDeviceLogEvent);
			yapiRegisterDeviceArrivalCallback(fwdDeviceArrivalEvent);
			yapiRegisterDeviceRemovalCallback(fwdDeviceRemovalEvent);
			yapiRegisterDeviceChangeCallback(fwdDeviceChangeEvent);
			yapiRegisterFunctionUpdateCallback(fwdFunctionUpdateEvent);

		}

		NODE_MODULE(node_yoctopuce, init);

	}
}