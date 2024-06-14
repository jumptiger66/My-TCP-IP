//
// Created by 孙昊 on 2024/6/13.
//

#ifndef NET_FIXQ_H
#define NET_FIXQ_H

#include "nlocker.h"
#include "sys.h"

typedef struct _fixq_t{
    int size;               // 消息队列容量
    int in, out, cnt;       // 读in 写out 位置索引，消息数 cnt

    void **buf;             // 消息缓存,一般buf是数组，传过来的是数组指针 *buf，所以用 **buf来跟踪这个指针，以实际只操纵buf数组

    nlocker_t locker;       // 消息队列互斥锁
    sys_sem_t recv_sem;     // 接收消息时使用的信号量
    sys_sem_t send_sem;     // 发送消息时使有的信号量
}fixq_t;

net_err_t fixq_init(fixq_t * q, void **buf, int size, nlocker_type_t type);
net_err_t fixq_send(fixq_t * q, void * msg, int tmo);
void * fixq_recv (fixq_t * q, int tmo);

void fixq_destroy (fixq_t * q);
int fixq_count (fixq_t * q);

#endif //NET_FIXQ_H