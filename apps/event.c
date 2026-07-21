#include "lib.h"

int i;
int j;

void event_handler(int event, void *arg)
{
   print("asdf\n");
}

void print_loop()
{
   for(i = 0; i < 10; i ++)
   {
      print("hello world\n");
      delay(1000);
   }
}

int main(int argc, char **argv)
{
   print("hello world\n");
   print_loop();
   return 0;

}
