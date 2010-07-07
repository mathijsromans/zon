#ifndef COORD_H
#define COORD_H

typedef unsigned int Direction;

struct Coord
{
  Coord() {}
  Coord(int myX, int myY) : x(myX), y(myY) {}
  Coord next() const { return Coord(x + 1, y); }
  Coord next(Direction dir) const { return Coord(x + dx[dir], y + dy[dir]); }
  unsigned int minDistanceTo(const Coord& other) const;
  bool operator==(const Coord& rhs) const { return x == rhs.x && y == rhs.y; }
  bool operator!=(const Coord& rhs) const { return !(*this == rhs); }
  Coord operator+(const Coord& rhs) const { return Coord(x + rhs.x, y + rhs.y); }
  Coord operator-(const Coord& rhs) const { return Coord(x - rhs.x, y - rhs.y); }
  Coord operator/(int rhs) const { return Coord(x / rhs, y / rhs); }
  Coord operator*(int rhs) const { return Coord(x * rhs, y * rhs); }
  bool operator<(const Coord& rhs) const { return y < rhs.y || ( y == rhs.y && x < rhs.x ); }

  int x, y;
  static const int dx[16],dy[16];
};

#endif
