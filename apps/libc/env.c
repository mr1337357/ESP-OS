#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lib.h"

char **__environ;

//returns the relevant entry in __environ
char **ggetenv(const char *key)
{
   char **tempenv;
   char *varp;
   int len;
   if(!__environ)
   {
      return 0;
   }
   len = strlen(key);
   //tempenv = __environ;
   for(tempenv = __environ; *tempenv; tempenv++)
   {
      if(!strncmp(*tempenv,key,len))
      {
         if(tempenv[0][len] == '=')
         {
            return tempenv;
         }
      }
   }
   return 0;
}

char *getenv (const char *__string)
{
   char **envp = ggetenv(__string);
   char *res;
   if(envp && *envp)
   {
      res = strchr(*envp,'=');
      if(res)
      {
         return res+1;
      }
   }
   return 0;
}

#define XSTR(a) STR(a)
#define STR(a) #a

int setenv(const char *name, const char *value, int overwrite)
{
   char *newentry;
   int numenv;
   char **entry;
   if(!__environ)
   {
      __environ = malloc(sizeof(char *));
      __environ[0] = 0;
      printf("line " XSTR(__LINE__) "\n");
   }
   printf("line " XSTR(__LINE__) "\n");
   entry = ggetenv(name);
   if(entry)
   {
      printf("line " XSTR(__LINE__) "\n");
      if(!overwrite)
      {
         return 0;
      }
      free(*entry);
   }
   else
   {
      printf("line " XSTR(__LINE__) "\n");
      for(numenv=0;__environ[numenv];numenv++);
      printf("line " XSTR(__LINE__) "\n");
      __environ = realloc(__environ, sizeof(char *) * (numenv+2));
      printf("line " XSTR(__LINE__) "\n");
      entry = &__environ[numenv];
      printf("line " XSTR(__LINE__) "\n");
      __environ[numenv+1]=0;
      printf("line " XSTR(__LINE__) "\n");
   }
   printf("line " XSTR(__LINE__) "\n");
   newentry = malloc(strlen(name)+strlen(value)+2);
   printf("line " XSTR(__LINE__) "\n");
   newentry[0] = 0;
   printf("line " XSTR(__LINE__) "\n");
   printf("line " XSTR(__LINE__) "\n");
   strcpy(newentry,name);
   printf("line " XSTR(__LINE__) "\n");
   strcat(newentry,"=");
   printf("line " XSTR(__LINE__) "\n");
   strcat(newentry,value);
   printf("line " XSTR(__LINE__) "\n");
   *entry = newentry;
   return 0;
}

int unsetenv(const char *name)
{
   return -1;
}
