#ifndef FIELDMAP_H
#define FIELDMAP_H

#include "config.h"
#include "coord.h"
#include <vector>

template <typename T>
class FieldMap
{
  public:
    FieldMap() : m_data(MAPWIDTH * MAPHEIGHT) {}
    ~FieldMap() {}
    T& operator()(const Coord& c) { return m_data[MAPWIDTH * c.y + c.x]; }
    const T& operator()(const Coord& c) const { return m_data[MAPWIDTH * c.y + c.x]; }
  private:
    std::vector<T> m_data;
};

#endif
