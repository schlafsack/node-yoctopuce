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
		static void Deinitialize(void*);
	private:

	};

	void Yoctopuce::Deinitialize(void*)
	{
		yapiFreeAPI();
	}

	void Yoctopuce::Initialize(Handle<Object> target)
	{
		node::AtExit(Yoctopuce::Deinitialize, 0);

		char errmsg[YOCTO_ERRMSG_LEN];
		if(yapiInitAPI(Y_DETECT_USB,errmsg) != YAPI_SUCCESS)
		{
			cerr << "Unable to initialize yapi. yapiInitAPI failed: " << errmsg << endl;
			abort();
		}

		HandleScope scope;
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