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
#include <stdio.h>
#include <stdlib.h>

#include "yoctopuce.h"

namespace node_yoctopuce
{
	extern "C" {

		void fwdLogEvent(const char* log, u32 loglen)
		{
			if(Yoctopuce::log_event)
				Yoctopuce::log_event->send(std::string(log, loglen)); 
		}

		void fwdDeviceLogEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::device_log_event)
				Yoctopuce::device_log_event->send(device); 
		}

		void fwdDeviceArrivalEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::device_arrival_event)
				Yoctopuce::device_arrival_event->send(device);
		}

		void fwdDeviceRemovalEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::device_removal_event)
				Yoctopuce::device_removal_event->send(device);
		}

		void fwdDeviceChangeEvent(YAPI_DEVICE device)
		{
			if(Yoctopuce::device_change_event)
				Yoctopuce::device_change_event->send(device);
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

			yapiRegisterLogFunction(fwdLogEvent);
			yapiRegisterDeviceLogCallback(fwdDeviceLogEvent);
			yapiRegisterDeviceArrivalCallback(fwdDeviceArrivalEvent);
			yapiRegisterDeviceRemovalCallback(fwdDeviceRemovalEvent);
			yapiRegisterDeviceChangeCallback(fwdDeviceChangeEvent);

			Yoctopuce::Initialize(target);

		}

		NODE_MODULE(node_yoctopuce, init);

	}
}