//
// Created by 孙昊 on 2024/6/5.
//

#include "dbg.h"
#include "sys_plat.h"
#include <stdarg.h> // 可变参数头文件

void dbg_print(int m_level, int s_level, const char * file, const char * func, int line, const char * fmt, ...){
    /*
     * param: m_level, 当前模块需要的调试等级
     * param: s_level, 调试打印函数的默认等级（如dbg_warning默认等级就是DBG_LEVEL_WARNING）
     * param: file, 指向路径字符串首部的指针（也是字符串）
     * param: func,
     * param: len, 信息提示的具体行号
     * param: fmt, 有格式的提示信息（字符串）
     */

    static const char * title[] = {
        [DBG_LEVEL_NONE] = "none",
        [DBG_LEVEL_ERROR] = DBG_STYLE_ERROR"error",
        [DBG_LEVEL_WARNING] = DBG_STYLE_WARNING"warning",
        [DBG_LEVEL_INFO] = "info",
    };

    // 仅在当前模块需要的调试等级大于函数默认等级时打印
    // e.g. 若当前项目的调试等级为WARNING，那么只会输出所有调用dbg_warning,dbg_info的函数，不会触发dgb_error
    // 若等级是项目调试等级是NONE，那么不会触发任何dbg调试函数
     if (m_level >= s_level){
        // 预处理：去除目录的路径，提取文件名
        const char * end = file + plat_strlen(file); // 定位到路径尾部
        while (end >= file){
            if ((*end == '\\') || (*end == '/')){ // 遇到第一个路径分隔符停止
                break;
            }
            end--;
        }
        end++; // 指向文件名第一个字符

        plat_printf("%s(%s-%s-%d): ", title[s_level], end, func, line);

        char str_buf[128];
        va_list args;
        va_start(args, fmt);
        plat_vsnprintf(str_buf, sizeof(str_buf), fmt, args);

        plat_printf("%s\n"DBG_STYLE_RESET, str_buf);
        va_end(args);
     }

}
