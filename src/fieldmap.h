#ifndef FIELDMAP_H
#define FIELDMAP_H

#include "config.h"
#include "coord.h"
#include <vector>

template <typename T>
class FieldMap
{
  public:
    FieldMap( unsigned int width, unsigned int height ) : m_width(width), m_height(height), m_data(width * height) {}
    ~FieldMap() {}
    T& operator()(const Coord& c) { return m_data[m_width * c.y + c.x]; }
    const T& operator()(const Coord& c) const { return m_data[m_width * c.y + c.x]; }
  private:
    unsigned int m_width, m_height;
    std::vector<T> m_data;
};

#endif
