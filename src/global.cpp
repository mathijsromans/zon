#include <cstdlib>

volatile int timer;

int randomNum(int max)
{
  return rand() % max;
}
