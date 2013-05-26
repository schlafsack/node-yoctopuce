
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

#include "./yoctopuce.h"

using v8::HandleScope;
using v8::Persistent;
using v8::Exception;
using v8::String;
using v8::Function;
using v8::Local;
using v8::Handle;
using v8::Arguments;
using v8::Value;
using v8::TryCatch;
using v8::Object;
using v8::Undefined;
using v8::ThrowException;

using node::FatalException;

using std::string;

namespace node_yoctopuce {

    Handle<Value> Yoctopuce::HttpRequestAsync(const Arguments& args) {
        HandleScope scope;

        if (args.Length() < 1 || !args[0]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 1 must be a string"))));
        }
        String::Utf8Value device_arg(args[0]->ToString());

        if (args.Length() < 2 || !args[1]->IsString()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 2 must be a string"))));
        }
        String::Utf8Value path_arg(args[1]->ToString());

        if (args.Length() < 3 || !args[2]->IsFunction()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 3 must be a function"))));
        }
        Local<Value> callback_arg = args[2];

        if (args.Length() < 4 || !args[3]->IsObject()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 3 must be an object"))));
        }
        Local<Value> request_arg = args[3];

        uv_work_t* work = new uv_work_t();
        HttpRequestBaton *baton = new HttpRequestBaton();
        baton->work = work;
        baton->device = string(*device_arg);
        baton->path = string(*path_arg);
        baton->callback = Persistent<Function>::New(Handle<Function>::Cast(callback_arg));
        baton->request = Persistent<Object>::New(Handle<Object>::Cast(request_arg));
        work->data = baton;

        uv_queue_work(uv_default_loop(), work, OnHttpRequest, (uv_after_work_cb)OnAfterHttpRequest);
        return scope.Close(Undefined());
    }

    void Yoctopuce::OnHttpRequest(uv_work_t* req) {
        HttpRequestBaton* baton = static_cast<HttpRequestBaton*>(req->data);

        char errmsg[YOCTO_ERRMSG_LEN];
        char *response;
        int response_size;
        YIOHDL request_handle;

        uv_mutex_lock(&http_request_mutex);
        baton->result = yapiHTTPRequestSyncStartEx(&request_handle, baton->device.c_str(),
            baton->path.c_str(), baton->path.length(), &response, &response_size, errmsg);
        if(YISERR(baton->result)) {
            baton->error = string(errmsg);
        } else {
            baton->response = string(response, response_size);
            yapiHTTPRequestSyncDone(&request_handle, errmsg);
        }
        uv_mutex_unlock(&http_request_mutex);
    }

    void Yoctopuce::OnAfterHttpRequest(uv_work_t* req) {
        HandleScope scope;
        HttpRequestBaton* baton = static_cast<HttpRequestBaton*>(req->data);

        if (YISERR(baton->result)) {
            EmitError(baton->request, baton->error);
        } else {
            Handle<Value> argv[1];
            argv[0] = String::New(baton->response.c_str());
            TryCatch try_catch;
            Function::Cast(*baton->callback)->Call(target_handle , 1, argv);
            if (try_catch.HasCaught()) {
                FatalException(try_catch);
            }
        }

        baton->callback.Dispose();
        baton->request.Dispose();
        delete baton;
    }

    void Yoctopuce::EmitError(Handle<Object> context, string error) {
        HandleScope scope;
        Local<Value> ev = context->Get(String::NewSymbol("emit"));
        // If the emit function has been bound call it; otherwise
        // drop the events.
        if (!ev.IsEmpty() && ev->IsFunction()) {
            Local<Function> emitter = Local<Function>::Cast(ev);
            Handle<Value> argv[2] = {String::NewSymbol("error"), String::New(error.c_str())};
            TryCatch try_catch;
            emitter->Call(context, 2, argv);
            if (try_catch.HasCaught()) {
                FatalException(try_catch);
            }
        }
    }

}  // namespace node_yoctopuce
