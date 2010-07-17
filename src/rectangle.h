#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "coord.h"
#include <iterator>
#include <cstdlib>

class Rectangle
{
  public:
    Rectangle() : m_topLeft(0, 0), m_bottomRight(0, 0) {}
    explicit Rectangle(const Coord& c) : m_topLeft(c), m_bottomRight(Coord(c.x + 1, c.y + 1)) {}
    Rectangle(const Coord& topLeft, const Coord& bottomRight) : m_topLeft(topLeft), m_bottomRight(bottomRight) {}
    Rectangle(const Rectangle& rect) : m_topLeft(rect.m_topLeft), m_bottomRight(rect.m_bottomRight) {}
    Rectangle(int x1, int y1, int x2, int y2) : m_topLeft(x1, y1), m_bottomRight(x2, y2) {}
    virtual ~Rectangle() {}
    int getArea() const { return ( m_bottomRight.x - m_topLeft.x ) * ( m_bottomRight.y - m_topLeft.y ); }
    void add(const Coord& c);
    virtual void resizeTo(const Rectangle& rect);
    Coord center() const { return (m_topLeft + m_bottomRight) / 2; }
    bool contains(const Coord& c) const {return c.x>=m_topLeft.x && c.x<m_bottomRight.x && c.y>=m_topLeft.y && c.y<m_bottomRight.y;}
    Rectangle intersection(const Rectangle& other) const;
    Rectangle unionWith(const Rectangle& other) const;
    bool intersects(const Rectangle& rect) const { return !intersection(rect).empty(); }
    bool empty() const { return m_bottomRight.x <= m_topLeft.x || m_bottomRight.y <= m_topLeft.y; }
    Coord getTopLeft() const { return m_topLeft; }
    Coord getBottomRight() const { return m_bottomRight; }
    void setTopLeft( const Coord& c ) { m_topLeft = c; }
    void setBottomRight( const Coord& c ) { m_bottomRight = c; }
    const Rectangle operator+(const Coord& c) const { return Rectangle(m_topLeft + c, m_bottomRight + c); }
    const Rectangle operator-(const Coord& c) const { return Rectangle(m_topLeft - c, m_bottomRight - c); }
    const Rectangle operator*(int i) const { return Rectangle(m_topLeft * i, m_bottomRight * i); }
    const Rectangle& operator=(const Rectangle& rect) { resizeTo(rect); return *this; }
    Coord project( const Coord& pos ) const;

  class Iterator : public std::iterator<std::forward_iterator_tag, Coord>
  {
    public:
      Iterator(const Rectangle& rect, const Coord& pos) : m_rect(rect), m_pos(pos) {}
      Iterator& operator++()
      {
        if (++m_pos.x == m_rect.m_bottomRight.x)
        {
          m_pos.x = m_rect.m_topLeft.x;
          ++m_pos.y;
        }
        return *this;
      }

      bool operator!=(const Iterator& rhs) const
      {
        return m_pos != rhs.m_pos;
      }

      const Coord& operator*()
      {
        return m_pos;
      }

      const Coord* operator->()
      {
        return &m_pos;
      }

    private:
      const Rectangle& m_rect;
      Coord m_pos;
  };

    Iterator begin() const
    {
      if ( m_topLeft.x == m_bottomRight.x )
      {
        return end();
      }
      return Iterator(*this, m_topLeft);
    }

    Iterator end() const { return Iterator(*this, Coord(m_topLeft.x, m_bottomRight.y)); }

  private:
    Coord m_topLeft, m_bottomRight;
};

#endif