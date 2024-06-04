//
// Created by 孙昊 on 2024/6/4.
//

#include "net.h"
#include "net_err.h"
#include "exmsg.h"
#include "net_plat.h"

net_err_t net_init(void){
    net_plat_init(); // 针对特定平台进行的初始化（如RTOS、开发板等）

    exmsg_init();
    return NET_ERR_OK;
}

net_err_t net_start(void){
    exmsg_start();
    return NET_ERR_OK;
}