/*
 * Public domain
 */

#include <stdlib.h>
#include "hwzip.c"

void * __memcpy_chk(void * dest, const void * src, size_t len, size_t destlen)
{
    char	*p;
    p = dest;
    while (len--) *p++ = *(char *)src++;
    return dest;
}

void * __memset_chk(void * dest, int c, size_t len, size_t destlen)
{
    char	*q;
    q = dest;
    while (len--) *(char *)dest++ = c;
    return q;
}

void * __memmove_chk(void * dest, const void * src, size_t len, size_t destlen)
{
    char *p;

    if (dest < src) {
	p = dest;
	while (len--) *p++ = *(char *)src++;
	return dest;
    }
    p = dest + len;
    src += len;
    while (len--) *--p = *(char *)--src;
    return dest;
}

char * __strrchr_chk(const char *str, int ch, size_t destlen)
{
    char *p;
    char *ret = NULL;

    p = str;
    while (*p) {
	if (*p == (char)ch) {
	    ret = p;
	}
	p++;
    }
    return ret;
}

size_t __strlen_chk(const char *s, size_t s_len)
{
    size_t r;
    r = 0;
    while (*s) {
	r++;
	s++;
    }
    return r;
}


