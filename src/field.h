#ifndef FIELD_H
#define FIELD_H

#include "pathfinder.h"
#include "item.h"
#include "coord.h"
#include "coordset.h"
#include "fieldmap.h"
#include "rectangle.h"
#include <vector>
#include <boost/signals.hpp>
class Serf;

class Field
{
public:
  Field( unsigned int width, unsigned int height );
  ~Field();
  static Field* current() { return s_current.get(); }
  static void setCurrent( std::auto_ptr<Field> field ) { s_current = field; }
  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  CoordSet& getWalkOn() { return m_walkOn; }
  Item getItem(const Coord& c) const { return m_items(c); }
  void setItem(const Coord& c, Item i);
  Serf* getSerf(const Coord& c) const { return m_serfs(c); }
  void setSerf(const Coord& c, Serf* s) { m_serfs(c) = s; }
  bool isPassable(const Coord& c) const { return m_t2pass[m_items(c)] != 0; }
  PathFinder& getPathFinder() { return m_pathFinder; }
  Rectangle getWorldRect() const { return Rectangle( Coord(0, 0), Coord(m_width, m_height) ); }
  Rectangle getInteriorWorldRect() const { return Rectangle( Coord(1, 1), Coord(m_width - 1, m_height - 1) ); }


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

  Serf* getSerf(const Coord& c) { return m_serfs(c); }

  boost::signals::connection addItemChangedCallback( const boost::signal<void (const Coord&, Item)>::slot_type& slot )
  {
    return m_itemChangedSignal.connect( slot );
  }

private:
  bool checkIfFinishedFloor(const Coord& c);
  void floodReplace( const Coord& c, Item old, Item replace );
  static std::auto_ptr<Field> s_current;
  unsigned int m_width, m_height;
  CoordSet m_walkOn;
  FieldMap<Item> m_items;
  FieldMap<Serf*> m_serfs;
  PathFinder m_pathFinder;
  std::vector<int> m_t2pass;
  boost::signal<void (const Coord&, Item)> m_itemChangedSignal;
};

#endif
