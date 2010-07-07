#include "pathfinder.h"

#include "field.h"
#include "path.h"
#include "coordset.h"

#include <queue>
#include <algorithm>
#include <cassert>

namespace
{

struct DistNode
{
  DistNode(const Coord& setCoord, unsigned int setFScore) : coord(setCoord), f_score(setFScore) {}
  bool operator<(const DistNode& rhs) const { return f_score > rhs.f_score; }
  Coord coord;
  unsigned int f_score;
};

struct Zero
{
  unsigned int operator()(const Coord& /*c*/) const { return 0; }
};

class DistanceTo
{
  public:
    DistanceTo(const Coord& target) : m_target(target) {}
    unsigned int operator()(const Coord& c) const  { return m_target.minDistanceTo(c); }
  private:
    Coord m_target;
};

class ReachedPos
{
  public:
    ReachedPos(const Coord& pos) : m_pos(pos) {}
    bool operator()(const Coord& c) const { return c == m_pos; }
  private:
    Coord m_pos;
};

class ReachedItem
{
  public:
    ReachedItem(Item item, const Field& field, const CoordSet<MAPWIDTH, MAPHEIGHT>& doNotUse ) : m_item(item), m_field(field), m_doNotUse(doNotUse) {}
    bool operator()(const Coord& c) const { return !m_doNotUse.has(c) && m_field.getItem(c) == m_item; }
  private:
    Item m_item;
    const Field& m_field;
    const CoordSet<MAPWIDTH, MAPHEIGHT>& m_doNotUse;
};

class ReachedAnyItemOf
{
  public:
    ReachedAnyItemOf( const std::vector<Item>& items, const Field& field, const CoordSet<MAPWIDTH, MAPHEIGHT>& doNotUse ) : m_items(items), m_field(field), m_doNotUse(doNotUse) {}
    bool operator()(const Coord& c) const
    {
      return !m_doNotUse.has(c) && std::find( m_items.begin(), m_items.end(), m_field.getItem(c) ) != m_items.end();
    }
  private:
    const std::vector<Item>& m_items;
    const Field& m_field;
    const CoordSet<MAPWIDTH, MAPHEIGHT>& m_doNotUse;
};

}

Path PathFinder::findPath(const Coord& start, const Coord& end)
{
  Coord mapEnd;
  if ( makeMap( start, DistanceTo(end), ReachedPos(end), &mapEnd ) )
  {
    return getPath(start, end);
  }
  return Path();
}

Path PathFinder::findPath(const Coord& start, Item item, const CoordSet<MAPWIDTH, MAPHEIGHT>& doNotUse, Coord* end)
{
  assert(end);
  if ( makeMap(start, Zero(), ReachedItem(item, m_field, doNotUse), end) )
  {
    return getPath(start, *end);
  }
  return Path();
}

Path PathFinder::findPath(const Coord& start, const std::vector<Item>& items, const CoordSet<MAPWIDTH, MAPHEIGHT>& doNotUse, Coord* end)
{
  if ( !items.empty() )
  {
    assert(end);
    if ( items.size() == 1 )
    {
      return findPath( start, items.front(), doNotUse, end );
    }
    if ( makeMap(start, Zero(), ReachedAnyItemOf(items, m_field, doNotUse), end) )
    {
      return getPath(start, *end);
    }
  }
  return Path();
}

Path PathFinder::getPath(const Coord& start, const Coord& end) const
{
  Path newPath(end);
  Direction allDirections[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  unsigned int currentGScore = g_score(end);
  while( newPath.getFinish() != start )
  {
    std::random_shuffle( allDirections, allDirections + 8 );
    for (unsigned i = 0;; ++i)
    {
      assert(i < 8);
      Direction d = allDirections[i];
      Coord next = newPath.getFinish().next(d);
      if ( m_closedSet.has(next) || m_openSet.has(next) )
      {
        unsigned int newG = currentGScore - m_field.passingTime( next, (d + 4) % 8 );
        if (newG == g_score(next))
        {
          newPath.append(d);
          currentGScore = newG;
          break;
        }
      }
    }
  }
  newPath.reverse();
  return newPath;
}

template<typename Heuristic, typename EndCondition>
bool PathFinder::makeMap(const Coord& start, Heuristic heur, EndCondition endCondition, Coord* end)
{
  m_openSet.clear();
  m_closedSet.clear();
  std::priority_queue<DistNode> openQueue;
  g_score(start) = 0;
  h_score(start) = heur(start);

  openQueue.push( DistNode( start, h_score(start) ) );
  m_openSet.add(start);

  while ( !openQueue.empty() )
  {
    DistNode dn = openQueue.top();
    Coord c = dn.coord;
    openQueue.pop();
    if (g_score(c) + h_score(c) == dn.f_score)
    {
      // this entry in the queue is valid
      assert(!m_closedSet.has(c));
      if (endCondition(c))
      {
        *end = c;
        return true;
      }
      m_openSet.remove(c); 
      m_closedSet.add(c);
      unsigned int currentGScore = g_score(c);
      for (int dir = 0; dir < 8; ++dir)
      {
        Coord next = c.next(dir);
        if (m_closedSet.has(next) || !m_field.isPassable(next))
        {
          continue;
        }
        unsigned int newG = currentGScore + m_field.passingTime(c, dir);
        bool newNode = false;
        if (!m_openSet.has(next))
        {
          h_score(next) = heur(next);
          m_openSet.add(next);
          newNode = true;
        }
        if (newNode || newG < g_score(next))
        {
          g_score(next) = newG;
          openQueue.push(DistNode(next, newG + h_score(next))); // may occur more than once, because we cannot remove the old one
        }
      }
    }
  }
  return false;
}

