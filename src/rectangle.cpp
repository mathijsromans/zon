#include "rectangle.h"
#include <algorithm>

using std::max;
using std::min;

void Rectangle::add(const Coord& c)
{
  resizeTo( unionWith( Rectangle( c ) ) );
}

void Rectangle::resizeTo(const Rectangle& rect)
{
  m_topLeft = rect.m_topLeft;
  m_bottomRight = rect.m_bottomRight;
}

Rectangle Rectangle::intersection(const Rectangle& other) const
{
  Coord topLeft( max( m_topLeft.x, other.m_topLeft.x ),
                 max( m_topLeft.y, other.m_topLeft.y ) );
  Coord bottomRight( min( m_bottomRight.x, other.m_bottomRight.x),
                     min( m_bottomRight.y, other.m_bottomRight.y) );
  if ( bottomRight.x <= topLeft.x || bottomRight.y <= topLeft.y )
  {
    return Rectangle(0, 0, 0, 0);
  }
  return Rectangle( topLeft, bottomRight );
}

Coord Rectangle::project( const Coord& pos ) const
{
  Coord c = pos;
  if (c.x < m_topLeft.x) c.x = m_topLeft.x;
  if (c.y < m_topLeft.y) c.y = m_topLeft.y;
  if (c.x >= m_bottomRight.x) c.x = m_bottomRight.x - 1;
  if (c.y >= m_bottomRight.y) c.y = m_bottomRight.y - 1;
  return c;
}

Rectangle Rectangle::unionWith(const Rectangle& other) const
{
  if ( empty() )
  {
    return other;
  }
  if ( other.empty() )
  {
    return *this;
  }
  Coord topLeft( min( m_topLeft.x, other.m_topLeft.x),
                 min( m_topLeft.y, other.m_topLeft.y) );
  Coord bottomRight( max( m_bottomRight.x, other.m_bottomRight.x ),
                     max( m_bottomRight.y, other.m_bottomRight.y ) );
  if ( bottomRight.x <= topLeft.x || bottomRight.y <= topLeft.y )
  {
    return Rectangle(0, 0, 0, 0);
  }
  return Rectangle( topLeft, bottomRight );
}


