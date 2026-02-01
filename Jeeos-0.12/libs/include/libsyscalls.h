/*
 * @Author: Jee Hsu
 * @Description: 库模块 - lib_syscalls
 *
 * Copyright (c) 2025 Jee Hsu. SPDX-License-Identifier: MIT
 */
#ifndef _LIB_SYSCALLS_H
#define _LIB_SYSCALLS_H

#ifndef __SYSCALL_DEFINEx
#define __SYSCALL_DEFINEx(x, name, ...)

#endif

//传递一个参数所用的系统调用宏
//宏汇编函数解析
//l1: 系统服务号
//l2: 第一个参数
//l3: 触发中断
//l4: 处理返回结果
#define __SYSCALL_PARAM1(intnr,rets,pval1) \
__asm__ __volatile__(\
         "movq %[inr],%%rax\n\t"\
         "movq %[prv1],%%rbx\n\t"\
         "int $255 \n\t"\
         "movq %%rax,%[retval] \n\t"\
         :[retval] "=r" (rets)\
         :[inr] "r" (intnr),[prv1]"r" (pval1)\
         :"rax","rbx","cc","memory"\
    )

#define __SYSCALL_PARAM2(intnr,rets,pval1,pval2) \
__asm__ __volatile__(\
         "movq %[inr],%%rax \n\t"\
         "movq %[prv1],%%rbx \n\t"\
         "movq %[prv2],%%rcx \n\t"\
         "int $255 \n\t"\
         "movq %%rax,%[retval] \n\t"\
         :[retval] "=r" (rets)\
         :[inr] "r" (intnr),[prv1]"r" (pval1),\
         [prv2] "r" (pval2)\
         :"rax","rbx","rcx","cc","memory"\
    )

#define __SYSCALL_PARAM3(intnr,rets,pval1,pval2,pval3) \
__asm__ __volatile__(\
         "movq %[inr],%%rax \n\t"\
         "movq %[prv1],%%rbx \n\t"\
         "movq %[prv2],%%rcx \n\t"\
         "movq %[prv3],%%rdx \n\t"\
         "int $255 \n\t"\
         "movq  %%rax,%[retval] \n\t"\
         :[retval] "=r" (rets)\
         :[inr] "r" (intnr),[prv1]"g" (pval1),\
         [prv2] "g" (pval2),[prv3]"g" (pval3)\
         :"rax","rbx","rbx","rcx","rdx","cc","memory"\
    )

#define __SYSCALL_PARAM4(intnr,rets,pval1,pval2,pval3,pval4) \
__asm__ __volatile__(\
         "movq %[inr],%%rax \n\t"\
         "movq %[prv1],%%rbx \n\t"\
         "movq %[prv2],%%rcx \n\t"\
         "movq %[prv3],%%rdx \n\t"\
         "movq %[prv4],%%rsi \n\t"\
         "int $255 \n\t"\
         "movq %%rax,%[retval] \n\t"\
         :[retval] "=r" (rets)\
         :[inr] "r" (intnr),[prv1]"g" (pval1),\
         [prv2] "g" (pval2),[prv3]"g" (pval3),\
         [prv4] "g" (pval4)\
         :"rax","rbx","rcx","rdx","rsi","cc","memory"\
    )

#endif // _LIB_SYSCALLS_H
