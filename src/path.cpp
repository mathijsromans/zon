#include "path.h"

#include <algorithm>
#include <cassert>

bool Path::done() const
{
  return m_index == m_path.size();
}

void Path::next()
{
  ++m_index;
}

void Path::append(Direction d)
{
  m_path.push_back(d);
  m_finish = m_finish.next(d);
  m_boundingBox.add(m_finish);
}

void Path::append(const Path& newPath)
{
  if ( !newPath.m_valid )
  {
    m_valid = false;
  }
  else
  {
    assert(m_finish == newPath.m_start);
    m_path.insert( m_path.end(), newPath.m_path.begin(), newPath.m_path.end() );
    m_finish = newPath.m_finish;
    m_boundingBox = m_boundingBox.unionWith( newPath.m_boundingBox );
  }
}

void Path::reverse()
{
  std::swap(m_start, m_finish);
  for ( std::size_t i = 0; i < m_path.size() / 2; ++i )
  {
    Direction tmp = m_path[i];
    m_path[i] = ( m_path[m_path.size() - 1 - i] + 4 ) % 8;
    m_path[m_path.size() - 1 - i] = ( tmp + 4 ) % 8;
  }
  if ( m_path.size() & 1 ) // odd
  {
    m_path[ m_path.size() / 2 ] = ( m_path[ m_path.size() / 2 ] + 4 ) % 8;
  }
}
