# 知识点
这个文件下面记录了学习过程中的知识点

## 1. include guard
`#ifndef NET_H` 通过宏定义量用于防止头文件被重复包含。具体说明如下：

`#ifndef NET_H` 这是一个条件编译指令，它会检查 NET_NET_H 是否未被定义。如果 NET_NET_H 还没有被定义，它将执行紧随其后的指令。

注：若一个头文件被个文件包含，那么这个`NET_H`肯定被定义过了，那么在最终链接完的代码中，保证其只会被包含一次。

## 2. .c 和 .h 文件关系

头文件是对其他文件开放函数接口的（别的文件只需要 include 该头文件，即可使用其中函数），具体函数需要在 .c 文件中实现。

## 3. debug display 部分知识点（可变参数 ... 、C 默认宏定义）

课程 5.1中实现了 debug 的操作（dbg.c），其中在 `dbg_print` 函数传入了默认的C语言宏定义和可变参数。

- C 语言宏定义
    - `__FILE__`: 返回当前文件的绝对路径（const char * 类型，字符串）
    - `__FUNCTION__`: 当前所属的函数名
    - `__LINE__`: 调用时的代码行数

- 可变参数 `...`
  - `##__VA_ARGS__`是一个可变参数宏，它允许宏定义`dbg_info`时接收任意数量的参数，并把这些参数传递给`dbg_print`函数
  - 打印可变参数操作如下
```c
#include <stdarg.h>

void dbg_print(int m_level, int s_level, const char * file, const char * func, int line, const char * fmt, ...){
    static const char * title[] = {
        [DBG_LEVEL_NONE] = "none",
        [DBG_LEVEL_ERROR] = DBG_STYLE_ERROR"error",
        [DBG_LEVEL_WARNING] = DBG_STYLE_WARNING"warning",
        [DBG_LEVEL_INFO] = "info",
    };

    if (m_level >= s_level){
      const char * end = file + plat_strlen(file);
      while (end >= file){
          if ((*end == '\\') || (*end == '/')){
              break;
          }
          end--;
      }
      end++;
    
      plat_printf("%s(%s-%s-%d): ", title[s_level], end, func, line);
      
      // 可变参数打印
      // 生成格式化的字符串并存储在str_buf缓冲区中，并打印
      
      char str_buf[128]; // 字符缓冲区, 用于存储最后的格式化字符串。长度为128字节。
      va_list args; // 定义一个可变参数列表
      va_start(args, fmt); // 初始化可变参数列表，fmt为可变参数前的最后一个固定参数
      plat_vsnprintf(str_buf, sizeof(str_buf), fmt, args);
      /* 
       * str_buf: 字符缓冲区
       * sizeof(str_buf): 限制了写入目标缓冲区的最大字符数，防止缓冲区溢出
       * fmt: 带格式的字符串
       * args: 传入前面字符串的可变参数
       * */
    
      plat_printf("%s\n"DBG_STYLE_RESET, str_buf);
      va_end(args);
    }
} 


```

## 4.字符串数组初始化、key-value实现（使用 指定初始化器 和 宏定义）
使用`指定初始化器 Designated Initializers`和`宏定义`实现 `key-value`结构。

下面这个结构中，`char * title[]`是一个字符串数组。
使用`指定初始化器`（即中括号+整形/宏定义）来实现索引/key，赋值来设定对应字符串/value。

实际意义就是`title`中的存储的是字符串指针，第一个字符串指针指向字符串 none， 第四个字符串指针指向字符串 info。通过`title[DBG_LEVEL_NONE]`或`title[0]`都能取到 none 这个字符串。
```c
#define DBG_LEVEL_NONE      0
#define DBG_LEVEL_ERROR     1
#define DBG_LEVEL_WARNING   2
#define DBG_LEVEL_INFO      3

static const char * title[] = {
    [DBG_LEVEL_NONE] = "none",
    [DBG_LEVEL_ERROR] = DBG_STYLE_ERROR"error",
    [DBG_LEVEL_WARNING] = DBG_STYLE_WARNING"warning",
    [DBG_LEVEL_INFO] = "info",
};
```


## 5.字符串打印颜色 和 连续字符串自动合并

```c
printf("\033[31m"" my word")
```
其中，`\033[..m`标志着在终端打印时变色，`31`标志着前景红色。

另外，两个连续字符串连在一起会默认合并成一个字符串
```c
printf("\033[31m"" my word") 等同于 printf("\033[31m my word")
```

另外，值得注意的是如何将宏中的变量转换为字符串
```c
#define dbg_assert(expr, msg) {\
    if ((!expr)){\
        dbg_print(DBG_LEVEL_ERROR, DBG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, \
        "assert failed: "#expr", "msg);    \
        while(1);   \
        }\
}
// "assert failed: "#expr", "msg) 中的 #expr 会将判断条件直接变成字符串
```

## 6. 链表遍历中的宏定义`n_list_entry`，通过成员指针找到父结构的起始指针

