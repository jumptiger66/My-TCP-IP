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

## 6. 