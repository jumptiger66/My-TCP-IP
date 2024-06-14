//
// Created by 孙昊 on 2024/6/4.
//

#ifndef NET_ERR_H
#define NET_ERR_H

typedef enum _net_err_t{
    NET_ERR_OK = 0,
    NET_ERR_SYS = -1, // 系统报错（如创建线程失败）
    NET_ERR_MEM = -2, // 内存相关报错
    NET_ERR_FULL = -3,
    NET_ERR_TMO = -4,
}net_err_t;


#endif //NET_ERR_H
