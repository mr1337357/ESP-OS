#include <stdlib.h>
#include <string.h>

#include "lib.h"

char **__environ;

char *ggetenv(const char *key)
{
   char **tempenv;
   char *varp;
   int len;
   if(!__environ)
   {
      return 0;
   }
   len = strlen(key);
   tempenv = __environ;
   for(varp = *tempenv++; varp; varp = *tempenv++)
   {
      if(!strncmp(varp,key,len))
      {
         if(varp[len]=='=')
         {
            return varp;
         }
      }
   }
   return 0;
}

char *getenv (const char *__string)
{
   char *res = ggetenv(__string);
   if(res)
   {
      res = strchr(res,'=');
      if(res)
      {
         return res+1;
      }
   }
   return 0;
}

int setenv(const char *name, const char *value, int overwrite)
{
   if(!__environ)
   {
      __environ = malloc(sizeof(char *));
      __environ[0] = 0;
   }
   return -1;
}

int unsetenv(const char *name)
{
   return -1;
}
