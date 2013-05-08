
// Adapted from node_sqlite3
// Copyright (c) 2011, Konstantin Käfer <kkaefer@gmail.com>
// All rights reserved.
// https://github.com/developmentseed/node-sqlite3/blob/master/LICENSE


#ifndef SRC_ASYNC_H_
#define SRC_ASYNC_H_

#include <node_version.h>
#include <vector>

using std::vector;
using std::size_t;

namespace node_yoctopuce {

    // Generic uv_async handler.
    template <class Item> class Async {
        typedef void (*Callback)(Item* item);

    protected:
        uv_mutex_t mutex;
        uv_async_t watcher;
        vector<Item*> data;
        Callback callback;

    public:
        explicit Async(Callback cb_) : callback(cb_) {
            watcher.data = this;
            uv_mutex_init(&mutex);
            uv_async_init(uv_default_loop(), &watcher, listener);
        }

        static void listener(uv_async_t* handle, int status) {
            Async* async = static_cast<Async*>(handle->data);
            vector<Item*> items;
            uv_mutex_lock(&async->mutex);
            items.swap(async->data);
            uv_mutex_unlock(&async->mutex);
            for (size_t i = 0, size = items.size(); i < size; i++) {
                uv_unref(reinterpret_cast<uv_handle_t*>(&async->watcher));
                async->callback(items[i]);
            }
        }

        static void close(uv_handle_t* handle) {
            assert(handle != NULL);
            assert(handle->data != NULL);
            Async* async = static_cast<Async*>(handle->data);
            delete async;
            handle->data = NULL;
        }

        void finish() {
            // Need to call the listener again to ensure all items have been
            // processed. Is this a bug in uv_async? Feels like uv_close
            // should handle that.
            listener(&watcher, 0);
            uv_close(reinterpret_cast<uv_handle_t*>(&watcher), close);
        }

        void add(Item* item) {
            // Make sure node runs long enough to deliver the messages.
            uv_ref(reinterpret_cast<uv_handle_t*>(&watcher));
            uv_mutex_lock(&mutex);
            data.push_back(item);
            uv_mutex_unlock(&mutex);
        }

        void send() {
            uv_async_send(&watcher);
        }

        void send(Item* item) {
            add(item);
            send();
        }

        ~Async() {
            uv_mutex_destroy(&mutex);
        }
    };

}  // namespace node_yoctopuce

#endif  // SRC_ASYNC_H_
