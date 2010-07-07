#ifndef PATH_H
#define PATH_H

#include "rectangle.h"

#include <deque>
#include <vector>
#include <cassert>

class Path
{
  public:
    Path() : m_valid( false ), m_index(0) {}
    Path(const Coord& start) : m_valid(true), m_start(start), m_finish(start), m_boundingBox(start), m_index(0) {}
    Direction getDirection() const { assert(m_index < m_path.size()); return m_path[m_index]; }
    void next() { ++m_index; }
    const Rectangle& getBoundingBox() const { return m_boundingBox; }
    const std::vector<Direction>& getDirs() const { return m_path; }
    Coord getStart() const { return m_start; }
    Coord getFinish() const { return m_finish; }
    void append(Direction d);
    void append(const Path& newPath);
    void reverse();
    bool isValid() const { return m_valid; }
  private:
    std::vector<Direction> m_path;
    bool m_valid;
    Coord m_start, m_finish;
    Rectangle m_boundingBox;
    unsigned int m_index; // move to private
};

#endif
