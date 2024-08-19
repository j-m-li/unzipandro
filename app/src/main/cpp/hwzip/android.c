
#include <stdlib.h>
#include "hwzip.c"

void * __memcpy_chk(void * dest, const void * src, size_t len, size_t destlen)
{
    return memcpy(dest, src, len);
}

void * __memset_chk(void * dest, int c, size_t len, size_t destlen)
{
    return memset(dest, c, len);
}

void * __memmove_chk(void * dest, const void * src, size_t len, size_t destlen)
{
    return memmove(dest, src,len);
}

size_t __strlen_chk(const char *s, size_t s_len)
{
    size_t ret = strlen(s);
    return ret;
}


