#include <wiringPi.h>
int main (void)
{
  wiringPiSetup () ;
  pinMode (30, OUTPUT) ;
  for (;;)
  {
    digitalWrite (0, HIGH) ; delay (20) ;
    digitalWrite (0,  LOW) ; delay (20) ;
  }
  return 0 ;
}
