
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(void)
{
	const char* string = "Hi\\n\\n";
	unsigned char bytes[100];
	int64_t curr = 0;

	for (const char* iter = string; iter != string + sizeof(string) - 1;)
	{
		fprintf(stdout, "%s\n", iter);
		getchar();

		if (*iter == '\\' && iter + 2 <= string + sizeof(string) - 1)
		{
			switch (*(iter + 1))
			{
				case 'n':
				{
					bytes[curr++] = 0x0a;
					iter += 2;
				} break;
			}
		}
		else
		{
			bytes[curr++] = (unsigned char)*iter;
			++iter;
		}
	}

	for (int64_t i = 0; i < curr; ++i)
		fprintf(stdout, "%#02x ", bytes[i]);
	fprintf(stdout, "\n");
	return 0;
}
