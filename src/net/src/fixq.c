//
// Created by 孙昊 on 2024/6/13.
//

#include "fixq.h"
#include "dbg.h"
#include "net_cfg.h"

// fixq_init(&msg_queue, msg_tbl, EXMSG_MSG_CNT, EXMSG_LOCKER);
net_err_t fixq_init(fixq_t * q, void **buf, int size, nlocker_type_t type){
    // 未判断前的初始化，基本都是空值
    q->size = size;
    q->in = q->out = q->cnt = 0;
    q->buf = (void **)0;
    q->send_sem = SYS_SEM_INVALID;
    q->recv_sem = SYS_SEM_INVALID;

    // 消息队列中的锁的初始化
    net_err_t err = nlocker_init(&q->locker, type);
    if (err < 0){
        dbg_error(DBG_QUEUE, "init locker failed");
        return err;
    }

    // 消息队列中写信号量的初始化
    q->send_sem = sys_sem_create(size); // 写信号量初始值和队列大小一样，因为初始队列中可用空间是满
    if (q->send_sem == SYS_SEM_INVALID){
        dbg_error(DBG_QUEUE, "create send sem failed");
        err = NET_ERR_SYS;
        goto  init_failed;
    }

    // 消息队列中读信号量的初始化
    q->recv_sem = sys_sem_create(0); // 读信号量初始值为0，因为初始队列中可读空间是空
    if (q->recv_sem == SYS_SEM_INVALID){
        dbg_error(DBG_QUEUE, "create recv sem failed");
        err = NET_ERR_SYS;
        goto  init_failed;
    }

    // 数据缓冲区->队列
    q->buf = buf;
    return NET_ERR_OK;

init_failed:
    if (q->send_sem != SYS_SEM_INVALID){
        sys_sem_free(q->send_sem);
    }

    if (q->recv_sem != SYS_SEM_INVALID){
        sys_sem_free(q->recv_sem);
    }

    nlocker_destroy(&q->locker);
    return err;
};

/*
 * 向消息队列写入一个消息
 * 如果消息队列满，则看下tmo，如果tmo < 0则不等待
 * */
net_err_t fixq_send(fixq_t * q, void * msg, int tmo){
    /*
     * param: q, 要写入的目标消息队列
     * param: msg, 需写入的消息
     * param: tmo, 写入的等待时间
     * */
    nlocker_lock(&q->locker);
    if ((tmo < 0) && (q->cnt >= q->size)){              // 如果缓存已满，并且不需要等待，则立即退出
        nlocker_unlock(&q->locker);
        return NET_ERR_FULL;
    }
    nlocker_unlock(&q->locker); // 防死锁

    if (sys_sem_wait(q->send_sem, tmo) < 0){   // wait 函数 等待超时返回-1
        return NET_ERR_TMO;
    }

    nlocker_lock(&q->locker); // 防死锁
    q->buf[q->in++] = msg; // 核心步骤：消息写入q->buf, 写指针环形后移一位
    if (q->in >= q->size){
        q->in = 0;
    }
    q->cnt++;
    nlocker_unlock(&q->locker);

    sys_sem_notify(q->recv_sem); // 线程间通信：通知接受信号量，消息队列中已有消息
    return NET_ERR_OK;
}

// 从数据包队列中取一个消息，如果无，则等待
// 调用一次读一个消息
void * fixq_recv (fixq_t * q, int tmo){
    /*
     * param: q, 被读取的消息队列
     * param: tmo, 等待时长
     * */
    nlocker_lock(&q->locker);
    if (!q->cnt && (tmo < 0)){ // 若无数据且不允许等待，则直接返回 空
        nlocker_unlock(&q->locker);
        return (void *)0;
    }
    nlocker_unlock(&q->locker);

    if (sys_sem_wait(q->recv_sem, tmo) < 0){ // 等待超时返回空
        return (void *) 0;
    }

    nlocker_lock(&q->locker);
    void * msg = q->buf[q->out++]; // 核心操作：从消息队列中读取一个消息，读指针环形后移
    if (q->out >= q->size){
        q->out = 0;
    }
    q->cnt--;
    nlocker_unlock(&q->locker);

    sys_sem_notify(q->send_sem); // 通知写信号量，消息队列有内存，可以继续写
    return msg;
};

// 销毁队列
void fixq_destroy (fixq_t * q){
    nlocker_destroy(&q->locker);
    sys_sem_free(q->send_sem);
    sys_sem_free(q->recv_sem);
};

// 返回缓冲中消息的数量
int fixq_count (fixq_t * q){
    nlocker_lock(&q->locker);
    int count = q->cnt;
    nlocker_unlock(&q->locker);
    return count;
};
