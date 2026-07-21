#include <string.h>
#include <stdint.h>

size_t strlen(const char *msg)
{
   size_t len;
   for(len=0;*msg++;len++);
   return len;
}

void *mempcpy(void *restrict dest, const void *restrict src, size_t n)
{
   int i;
   uint8_t *dstp = dest;
   const uint8_t *srcp = src;
   for(i=0;i<n;i++)
   {
      *dstp++ = srcp[i];
   }
   return dstp;
}

void *memcpy (void *restrict dest, const void *restrict src, size_t len)
{
   int i;
   uint8_t *dstp = dest;
   const uint8_t *srcp = src;
   for(i=0;i<len;i++)
   {
      dstp[i] = srcp[i];
   }
   return dest;
}

char *stpcpy(char *restrict dst, const char *restrict src)
{
    char  *p;

    p = mempcpy(dst, src, strlen(src));
    *p = '\0';

    return p;
}

char *strcpy(char *restrict dst, const char *restrict src)
{
    stpcpy(dst, src);
    return dst;
}

char *strcat(char *restrict dst, const char *restrict src)
{
    stpcpy(dst + strlen(dst), src);
    return dst;
}
