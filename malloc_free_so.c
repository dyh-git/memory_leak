#include <unistd.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
 
void malloc_free_so(size_t size)
{
	char *str = "This is a computer.\n";
	char *p = NULL;

	p = (char *)malloc(size);
	memset(p, 0x00, size);
	
	strncpy(p, str, (strlen(str) + 1));
	printf("%s, %d, %s\n", __func__, __LINE__, str);
	
	free(p);
	
	return;
}
