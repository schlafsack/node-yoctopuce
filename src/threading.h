
// Adapted from node_sqlite3
// Copyright (c) 2011, Konstantin Käfer <kkaefer@gmail.com>
// All rights reserved.
// https://github.com/developmentseed/node-sqlite3/blob/master/LICENSE

#ifndef SRC_THREADING_H_
#define SRC_THREADING_H_


#ifdef _WIN32

#include <windows.h>

#define NODE_YOCTOPUCE_MUTEX_t HANDLE mutex;

#define NODE_YOCTOPUCE_MUTEX_INIT CreateMutex(NULL, FALSE, NULL);

#define NODE_YOCTOPUCE_MUTEX_LOCK(m) WaitForSingleObject(m, INFINITE);

#define NODE_YOCTOPUCE_MUTEX_UNLOCK(m) ReleaseMutex(m);

#define NODE_YOCTOPUCE_MUTEX_DESTROY CloseHandle(mutex);

#else

#define NODE_YOCTOPUCE_MUTEX_t pthread_mutex_t mutex;

#define NODE_YOCTOPUCE_MUTEX_INIT pthread_mutex_init(&mutex, NULL);

#define NODE_YOCTOPUCE_MUTEX_LOCK(m) pthread_mutex_lock(m);

#define NODE_YOCTOPUCE_MUTEX_UNLOCK(m) pthread_mutex_unlock(m);

#define NODE_YOCTOPUCE_MUTEX_DESTROY pthread_mutex_destroy(&mutex);

#endif


#endif  // SRC_THREADING_H_
