#ifndef COORDSET_H
#define COORDSET_H

#include <boost/dynamic_bitset.hpp>
#include <set>
#include "coord.h"

// efficient implementation of a large set of coordinates
class CoordSet
{
  public:
    CoordSet( unsigned int width, unsigned int height ) : m_width(width), m_height(height), m_data(width * height) {}
    ~CoordSet() {}
    void set(const Coord& c, bool value)    { m_data.set(c.y * m_width + c.x, value); }
    void add(const Coord& c)    { m_data.set(c.y * m_width + c.x); }
    void remove(const Coord& c) { m_data.reset(c.y * m_width + c.x); }
    bool has(const Coord& c) const { return m_data[(c.y * m_width + c.x)]; }
    void clear() { m_data.reset(); }
  private:
    unsigned int m_width, m_height;
    boost::dynamic_bitset<> m_data;
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