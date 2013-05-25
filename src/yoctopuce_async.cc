
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
using v8::Undefined;
using v8::ThrowException;

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
        String::Utf8Value request_arg(args[1]->ToString());

        if (args.Length() < 3 || !args[2]->IsFunction()) {
            return scope.Close(ThrowException(Exception::TypeError(String::New("Argument 3 must be a function"))));
        }
        Local<Value> callback_arg = args[2];

        uv_work_t* req = new uv_work_t();
        HttpRequestBaton *baton = new HttpRequestBaton();
        baton->req=req;
        baton->c_device = *device_arg;
        baton->c_request = *request_arg;
        baton->callback = v8::Persistent<v8::Value>::New(callback_arg); 
        req->data = baton;

        uv_queue_work(uv_default_loop(), req, onHttpRequest, (uv_after_work_cb)onAfterHttpRequest);

        return scope.Close(Undefined());
    }

    void Yoctopuce::onHttpRequest(uv_work_t* req) {
        HttpRequestBaton* baton = static_cast<HttpRequestBaton*>(req->data);
        uv_mutex_lock(&g_http_request_mutex);
        baton->result = yapiHTTPRequestSyncStartEx(&baton->request_handle, baton->c_device,
            baton->c_request, baton->request_size, &baton->c_reply, &baton->reply_size, baton->errmsg);
        fprintf(stderr, "onHttpRequest %s %s\n", baton->c_device, baton->c_request);
    }

    void Yoctopuce::onAfterHttpRequest(uv_work_t* req) {
        HttpRequestBaton* baton = static_cast<HttpRequestBaton*>(req->data);

        if (YISERR(yapiHTTPRequestSyncDone(&baton->request_handle, baton->errmsg))) {
            // what to do?
        }
        uv_mutex_unlock(&g_http_request_mutex);

        fprintf(stderr, "onAfterHttpRequest %s %s\n", baton->c_reply, baton->errmsg);

        Local<String> reply = String::New(baton->c_reply, baton->reply_size);
        v8::Handle<v8::Value> argv[1] = { reply };
        Function::Cast(*baton->callback)->Call(g_target_handle , 1, argv);

        baton->callback.Dispose();
        delete baton;
    }

}  // namespace node_yoctopuce