```c

#define noffset_in_parent(parent_type, node_name) \
    ((char *)(&(((parent_type *) 0))->node_name))
/*  
    作用：
        此宏用于计算某个成员（node_name）在其父结构体（parent_type）中的偏移量。
    参数：
        parent_type：父结构体的类型。
        node_name：要计算偏移量的成员名称。
    实现：
        通过将parent_type *类型的指针设置为0（NULL），然后取这个成员的地址，
        基本上这个地址值代表了其偏移量。
        由于偏移量是独立于实际内存地址的，利用这种方式来计算偏移是安全的。
 */

#define noffset_to_parent(node, parent_type, node_name) \
    (parent_type *)((char *) node - noffset_in_parent(parent_type, node_name))
/*
    作用：
      此宏利用成员（node）的地址来计算并返回指向其父结构体（parent_type）的指针。
    参数：
      node：指向成员的指针。
      parent_type：父结构体的类型。
      node_name：成员名称。
    实现：通过node指针减去noffset_in_parent计算得到的偏移量，来得到指向父结构体的指针。
*/

#define nlist_entry(node, parent_type, node_name) \
    ((node) ? noffset_to_parent((node), parent_type, node_name) : (parent_type *) 0)
/*
  实现：
    它首先检查node是否为NULL，
    如果不是，它使用noffset_to_parent来获取并返回指向父结构体的指针；
    如果是，就返回NULL。
*/
```

## 7.static 和 static inline

- `static`限定定义函数的作用域，一般在`.c`文件中定义，且不对外放出接口。
  
  若有需要对外放出接口的函数，需要在对应的头文件中定义出来，方便别的文件调用。
  
  注意：此时不要给这个函数加static关键字

- `static inline void function(void);`静态内联函数，一般是在头文件中直接定义的短函数，导入该头文件的函数直接可以用。对应的，如果复杂函数，在头文件中声明最好不要加`static`防止报错。


记录一个报错：

情况 1：`main.c`文件引用`nlist.h`头文件，其中`nlist.h`定义了 \
`static void nlist_insert_first (nlist_t * list, nlist_node_t * node);`
然后再对应的`nlist.c`文件中实现这个函数，编译时报错，无法找到该函数。

修改：`void nlist_insert_first (nlist_t * list, nlist_node_t * node);`



## 8.mblock、nlist 和 exmsg

**workfolw**：

1. 工作线程启动，需要进行 消息队列 和 内存管理 的初始化。
2. 消息队列初始化：确定消息队列的缓存大小（通过数组自己来定义，每个元素就是每个消息，消息一般由结构体构成，大小统一。因为消息队列是统一定义，但是每个消息的类型还需要各应用来自己设置），启动后会调用 `fixq` 的init函数，创建一个对应于该工作线程的消息队列，后续工作线程只对消息队列里的消息进行处理。（PS：消息内存结构体其实保存的是读写指针、队列容量、已用容量、读写信号量、锁等信息，实际只通过`**buf`来指向实际定义的数组内存）
3. 设定一个连续数组，确定本协议栈可用的内存大小（非常重要，用这个结构来管理内存）。使用 `mblock` 的 `init` 函数，将（数组）内存分块，并进行链式存储，整个协议栈可用的内存空间受该模块控制。（PS：线程申请时需要有锁）
4. 应用线程 申请内存块，`mblock` 会调用 `alloc`，从空闲内存块（链式）的头结点返回一块内存。（超时或无结点会返回空值）
5. 应用线程 拿到内存块，对其进行操作，变为数据块，如把自己的数据填入等等。。（PS： 但其实这个`node`还是有部分空间是用来保存`pre`和`next`信息的）
6. 调用 `fixq_send` 函数将数据 node 发送到消息队列 `fixq_t` 中，此时工作线程收到信号量提醒，调用`fixq_recv`读取消息并后续处理。
7. 释放操作：处理完后，通过移动读指针，其实不需要清空队列操作。同时，在应用成功把数据送入消息队列后调用了free就释放block

注：其实 `block_t` 结构体 和 `fixq_t` 结构体都没有实际占用很大的内存空间，都是通过内置的指针来指向我们自己定义的（数组）内存。
`block_t` 是用来管理内存空间的，`fixq_t` 是用来管理消息队列的（工作线程与其交互，实现线程间通信）。

```c
// 内存块控制模块
typedef struct _mblock_t {
    nlist_t free_list; // 封装前的数据包（结点是数据块）
    void * start; // 指向内存块的起始地址
    nlocker_t locker; // ...
    sys_sem_t alloc_sem; // 通过 锁 和 信号量 来实现多个线程对链式存储结构的申请和释放
}mblock_t;

// 消息队列
typedef struct _fixq_t{
    int size;               // 消息队列容量
    int in, out, cnt;       // 读in 写out 位置索引，消息数 cnt
    void **buf;             // 消息缓存,一般buf是数组，传过来的是数组指针 *buf，所以用 **buf来跟踪这个指针，以实际只操纵buf数组
    nlocker_t locker;       // 消息队列互斥锁
    sys_sem_t recv_sem;     // 接收消息时使用的信号量
    sys_sem_t send_sem;     // 发送消息时使有的信号量
}fixq_t;
```
结构的包含关系：
- nlist_node_t：链表中的结点信息（只包含pre和next指针，不包含实际内容）
- nlist：链表的信息（只包含first、last和count信息，不包含实际内容）
- mblock：包含空闲链表、实际内存的指针（分块后链表化）、锁、信号量


