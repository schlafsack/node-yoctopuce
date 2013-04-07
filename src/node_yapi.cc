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
/*
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <stdlib.h>

#include <v8.h>
#include <node.h>

*/
#include <iostream>
#include <sstream>
#include <node.h>
#include <v8.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <yapi.h>

using namespace std;
using namespace v8;
using namespace node;

namespace node_yoctopuce {

	// //////////////////////////////////////////////////////////////////
	// Throwable error class that can be converted to a JavaScript
	// exception
	// //////////////////////////////////////////////////////////////////
	class JSException
	{
	public:
		JSException(const string& text) : _message(text) {};
		virtual const string message() const { return _message; }
		virtual Handle<Value> AsV8Exception() const { return ThrowException(String::New(message().c_str())); }
	protected:
		string _message;
	};

	class NodeYapi : public ObjectWrap
	{
	public:

		static void Initialize(Handle<Object> target);
		static void Deinitialize(void*);
		static void SetCallbacks();

		static Handle<Value> UpdateDeviceList(const Arguments& args);
		static Handle<Value> RegisterLogFunction(const Arguments& args);

	private:
		static void LogCallback(const char *log, u32 loglen);
	};

	void NodeYapi::SetCallbacks()
	{
		yapiRegisterLogFunction(LogCallback);
	}

	void NodeYapi::LogCallback(const char *log, u32 loglen)
	{
		cout << log << endl;
	}

	void NodeYapi::Initialize(Handle<Object> target)
	{
		node::AtExit(NodeYapi::Deinitialize, 0);

		char errmsg[YOCTO_ERRMSG_LEN];
		if(yapiInitAPI(Y_DETECT_USB,errmsg) != YAPI_SUCCESS
			|| yapiUpdateDeviceList(true, errmsg) != YAPI_SUCCESS)
		{
			cerr << "Unable to initialize yapi. " << errmsg << endl;
			abort();
		}

		HandleScope scope;

		NODE_SET_METHOD(target,"updateDeviceList", NodeYapi::UpdateDeviceList);
		NODE_SET_METHOD(target,"registerLogFunction", NodeYapi::RegisterLogFunction);
	}

	void NodeYapi::Deinitialize(void*)
	{
		yapiFreeAPI();
	}

	Handle<Value> NodeYapi::UpdateDeviceList(const Arguments& args)
	{
		HandleScope scope;

		try{
			char errmsg[YOCTO_ERRMSG_LEN];
			if(yapiUpdateDeviceList(false, errmsg) != YAPI_SUCCESS)
			{
				ostringstream os;
				os << "Unable to update the device list. yapiUpdateDeviceList failed: " << errmsg;
				throw JSException(os.str());
			}

			return scope.Close(Undefined());
		}
		catch (const JSException& e) {
			return scope.Close(e.AsV8Exception());
		}
	}

	Handle<Value> NodeYapi::RegisterLogFunction(const Arguments& args)
	{
		HandleScope scope;

		try{

			if (args.Length() != 1
				|| !args[0]->IsFunction()) {
					return ThrowException(String::New("need one callback function argument in read"));
			}


			return scope.Close(Undefined());
		}
		catch (const JSException& e) {
			return scope.Close(e.AsV8Exception());
		}
	}

	extern "C" {
		static void init (Handle<Object> target)
		{
			HandleScope handleScope;
			NodeYapi::Initialize(target);
			NodeYapi::SetCallbacks();
		}
		NODE_MODULE(node_yapi, init);
	}

}