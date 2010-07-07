#include "occarea.h"
#include "field.h"

OccArea::OccArea( Planner& planner, const Coord& c, Serf::Type type )
  : Area( planner, c, type),
    m_occupant(0)
{
  for ( int i = PORT_START + 1; i < PORT_END; ++i )
  {
    setPort( static_cast<Item>( i ), -MAXPORT );
  }
}

OccArea::~OccArea()
{
  resetOccupant();
}

void OccArea::resizeTo(const Rectangle& newRect)
{
  Rectangle checkRect( newRect );
  if ( !checkNewRect( checkRect ) )
  {
    return;
  }
  resetOccupant();
  Area::resizeTo( checkRect );
}

void OccArea::resetOccupant()
{
  if (m_occupant)
  {
    m_occupant->setOccupies(0);
    m_occupant = 0;
  }
}

bool OccArea::hasFreeFloor() const
{
  for (Iterator it = begin(); it != end(); ++it)
  {
    if ( field.getItem(*it) != FLOOR && field.getItem(*it) != CLASSROOMFLOOR )
    {
      return false;
    }
  }
  return true;
}

bool OccArea::isFreeBuilding() const
{
  if ( m_occupant || !hasFreeFloor() )
  {
    return false;
  }
  for (int a = getTopLeft().x - 1; a < getBottomRight().x + 1; a++)
  {
    if (!equiv(field.getItem(Coord(a, getTopLeft().y - 1)), WALL))
      return false;
    if (!equiv(field.getItem(Coord(a, getBottomRight().y)), WALL))
      return false;
  }
  for (int b = getTopLeft().y;b < getBottomRight().y; b++)
  {
    if (!equiv(field.getItem(Coord(getTopLeft().x - 1, b)), WALL))
      return false;
    if (!equiv(field.getItem(Coord(getBottomRight().x, b)), WALL))
      return false;
  }
  return true;
}
