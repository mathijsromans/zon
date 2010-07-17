#include "occarea.h"
#include "field.h"
#include "userinterface.h"

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

void OccArea::draw(BITMAP* mainscreen, int PICSZ, const Coord& viewOrigin, bool isSelected) const
{
  int color = isSelected ? ( m_occupant ? 13 : 12 ) : ( m_occupant ? ( isActive() ? 15 : 16 ) : 161 );
  Rectangle viewRect = (*this - viewOrigin) * PICSZ;
  UserInterface::drawRect(mainscreen, viewRect.getTopLeft().x, viewRect.getTopLeft().y, viewRect.getBottomRight().x, viewRect.getBottomRight().y, color);
  UserInterface::drawRect(mainscreen,viewRect.getTopLeft().x + 2, viewRect.getTopLeft().y + 2, viewRect.getBottomRight().x - 2, viewRect.getBottomRight().y - 2,color);
  UserInterface::printText(mainscreen,viewRect.getTopLeft().x, viewRect.getTopLeft().y, color,"##");
  UserInterface::printText(mainscreen,viewRect.getBottomRight().x - PICSZ/2, viewRect.getBottomRight().y - PICSZ/2 ,color, "");  
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
