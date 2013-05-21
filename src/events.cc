
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

#include "./events.h"

using v8::Function;
using v8::Value;
using v8::Local;
using v8::HandleScope;
using v8::String;
using v8::TryCatch;
using v8::Integer;
using v8::HandleScope;

using std::string;

using node::FatalException;

namespace node_yoctopuce {

    void Event::dispatchToV8(Handle<Object> context, int argc, Handle<Value> argv[]) {
        HandleScope scope;
        if (!context.IsEmpty()) {
            Local<Value> dispatchValue = context->Get(String::NewSymbol("emit"));

            // If the emit function has been bound call it; otherwise
            // drop the events.
            if (!dispatchValue.IsEmpty() && dispatchValue->IsFunction()) {
                Local<Function> dispatchFunction = Local<Function>::Cast(dispatchValue);
                TryCatch try_catch;
                dispatchFunction->Call(context, argc, argv);
                if (try_catch.HasCaught()) {
                    FatalException(try_catch);
                }
            }
        }
    }

    void LogEvent::dispatch(Handle<Object> context) {
        int argc = 2;
        string log = string(data);
        log.erase(std::remove(log.begin(), log.end(), '\n'), log.end());
        log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());
        Handle<Value> argv[2] = {String::NewSymbol("log"), String::New(log.c_str())};
        dispatchToV8(context, argc, argv);
    }

    void FunctionUpdateEvent::dispatch(Handle<Object> context) {
        int argc = 3;
        Handle<Value> argv[3] =
        {String::NewSymbol("functionUpdate"), Integer::New(fundescr), String::New(data.c_str())};
        dispatchToV8(context, argc, argv);
    }

    void DeviceEvent::dispatch(Handle<Object> context) {
        int argc = 2;
        Handle<Value> argv[2] = {String::NewSymbol(name), Integer::New(device)};
        dispatchToV8(context, argc, argv);
    }

}  // namespace node_yoctopuce
