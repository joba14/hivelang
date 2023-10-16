
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(void)
{
	int a = 69;
	int* p = &a;

	fprintf(stdout, "%p\n", p);
	fprintf(stdout, "%s\n", (char*)p);
	return 0;
}
