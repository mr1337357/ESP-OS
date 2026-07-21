#include <string.h>

#include "lib.h"

int launcherindex = 0;
int oldindex = 0;
int numfiles = 0;
int click = 0;

char namelist[16][64];

char hexlist[] = "0123456789ABCDEF";

void print_hex(int num)
{
   char hex[9];
   int i;
   for(i=7;i>=0;i--)
   {
      hex[i] = hexlist[num & 0x0F];
      num >>= 4;
   }
   hex[8] = 0;
   print(hex);
}

void event_handler(int event, void *arg)
{
   switch(event)
   {
      case 1: //EVENT_WHEEL
         int dir = (int)arg;
         if(dir&1)
         {
            launcherindex++;
         }
         if(dir&2)
         {
            launcherindex--;
         }
         if(dir&4)
         {
            click = 1;
         }
         if(launcherindex < 0)
         {
            launcherindex = numfiles - 1;
         }
         if(launcherindex >= numfiles)
         {
            launcherindex = 0;
         }
         break;
   }
}

void showmenu()
{
   int i;
   char linebuffer[64];
   char *destbuffer;

   print("\e0;0HLAUNCHER\n");
   for(i=0;i<numfiles;i++)
   {
      destbuffer = linebuffer;
      if(launcherindex == i)
      {
         destbuffer = stpcpy(destbuffer,"> ");
      }
      else
      {
         destbuffer = stpcpy(destbuffer,"  ");
      }
      destbuffer = stpcpy(destbuffer, namelist[i]);
      destbuffer = stpcpy(destbuffer,"\n");
      print(linebuffer);
   }
}

int main(int argc, char **argv)
{
   int appdir = open("/apps",FILEMODE_R);
   numfiles = 0;
   launcherindex = 0;
   while(read(appdir,namelist[numfiles],64) > 0)
   {
      //print(namelist[numfiles]);
      //print("\n");
      numfiles++;
   }
   close(appdir);
   while(1)
   {
      delay(100);
      if(launcherindex != oldindex)
      {
         oldindex = launcherindex;
         showmenu();
      }
      if(click)
      {
         click = 0;
         run(namelist[launcherindex]);
      }
   }
   return 0;
}
