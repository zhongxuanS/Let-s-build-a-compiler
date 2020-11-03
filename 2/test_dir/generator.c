//
// Created by szx on 2020/10/31.
//
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