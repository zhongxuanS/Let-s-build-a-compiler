//
// Created by szx on 2020/10/31.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cradle.h"

void Factor()
{
    sprintf(tmp,"movl $%c, %%eax", GetNum());
    EmitLn(tmp);
}

void Mul()
{
    Match('*');
    Factor();
    EmitLn("imull (%esp), %eax");
    EmitLn("addl $4, %esp");
}

void Div()
{
    Match('/');
    Factor();
    EmitLn("imull (%esp), %eax");
    EmitLn("addl $4, %esp");
}

void Term()
{
    Factor();
    while (strstr("*/", Look))
    {
        EmitLn("pushl %eax");
        switch (Look)
        {
        case '*':
            Mul();
            break;
        case '/':
            Div();
            break;
        default:
            Expected("* or /");
            break;
        }
    }
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
    while(strchr("+-", Look))
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
            Expected("+ or -");
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