#include <stdint.h>
#include <stddef.h>

void* memset(void* bufptr, int val, size_t size)
{
	uint8_t* buf = (uint8_t*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = val;
	
	return bufptr;
}

void* memcpy(void* dstptr, const void* srcptr, size_t size)
{
	uint8_t* dst = (uint8_t*) dstptr;
	const uint8_t* src = (const uint8_t*) srcptr;
	for(size_t i = 0; i < size; i++)
	{
		dst[i] = src[i];
	}
	return dstptr;
}

int memcmp(const void* ap, const void* bp, size_t size)
{
	const uint8_t* a = (const uint8_t*) ap;
	const uint8_t* b = (const uint8_t*) bp;
	for(size_t i = 0; i < size; i++)
	{
		if(a[i] < b[i])
			return -1;
		else if(b[i] < a[i])
			return 1;
	}
	return 0;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

char* strcpy(char* ds, const char* ss)
{
	return memcpy(ds, ss, strlen(ss) + 1);
}

char* strcat(char* dest, const char* after)
{
	char* end = strlen(dest) + dest;
	memcpy(end, after, strlen(after) + 1);
	return dest;
}

// apparently it still needs these for whatever reason.
void free(void* a){}
void fflush(){}
void __dyn_tls_init_callback(){}
void _pei386_runtime_relocator(){}
void __main(){}
void _errno(){}
void __dllonexit(){}

extern int MessageBoxA(void*, const char*, const char*, int);

void* malloc(size_t a)
{
	MessageBoxA(NULL, "malloc called", "Kernel32 Forwarder", 0);
	*(int*) 0 = 0;
}
