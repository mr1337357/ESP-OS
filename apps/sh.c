#include "lib.h"

int main(int argc, char **argv)
{
   int readlen;
   char cmd[256];
   char *readlen_str = "readlen x\n";
   while(1)
   {
      print("sh> ");
      readlen = read(0, cmd, 256);
      if(readlen < 1)
      {
         readlen_str[8] = '0' + readlen;
         print(readlen_str);
      }
   }
   return 0;
}
