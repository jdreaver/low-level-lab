#include <stdio.h>
#include <unistd.h>

#define SYSCALL_NO 451

int main()
{
	printf("sys_eudyptula(1, 1) = %lu\n", syscall(SYSCALL_NO, 1, 1));
	printf("sys_eudyptula(0x7c1c,0xaf2f50d1) = %lu\n", syscall(SYSCALL_NO, 0x7c1c,0xaf2f50d1));
}
