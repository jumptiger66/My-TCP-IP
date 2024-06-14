//
// Created by 孙昊 on 2024/6/11.
//

#ifndef NET_MBLOCK_H
#define NET_MBLOCK_H

#include "nlist.h"
#include "nlocker.h"

/* 该结构构成了完整的数据包
 * --------
 *  first
 * --------
 *  last
 * --------
 *  count
 * --------
 *  start
 * --------
 *  locker
 * --------
 * alloc_sem
 * --------
 *
 * 操作思路
 *      初始化：自定义一个数组（确定所需的容量大小，也是本协议站可用的内存大小），然后生成链式的存储结构（主要为了获取更多信息，如多少空闲块/空闲资源，同时便于分配）
 *      分配 alloc：从 数据包 mblock 中取出一个数据块，链式结构中remove一个
 *      释放 free： 将 数据块 插回 数据包 的链表尾部
 * */
typedef struct _mblock_t {
    nlist_t free_list; // 封装前的数据包（结点是数据块）
    void * start; // 指向内存块的起始地址
    nlocker_t locker; // ...
    sys_sem_t alloc_sem; // 通过 锁 和 信号量 来实现多个线程对链式存储结构的申请和释放
}mblock_t;

net_err_t mblock_init (mblock_t * mblock, void * mem, int blk_size, int cnt, nlocker_type_t type);

void * mblock_alloc (mblock_t * mblock, int ms);
int mblock_free_cnt (mblock_t * block);

void mblock_free(mblock_t * mblock, void * block);
void mblock_destroy (mblock_t * mblock);


#endif //NET_MBLOCK_H
