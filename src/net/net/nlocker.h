//
// Created by 孙昊 on 2024/6/11.
//

#ifndef NET_NLOCKER_H
#define NET_NLOCKER_H

#include "sys.h"
#include "net_err.h"

// 锁的类型(当前锁用于何种场合)
typedef enum _nlocker_type_t{
    NLOCKER_NONE, // 不需要使用锁
    NLOCKER_THREAD, // 线程间互斥锁
}nlocker_type_t;

typedef struct _nlocker_t {
    nlocker_type_t type;
    union {
        sys_mutex_t mutex;
        // ...
    };
}nlocker_t;

net_err_t nlocker_init (nlocker_t * locker, nlocker_type_t type);
void nlocker_destroy (nlocker_t * locker);
void nlocker_lock (nlocker_t * locker);
void nlocker_unlock (nlocker_t * locker);

#endif //NET_NLOCKER_H
