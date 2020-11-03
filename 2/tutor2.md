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

下面我们再回过头来看怎么去解析四则运算表达式。为了简单起见，我们先把问题简单化，只考虑个位数的四则运算。

先考虑`1+1`这样的加法形式，不考虑括号，最直观的解析思路就是先读一个`1`，碰到`+`说明`1`要和另外一个数相加，那么下一个数必须也是一个数字。如果不是数字那就要报错。
如果下一个字符是数字，那么就要和已经读取的数字进行相加。

思路是有了，那么要怎么去实现呢？

写代码最关键的是要先把思路伪代码化，人脑是用来思考不是用来记忆的。按照上面的思路我们把伪代码写下来：
```c
    look = getchar()
    if(isdigit(look))
    {
        expect('+')
        look1 = getchar()
        add(look, look1)
    }
    else
    {
        abort("please input digit");
    }
```
下面就用真实的代码实现。
我们把处理表达式的函数命名为`Expression`函数。在这个函数中按照上面的伪代码进行处理。
```c
//
// Created by szx on 2020/10/31.
//
#include <stdio.h>
#include <stdlib.h>

#include "../cradle.h"

void Term()
{
    // save current value into eax
    sprintf(tmp,"movl $%c, %%eax", GetNum());
    EmitLn(tmp);
}

void Add()
{
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
}

void Expression()
{
    Term();
    if(IsAddop(Look))
    {
        EmitLn("pushl %eax");
        Match('+');
        Term();
        Add();
    }
}

int main()
{
    Init();
    EmitLn(".text");
    EmitLn(".global _start");
    EmitLn("_start:");
    Expression();

    /* return the result */
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
    return 0;
}
```
现在我们已经可以处理`1+1`这样的表达式了。不知道大家有没有注意到，我们一直都在说四则运算，但是我们并没有定义什么样的语法才符合四则运算。这个非常关键，有点编程经验的人都知道，每个语言有自己的语法，不符合语法的话编译器会编译不过。
那么怎么定义个语法呢？
在编译器中一般使用`BNF`方式来定义一个语法。
比如说`1+1`可以被表示为:
```
Expr := Term + Term
Term := digit
```
在`BNF`中，分为终结符和非终结符，非终结符可以由终结符或其他非终结符表示，终结符一般由数字、字符构成。这里`Term`和`Expr`就是非终结符，`Term`由`digit`构成。`digit`就是一个终结符，表示数字。`Expr`由`Term`构成。

有了`BNF`，就可以把语法定义公式化，方便大家理解。

下面我们再增加减法。那么加减法的`BNF`就可以写成：
```
Expr := Term + Term | Term - Term
Term := digit
```
只需要在上面的例子中增加几行代码就可以实现。
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cradle.h"

void Term()
{
    // save current value into eax
    sprintf(tmp,"movl $%c, %%eax", GetNum());
    EmitLn(tmp);
}

void Add()
{
    Match('+');
    Term();
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
}

void Sub()
{
    Match('-');
    Term();
    EmitLn("subl (%esp), %eax");
    EmitLn("negl %eax");
    EmitLn("addl $4, %esp");
}

void Expression()
{
    Term();
    if(strchr("+-", Look))
    {
        EmitLn("pushl %eax");
        switch (Look)
        {
        case '+':
            Add();
            break;
        case '-':
            Sub();
            break;
        default:
            break;
        }
    }
}

int main()
{
    Init();
    EmitLn(".text");
    EmitLn(".global _start");
    EmitLn("_start:");
    Expression();

    /* return the result */
    EmitLn("movl %eax, %ebx");
    EmitLn("movl $1, %eax");
    EmitLn("int $0x80");
    return 0;
}
```
但是运行`1-2`的时候发现结果不对，这是因为我们在减法的时候计算的表达式为：
`2-1`。这是因为我们想把结果保存到`eax`寄存器里。怎么修改呢？
只需要取负就行了。把`Sub`函数改成这样：
```c
void Sub()
{
    Match('-');
    Term();
    EmitLn("subl (%esp), %eax");
    EmitLn("negl %eax");
    EmitLn("addl $4, %esp");
}
```


