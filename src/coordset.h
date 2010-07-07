#ifndef COORDSET_H
#define COORDSET_H

#include <bitset>
#include <set>
#include "coord.h"

// efficient implementation of a large set of coordinates
template<size_t W, size_t H>
class CoordSet
{
  public:
    CoordSet() {}
    ~CoordSet() {}
    void set(const Coord& c, bool value)    { m_data.set(c.y * W + c.x, value); }
    void add(const Coord& c)    { m_data.set(c.y * W + c.x); }
    void remove(const Coord& c) { m_data.reset(c.y * W + c.x); }
    bool has(const Coord& c) const { return m_data[(c.y * W + c.x)]; }
    void clear() { m_data.reset(); }
  private:
    std::bitset<H * W> m_data;
};

// efficient implementation of a small set of coordinates
class SmallCoordSet
{
  public:
    SmallCoordSet() {}
    ~SmallCoordSet() {}
    void add(const Coord& c)    { m_data.insert(c); }
    void remove(const Coord& c) { m_data.erase(c); }
    bool has(const Coord& c) const { return m_data.count(c); }
    void clear() { m_data.clear(); }
  private:
    std::set<Coord> m_data;
};

#endif