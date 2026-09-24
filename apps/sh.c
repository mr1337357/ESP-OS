#include "lib.h"

char *msg = "readlen 0\n";

int main(int argc, char **argv)
{
   int readlen;
   char cmd[256];
   while(1)
   {
      print("sh> ");
      readlen = read(0, cmd, 256);
      if(readlen < 0)
      {
         print("read error\n");
      }
      else if(readlen > 0)
      {
         msg[8] = '0' + readlen;
         print(msg);
      }
      else
      {
         print("read zero\n");
      }
   }
   return 0;
}
