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