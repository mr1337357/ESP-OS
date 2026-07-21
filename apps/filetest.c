#include "lib.h"

int main()
{
   char buff[21];
   int len;
   int fd;
   fd = open("/test.txt",FILEMODE_R);
   do
   {
      len = read(fd,buff,20);
      buff[len] = 0;
      print(buff);
   } while(len);
   delay(10000);
   close(fd);
   return 0;
}
