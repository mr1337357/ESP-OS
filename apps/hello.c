#include <stdio.h>

int main(int argc, char **argv)
{
   int i;
   printf("hello %s\n", "world");
   for(i=0;i<100;i++)
   {
      printf("%s(%x) = %s(%d)\n","hex",i,"dec",i);
   }
   return 0;
}
