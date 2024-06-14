/**
 * @file main.c
 * @author lishutong (527676163@qq.com)
 * @brief 测试主程序，完成一些简单的测试主程序
 * @version 0.1
 * @date 2022-10-23
 *
 * @copyright Copyright (c) 2022
 * @note 该源码配套相应的视频课程，请见源码仓库下面的README.md
 */
#include <stdio.h>
#include "sys_plat.h"
#include "net.h"
#include "netif_pcap.h"
#include "dbg.h"
#include "nlist.h"
#include "mblock.h"
#include "nlocker.h"

net_err_t netdev_init(void ){
    netif_pcap_open();

    return NET_ERR_OK;
}

typedef struct _tnode_t {
    int id;
    nlist_node_t node;
}tnode_t;

void nlist_test (void){
    #define NODE_CNT 4

    tnode_t node[NODE_CNT];
    nlist_t list;

    nlist_init(&list);

    plat_printf("insert first\n");
    for (int i = 0; i < NODE_CNT; i++){
        node[i].id = i;
        nlist_insert_first(&list, &node[i].node);
    }

    nlist_node_t * p;
    nlist_for_each(p, &list){
        tnode_t * tnode = nlist_entry(p, tnode_t, node);
        plat_printf("id: %d\n", tnode->id);
    }

    plat_printf("remove head node ...\n");
    for (int i = 0; i < NODE_CNT; ++i) {
        p = nlist_remove_first(&list);
        plat_printf("id: %d\n", nlist_entry(p, tnode_t, node)->id);
    }

    // 尾插
    nlist_init(&list);
    plat_printf("insert last\n");
    for (int i = 0; i < NODE_CNT; i++){
        nlist_insert_last(&list, &node[i].node);
    }

    nlist_node_t * p2;

    nlist_for_each(p2, &list){
        tnode_t * tnode = nlist_entry(p2, tnode_t, node);
        plat_printf("id: %d\n", tnode->id);
    }

    // 尾删
    plat_printf("remove tail node ...\n");
    for (int i = 0; i < NODE_CNT; ++i) {
        p = nlist_remove_last(&list);
        plat_printf("id: %d\n", nlist_entry(p, tnode_t, node)->id);
    }

    // 结点后插
    nlist_init(&list);
    nlist_node_t * p3;

    plat_printf("insert after\n");
    for (int i = 0; i < NODE_CNT; i++){
        nlist_insert_after(&list, nlist_first(&list), &node[i].node);
    }

    nlist_for_each(p3, &list){
        tnode_t * tnode = nlist_entry(p3, tnode_t, node);
        plat_printf("id: %d\n", tnode->id);
    }

}

void mblock_test (void) {
    mblock_t blist;
    static uint8_t buffer[100][10];
    mblock_init(&blist, buffer, 100, 10, NLOCKER_THREAD);

    void * temp[10];
    for (int i = 0; i < 10; ++i) {
        temp[i] = mblock_alloc(&blist, 0);
        plat_printf("block: %p, free_count: %d\n",temp[i], mblock_free_cnt(&blist));

    }

    for (int i = 0; i < 10; ++i) {
        mblock_free(&blist, temp[i]);
        plat_printf("free_count: %d\n", mblock_free_cnt(&blist));
    }
}

void basic_test(void){
    nlist_test();
    mblock_test();
}

#define DBG_TEST  DBG_LEVEL_INFO

int main (void) {
    dbg_info(DBG_TEST, "dbg_info %d %d", 1, 200);
    dbg_warning(DBG_TEST, "dbg_warning");
    dbg_error(DBG_TEST, "dbg_error");
//    dbg_assert(1==0, "failed");

    // 网络初始化
    net_init();

    basic_test();


    // 网络核心启动
    net_start();

    netdev_init(); // 调用pcap创建读写线程

    while (1){
        sys_sleep(10);
    }

	// 以下是测试代码，可以删掉
	// 打开物理网卡，设置好硬件地址
//	static const uint8_t netdev0_hwaddr[] = { 0x00, 0x50, 0x56, 0xc0, 0x00, 0x11 };
//	pcap_t* pcap = pcap_device_open("192.168.74.1", netdev0_hwaddr);
//	sys_sleep(1000);

	printf("Hello, world");
	return 0;
}