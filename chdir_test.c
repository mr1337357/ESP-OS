#include <stdio.h>
#include <string.h>
#include <unistd.h>

char cwd[256];

int diy_chdir(char *path)
{
   char newcwd[256];
   char *writep;
   char *readp;
   newcwd[0] = 0;
   if(!path)
   {
      return -1;
   }
   if(path[0] != '/')
   {
      strcpy(newcwd,cwd);
   }
   writep = newcwd+strlen(newcwd);//strnul(newcwd);
   for(readp = strtok(path,"/"); readp && *readp; readp = strtok(0,"/"))
   {
      printf("%s\n",readp);
      if(!strcmp(readp,"."))
      {
         //do nothing
      }
      else if(!strcmp(readp,".."))
      {
         //unwind a level
         writep = strrchr(newcwd,'/');
         if(writep && writep != newcwd)
         {
            *writep = 0;
         }
         else
         {
            writep = newcwd;
         }
      }
      else
      {
         writep = stpcpy(writep,"/");
         writep = stpcpy(writep,readp);
      }
   }
   strcpy(cwd, newcwd);
   return 0;
}

int main(int argc, char **argv)
{
   memset(cwd, 0, 256);
   getcwd(cwd,256);
   printf("cwd %s\n",cwd);
   if(argc >= 2)
   {
      diy_chdir(argv[1]);
   }
   printf("cwd %s\n",cwd);
   return 0;
}
