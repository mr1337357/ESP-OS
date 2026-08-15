#include "lib.h"

void print_loop()
{
   int i;
   char *num = "  \n";
   for(i = 0; i < 10; i ++)
   {
      print("hello world");
   }
   num[1] = '0' + i;
   print(num);
}

int main(int argc, char **argv)
{
   print("hello world\n");
   print_loop();
   return 0;

}
