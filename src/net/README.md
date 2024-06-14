# 协议栈目录

net 目录逐个实现各个协议，其中
- net 目录：存放协议头文件（.h）
- src 目录：存放协议源码 （.c）

各文件的作用（逐步丰富，后续用多级列表）
- dbg.h: 调试代码（打印 info、warning、 error信息）、assert 断言
- net_err.h: 存放错误码
- net.h: 网络的初始化和启动
- exmsg.h: 工作线程初始化和启动
- xxx'