#ifndef FIELD_H
#define FIELD_H

#include "pathfinder.h"
#include "item.h"
#include "coord.h"
#include "fieldmap.h"

#include <vector>
#include <boost/signals.hpp>

class Field
{
public:
  Field();
  ~Field();
  Item getItem(const Coord& c) const { return m_items(c); }
  void setItem(const Coord& c, Item i);
  bool isPassable(const Coord& c) const { return m_t2pass[m_items(c)] != 0; }
  PathFinder& getPathFinder() { return m_pathFinder; }

  /**
   * Time to go one step from coordinate c in direction d
   * @param c from
   * @param d direction
   * @return time
   */
  unsigned int passingTime( const Coord& c, Direction d ) const { return ( d & 1 ? 3 : 2 ) * m_t2pass[m_items(c.next(d))]; }

  /**
   * Turns the floor into a safe floor if it is finished
   * @param c coordinate on the floor
   * @return true if floor is safe
   */
  bool checkFloor(const Coord& c);

  boost::signals::connection addItemChangedCallback( const boost::signal<void (const Coord&)>::slot_type& slot )
  {
    return m_itemChangedSignal.connect( slot );
  }

private:
  bool checkIfFinishedFloor(const Coord& c);
  void floodReplace( const Coord& c, Item old, Item replace );
  
  FieldMap<Item> m_items;
  PathFinder m_pathFinder;
  std::vector<int> m_t2pass;
  boost::signal<void (const Coord&)> m_itemChangedSignal;
};

extern Field field;

#endif
