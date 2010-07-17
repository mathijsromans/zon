#include <cstdlib>

namespace Global
{
  volatile int timer;
  unsigned int turn = 0;
}

int randomNum(int max)
{
  return rand() % max;
}
