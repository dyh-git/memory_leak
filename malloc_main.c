#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include <dlfcn.h>

extern void *malloc_so(size_t size);
extern void free_so(void *ptr);
extern void malloc_free_so(size_t size);
extern void *malloc_a(size_t size);
extern void free_a(void *ptr);
extern void malloc_free_a(size_t size);

#define BUF_LENGTH			30
#define MEM_CONTENT_LENGTH	128

int malloc_stub_enable = 1;
int free_stub_enable = 1;

typedef void *(*malloc_t)(size_t size);
malloc_t malloc_f = NULL;

typedef void (*free_t)(void *p);
free_t free_f = NULL;

void *malloc(size_t size)
{
	void *p = NULL;
	void *caller = NULL;
	char buf[MEM_CONTENT_LENGTH] = {0x00};
	FILE *fp = NULL;
	Dl_info info;
	
	if (malloc_stub_enable) {
		malloc_stub_enable = 0;
		p = malloc_f(size);
		caller = __builtin_return_address(0);
		
		memset(&info, 0x00, sizeof(Dl_info));
		(void)dladdr((void*)caller, &info);
		sprintf(buf, "./tmp/%p.mem", p);
		fp = fopen(buf, "w");
		fprintf(fp, "[+%p]malloc --> caller: %s, addr: %p, size: %lu\n", caller, info.dli_sname, p, size);
		fflush(fp);
		malloc_stub_enable = 1;
		
		return p;
	} else {
		return malloc_f(size);
	}
}

void free(void *p)
{
	char buf[MEM_CONTENT_LENGTH] = {0x00};
	
	if (free_stub_enable) {
		free_stub_enable = 0;
		
		sprintf(buf, "./tmp/%p.mem", p);
		
		if (unlink(buf) < 0) {
			printf("double free: %p\n", p);
		}
		
		free_stub_enable = 1;
	}
	
	free_f(p);
}

static void init_hook()
{
	malloc_f = dlsym(RTLD_NEXT, "malloc");
	free_f = dlsym(RTLD_NEXT, "free");
}

int main(int argc, char *argv[])
{
	char *p = NULL;
	
	init_hook();

#if 0	
	p = (char *)malloc(BUF_LENGTH);			//工程本身出现的内存泄漏
	memset(p, 0x00, BUF_LENGTH);

	free(p);
#endif
#if 1
	p = (char *)malloc_so(BUF_LENGTH);		//动态库出现的内存泄漏
	free_so(p);
#endif
#if 0
	malloc_free_so(BUF_LENGTH);
#endif
#if 0
	p = (char *)malloc_a(BUF_LENGTH);		//静态库出现的内存泄漏
	free_a(p);
#endif
#if 0
	malloc_free_a(BUF_LENGTH);
#endif

	return 0;	
}