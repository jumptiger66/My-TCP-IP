//
 // Created by 孙昊 on 2024/6/4.
//
#include "exmsg.h"
#include "sys_plat.h"
#include "fixq.h"
#include "dbg.h"
#include "mblock.h"
#include "net_cfg.h"

static void * msg_tbl[EXMSG_MSG_CNT]; // 消息缓冲区
static fixq_t msg_queue; // 消息队列

static exmsg_t msg_buffer[EXMSG_MSG_CNT]; // 消息块
static mblock_t msg_block; // 消息分配器（实际就是链表+锁+信号量）

// 核心线程通信初始化
net_err_t exmsg_init(void){
    dbg_info(DBG_MSG, "exmsg init");

    // 初始化消息队列
    net_err_t err = fixq_init(&msg_queue, msg_tbl, EXMSG_MSG_CNT, EXMSG_LOCKER);
    if (err < 0){
        dbg_error(DBG_MSG, "fixq init failed");
        return err;
    }

    // 初始化消息分配器
    err = mblock_init(&msg_block, msg_buffer, sizeof(exmsg_t ), EXMSG_MSG_CNT, EXMSG_LOCKER);
    if (err < 0){
        dbg_error(DBG_MSG, "mblock init failed");
        return err;
    }

    // 初始化完成
    dbg_info(DBG_MSG, "exmsg init done");
    return NET_ERR_OK;
}

// 模拟：收到来自网卡的消息
net_err_t exmsg_netif_in(void){
    exmsg_t * msg = mblock_alloc(&msg_block, -1); // 网卡包不等待，丢包由协议处理
    if (!msg){
        dbg_warning(DBG_MSG, "no free msg");
        return NET_ERR_MEM;
    }
    static int id = 0;

    // test：对空的msg块填入信息发到消息队列里，供工作线程处理
    msg->type = NET_EXMSG_NETIF_IN;
    msg->id = id++;

    net_err_t err = fixq_send(&msg_queue, msg, -1);
    if (err < 0){
        dbg_warning(DBG_MSG, "fixq full");
        mblock_free(&msg_block, msg);
        return err;
    }
    return NET_ERR_OK;
};

// 核心线程功能体
static void work_thread(void * arg){
    dbg_info(DBG_MSG, "exmsg is running...\n");

    while(1){
        sys_sleep(1000);
        // 接受消息，该工作现场不断读消息队列中的消息，读到了就打印
        exmsg_t * msg = (exmsg_t *) fixq_recv(&msg_queue, 0);

        plat_printf("recv a msg type: %d, id : %d\n",msg->type, msg->id);

        // 释放消息
        mblock_free(&msg_block, msg);
    }
}

// 启动核心线程通信机制
net_err_t exmsg_start(void){
    sys_thread_t thread = sys_thread_create(work_thread, (void *) 0);
    if (thread == SYS_THREAD_INVALID){
        return NET_ERR_SYS;
    }
    return NET_ERR_OK;
}
