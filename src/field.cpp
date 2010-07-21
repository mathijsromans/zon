#include "field.h"
#include "pathfinder.h"
#include <cassert>

Field field;

Field::Field()
  : m_pathFinder(*this)
{
  m_t2pass.resize(N_OF_ITEMS, 2);
  m_t2pass[RIVER]=0;
  m_t2pass[WALL]=0;
  m_t2pass[ROAD]=1;
  m_t2pass[SUPERROCK]=0;
  m_t2pass[VOID]=2;
  m_t2pass[GRASS]=2;
  m_t2pass[ROCK1]=8;
  m_t2pass[ROCK2]=6;
  m_t2pass[ROCK3]=4;
  m_t2pass[TREE]=4;
}

Field::~Field()
{
}

void Field::setItem(const Coord& c, Item i)
{
  Item oldItem = m_items(c);
  m_items(c) = i;
  m_itemChangedSignal(c, oldItem);
}

bool Field::checkFloor(const Coord& c)
{
  bool result = checkIfFinishedFloor( c );
  floodReplace( c, UNSAFEFLOORCHECK, result ? FLOOR : UNSAFEFLOOR );
  return result;
}

bool Field::checkIfFinishedFloor( const Coord& c )
{
  Item item = getItem( c );
  if ( item == UNSAFEFLOORCHECK || equiv(item, WALL) )
  {
    return true;
  }
  if ( item != UNSAFEFLOOR )
  {
    return false;
  }
  setItem(c, UNSAFEFLOORCHECK);
  for (int d = 0; d < 8; d++)
  {
    if ( !checkIfFinishedFloor( c.next(d) ) )
    {
      return false;
    }
  }
  return true;
}

void Field::floodReplace( const Coord& c, Item old, Item replace )
{
  if ( getItem( c ) == old )
  {
    setItem( c, replace );
    for (int d = 0; d < 8; d++)
    {
      floodReplace( c.next(d), old, replace );
    }
  }
}

