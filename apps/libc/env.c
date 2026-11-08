#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lib.h"

char **__environ;

//returns the relevant entry in __environ
int ggetenv(const char *key)
{
   char *varp;
   int len;
   int index;
   if(!__environ)
   {
      return -1;
   }
   len = strlen(key);
   for(index = 0; __environ[index]; index++)
   {
      if(!strncmp(__environ[index],key,len))
      {
         if(__environ[index][len] == '=')
         {
            return index;
         }
      }
   }
   return -1;
}

char *getenv (const char *__string)
{
   char *res;
   int index = ggetenv(__string);
   if(index >= 0)
   {
      res = strchr(__environ[index],'=');
      if(res)
      {
         return res+1;
      }
   }
   return 0;
}

#define XSTR(a) STR(a)
#define STR(a) #a

//      printf("line " XSTR(__LINE__) "\n");

int setenv(const char *name, const char *value, int overwrite)
{
   char *newentry;
   int index;
   if(!__environ)
   {
      __environ = malloc(sizeof(char *));
      __environ[0] = 0;
   }
   index = ggetenv(name);
   if(index >= 0)
   {

      if(!overwrite)
      {
         return 0;
      }
      free(__environ[index]);
   }
   else
   {
      for(index = 0; __environ[index]; index++);

      __environ = realloc(__environ, sizeof(char *) * (index+2));
      __environ[index+1]=0;
   }
   newentry = malloc(strlen(name)+strlen(value)+2);
   newentry[0] = 0;
   strcpy(newentry,name);
   strcat(newentry,"=");
   strcat(newentry,value);
   __environ[index] = newentry;
   return 0;
}

int unsetenv(const char *name)
{
   return -1;
}
