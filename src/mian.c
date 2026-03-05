#include <stdio.h>

int main()
{
	int a = 0x12345678;
	int b = 0x87654321;
	int c = a ^ b;
	printf("a: %08x\n", a);
	printf("b: %08x\n", b);
	printf("c: %08x\n", c);
	return 0;
}