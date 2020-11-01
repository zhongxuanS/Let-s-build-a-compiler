//
// Created by szx on 2020/10/31.
//
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char val;
    scanf("%d", &val);

    printf(".text\n");
    printf(".global _start\n");
    printf("_start:\n");

    printf("movl $%c, %%eax\n", val);

    scanf("%d", &val);

    /* save old val */
    printf("pushl %%eax\n");

    /* save new val */
    printf("movl $%c, %%eax\n", val);

    // add value
    printf("add (%%esp) %%eax\n");

    // restore esp
    printf("add $4 %%esp\n");

    /* return the result */
    printf("movl %%eax, %%ebx\n");
    printf("movl $1, %%eax\n");
    printf("int $0x80\n");
}