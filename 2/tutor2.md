### 开始
一般教编译器的书都会用一个表达式计算器来作为例子，本教程也不例外。
这个章节的目的就是教会大家怎么去实现一个表达式计算器，我们会去解析表达式并生成
对应的汇编代码。

做开发最重要的事情就是要知道自己要解决的问题是什么，也就是需求。对于解析表达式来说，
我们首先要定义表达式的格式。符合这个格式的表达式才能会被正常解析。
我们要解析的表达式是一个普通的四则运算表达式，支持小括号。比如说：
```c
    x = 2 * y + 3 / (4 * z)
```

为了简单起见，我们先感受一下一个最简单的编译器是什么样子的。
```c
   //
   // Created by szx on 2020/10/31.
   //
   #include <stdio.h>
   #include <stdlib.h>
   
   int main()
   {
       char val = getchar();
   
       printf(".text\n");
       printf(".global _start\n");
       printf("_start:\n");
   
       printf("movl $%c, %%eax\n", val);
   
       /* return the result */
       printf("movl %%eax, %%ebx\n");
       printf("movl $1, %%eax\n");
       printf("int $0x80\n");
   } 
```
编译运行:
```commandline
gcc generator.c -o generator
./generator > test.s
8
as --32 -o test.o test.s
ld -m elf_i386 test.o -o test
./test
echo $?
```
首先看一下生产的汇编代码，考虑到是一个入门教程，顺便讲讲汇编。
```asm
 .text
 .global _start
 _start:
 movl $8,  %eax
 movl %eax, %ebx
 movl $1, %eax
 int $0x80
```
汇编中`.`开头的字符串是汇编指示，`.text`表示代码段，可读可执行。`.text`后面
都属于代码段。`.global _start`声明了一个`_start`的符号，并且是`.global`的。
也就是说可以链接器看到链接的。`_start`是一个约定，相当于`main`函数。

那么`_start`对应的指令表示什么意思呢，最关键的是`int $0x80`，在linux上通过他
可以进行系统调用，eax寄存器中为调用的功能号，ebx、ecx、edx、esi等等寄存器则依次为参数。
查看`/usr/include/asm/unistd.h`可以看出`exit`的号是1。所以这里是调用了
`exit`函数，并且参数为8。

这就是一个最简单的编译器，从输入设备读取字符串，生成汇编代码。当然离真正的编译器还差很远。
因为目前我们关注的是词法和语法解析，也就是前端工作，后端的事情先暂时不管。饭要一口一口吃嘛。

