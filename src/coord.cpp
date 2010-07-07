#include "coord.h"

#include <cstdlib>
#include <algorithm>

const int Coord::dx[16] = { +1, +1,  0, -1, -1, -1,  0, +1, +1, +1,  0, -1, -1, -1,  0, +1 };
const int Coord::dy[16] = {  0, +1, +1, +1,  0, -1, -1, -1,  0, +1, +1, +1,  0, -1, -1, -1 };

unsigned int Coord::minDistanceTo(const Coord& other) const
{
  unsigned int mindx = abs(other.x - x);
  unsigned int mindy = abs(other.y - y);

// minimum distance from here to target is 2 * long edge + short edge
// because you get a discount for the short edge as you travel diagonally
  return mindx + mindy + std::max(mindx, mindy);
}
