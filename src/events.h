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

#ifndef NODE_YOCTOPUCE_EVENTS_H
#define NODE_YOCTOPUCE_EVENTS_H


#include <algorithm>
#include <string>
#include <v8.h>
#include <node.h>
#include <yapi.h>

using namespace v8;
using namespace node;
using std::string;

#define EMIT_EVENT(obj, argc, argv)	TRY_CATCH_CALL((obj), Local<Function>::Cast((obj)->Get(String::NewSymbol("emit"))),	argc, argv);
#define TRY_CATCH_CALL(context, callback, argc, argv) {	TryCatch try_catch; (callback)->Call((context), (argc), (argv)); if (try_catch.HasCaught()) { FatalException(try_catch); } }

namespace node_yoctopuce 
{

	struct Event
	{
		inline virtual void send(Handle<Object> context)=0;
	};

	struct CharDataEvent : Event 
	{
		inline CharDataEvent(const char *_data) {
			data = _data ? string(_data) : string();
		}
		
		string data;

	};

	struct DeviceEvent : Event
	{
		inline DeviceEvent(const char* name, YAPI_DEVICE device) : device(device), name(name) {}
		
		const char* name;
		YAPI_DEVICE device;

		inline virtual void send(Handle<Object> context) {
			int argc = 2;
			Handle<Value> argv[2] = {String::New(name), Integer::New(device)};
			EMIT_EVENT(context, argc, argv); 
		}
	};

	struct LogEvent : CharDataEvent
	{
		inline LogEvent(const char *data) : CharDataEvent(data) {}

		inline virtual void send(Handle<Object> context) {
			int argc = 2;
			string log = string(data);
			log.erase(std::remove(log.begin(), log.end(), '\n'), log.end());
			log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());
			Handle<Value> argv[2] = {String::New("log"), String::New(log.c_str())};
			EMIT_EVENT(context, argc, argv); 
		}

	};

	struct DeviceLogEvent : DeviceEvent {
		inline DeviceLogEvent(YAPI_DEVICE device) : DeviceEvent("deviceLog", device) {}
	};

	struct DeviceArrivalEvent : DeviceEvent {
		inline DeviceArrivalEvent(YAPI_DEVICE device) : DeviceEvent("deviceArrival", device) {}
	};

	struct DeviceChangeEvent : DeviceEvent {
		inline DeviceChangeEvent(YAPI_DEVICE device) : DeviceEvent("deviceChange", device) {}
	};

	struct DeviceRemovalEvent : DeviceEvent {
		inline DeviceRemovalEvent(YAPI_DEVICE device) : DeviceEvent("deviceRemoval", device) {}
	};


	struct FunctionUpdateEvent : CharDataEvent
	{
		inline FunctionUpdateEvent(YAPI_FUNCTION fundescr, const char *data) : CharDataEvent(data), fundescr(fundescr) {};

		YAPI_FUNCTION fundescr;

		inline virtual void send(Handle<Object> context) {
			int argc = 3;
			Handle<Value> argv[3] = {String::New("functionUpdate"), Integer::New(fundescr), String::New(data.c_str())};
			EMIT_EVENT(context, argc, argv); 
		}
	};

}

#endif