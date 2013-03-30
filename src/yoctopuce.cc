// -*- C++ -*-

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
#include <node.h>
#include <v8.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <yapi.h>

using namespace std;
using namespace v8;
using namespace node;

namespace yoctopuce {

	// //////////////////////////////////////////////////////////////////
	// Throwable error class that can be converted to a JavaScript
	// exception
	// //////////////////////////////////////////////////////////////////
	class JSException
	{

	public:
		JSException(const string& text) : _message(text) {};
		virtual const string message() const { return _message; }
		virtual Handle<Value> asV8Exception() const { return ThrowException(String::New(message().c_str())); }

	protected:
		string _message;

	};

	class Yoctopuce : public ObjectWrap
	{

	public:
		static void Initialize(Handle<Object> target);
		static Handle<Value> devices(const Arguments& args);
	private:
		static Handle<Value> New(const Arguments& args);

	};

	Handle<Value> Yoctopuce::New(const Arguments& args)
	{
		if (!args.IsConstructCall()) {
			return ThrowException(String::New("Yoctopuce function can only be used as a constructor"));
		}

		HandleScope scope;

		try {
			Yoctopuce* yoctopuce;
			yoctopuce = new Yoctopuce();
			yoctopuce->Wrap(args.This());
			return args.This();
		}    
		catch (const JSException& e) {
			return e.asV8Exception();
		}
	}

	Handle<Value> Yoctopuce::devices(const Arguments& args)
	{
		Local<Array> retval = Array::New();

		char errmsg[256];
		if(yapiUpdateDeviceList(false, errmsg) != YAPI_SUCCESS)
		{
			cerr << "Unable to update device list. yapiUpdateDeviceList failed: " << errmsg << endl;
			abort();
		}

		return retval;
	}

	static void deinitialize(void*)
	{
		yapiFreeAPI();
	}

	void Yoctopuce::Initialize(Handle<Object> target)
	{

		node::AtExit(deinitialize, 0);

		char errmsg[256];
		if(yapiInitAPI(Y_DETECT_USB,errmsg) != YAPI_SUCCESS)
		{
			cerr << "Unable to initialize yapi. yapiInitAPI failed: " << errmsg << endl;
			abort();
		}

		HandleScope scope;

		Local<FunctionTemplate> yoctopuceTemplate = FunctionTemplate::New(Yoctopuce::New);
		yoctopuceTemplate->InstanceTemplate()->SetInternalFieldCount(1);
		yoctopuceTemplate->SetClassName(String::New("Yoctopuce"));

		target->Set(String::NewSymbol("Yoctopuce"), yoctopuceTemplate->GetFunction());
		target->Set(String::NewSymbol("devices"), FunctionTemplate::New(Yoctopuce::devices)->GetFunction());
	}

	extern "C" {
		static void init (Handle<Object> target)
		{
			HandleScope handleScope;
			Yoctopuce::Initialize(target);
		}
		NODE_MODULE(yoctopuce, init);
	}

}