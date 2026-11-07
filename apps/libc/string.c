#include <string.h>

size_t strlen(const char *msg)
{
   size_t len;
   for(len=0;*msg++;len++);
   return len;
}

void *mempcpy(void *restrict dest, const void *restrict src, size_t n)
{
   int i;
   unsigned char *dstp = dest;
   const unsigned char *srcp = src;
   for(i=0;i<n;i++)
   {
      *dstp++ = srcp[i];
   }
   return dstp;
}

void *memcpy (void *restrict dest, const void *restrict src, size_t len)
{
   int i;
   unsigned char *dstp = dest;
   const unsigned char *srcp = src;
   for(i=0;i<len;i++)
   {
      dstp[i] = srcp[i];
   }
   return dest;
}

char *stpcpy(char *restrict dst, const char *restrict src)
{
   char *dstp = dst;
   const char *srcp = src;
   while(*srcp != 0)
   {
      *dstp++ = *srcp++;
   }
   *dstp = 0;
   return dstp;
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

int strncmp(const char *a, const char *b, size_t n)
{
   int i;
   for(i=0;i<n;i++)
   {
      if(a[i] == 0 || a[i] != b[i])
      {
         return a[i] - b[i];
      }
   }
   return 0;
}

int strcmp(const char *a, const char *b)
{
   return strncmp(a,b,strlen(a)+1);
}

char *strchr(const char *s, int c)
{
   int i;
   for(i=0;s[i];i++)
   {
      if(s[i] == c)
      {
         return (char *)&s[i];
      }
   }
   return 0;
}
