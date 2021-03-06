#include "area.h"
#include "field.h"
#include "userinterface.h"
#include "logger.h"
#include "planner.h"
#include <cassert>
#include <boost/bind.hpp>

Area::Area( Planner& planner, const Coord& c, Serf::Type type )
  : Rectangle(c),
    m_planner( planner),
    m_type(type),
    m_areaManager(0)
{
  m_available.assign(0);
  m_port.assign(0);
  countItems();
}

Area::~Area()
{
  resize(0, 0);
}

void Area::moveTo(Coord c)
{
  Coord size = getBottomRight() - getTopLeft();
  Rectangle world = Field::current()->getInteriorWorldRect();
  world.setBottomRight( world.getBottomRight() - size + Coord(1, 1) );
  c = world.project( c );
  resizeTo( Rectangle(c, c + size) );
}

void Area::resize(int width, int height)
{
  resizeTo( getTopLeft(), getTopLeft() + Coord(width, height) );
}

void Area::resizeTo(Coord c, Coord fix)
{
  if (m_type > Serf::BUILDER)
  {
    c = fix + Coord(2, 2);
    if (c.x >= static_cast<int>(Field::current()->getWidth()) - 1)
      c.x -= 4;
    if (c.y >=static_cast<int>(Field::current()->getHeight()) - 1)
      c.y -= 4;
  }
  c = Field::current()->getWorldRect().project( c );
  resizeTo( Rectangle(std::min(fix.x, c.x), std::min(fix.y, c.y), std::max(fix.x, c.x) + 1, std::max(fix.y, c.y) + 1) );
}

bool Area::checkNewRect( Rectangle& newRect ) const
{
  newRect = newRect.intersection( Field::current()->getInteriorWorldRect() );
  return ( getTopLeft() != newRect.getTopLeft() ||
           getBottomRight() != newRect.getBottomRight() );
}

void Area::resizeTo( const Rectangle& newRect )
{
  Rectangle checkRect( newRect );
  if ( !checkNewRect( checkRect ) )
  {
    return;
  }
  if ( m_type != Serf::BUILDER )
  {
    for (Iterator it = begin(); it != end(); ++it)
    {
      if ( !checkRect.contains(*it) )
      {
        m_planner.setChangeTarget( *it );
      }
    }
  }
  Rectangle::resizeTo( checkRect );
  countItems();
}

void Area::itemChanged( const Coord& c, Item oldItem )
{
  if ( contains( c ) && !m_planner.hasTarget(c) )
  {
    Item newItem = Field::current()->getItem( c );
    --m_available[oldItem];
    ++m_available[newItem];
    if ( m_areaManager )
    {
       m_areaManager->addToAvailable( oldItem, -1 );
       m_areaManager->addToAvailable( newItem, +1 );
    }
  }
}

void Area::targetChanged( const Coord& c )
{
  if ( contains( c ) )
  {
    int diff = m_planner.hasTarget(c) ? -1 : +1;
    Item item = Field::current()->getItem( c );
    m_available[item] += diff;
    if ( m_areaManager )
    {
       m_areaManager->addToAvailable( item, diff );
    }
  }
}

void Area::countItems()
{
  boost::array<int, N_OF_ITEMS> diff = m_available;
  m_available.assign(0);
  for (Iterator it = begin(); it != end(); ++it)
  {
    if ( !m_planner.hasTarget( *it ) )
    {
        m_available[Field::current()->getItem(*it)]++;
    }
  }
  if ( m_areaManager )
  {
    for ( unsigned int i = 0; i < diff.size(); ++i )
    {
      diff[i] = m_available[i] - diff[i];
    }
    m_areaManager->addToAvailable( diff );
  }
}

Coord Area::findNearestItem (const Coord& from, Item load) const
{
  assert( hasAvailable( load ) );
  Coord result;
  int min = -1;
  for (Iterator it = begin(); it != end(); ++it)
  {
    if ( !m_planner.hasTarget( *it ) && Field::current()->getItem(*it) == load )
    {
      int testMin = from.minDistanceTo( *it );
      if (min == -1 || testMin < min)
      {
        min = testMin;
        result = *it;
      }
    }
  }
  assert(min !=- 1);
  return result;
}

void Area::draw(BITMAP* mainscreen, int PICSZ, const Coord& viewOrigin, bool isSelected) const
{
  int color = isSelected ? ( isActive() ? 13 : 12 ) : ( isActive() ? 15 : 161 );
  Rectangle viewRect = (*this - viewOrigin) * PICSZ;
  UserInterface::drawRect(mainscreen, viewRect.getTopLeft().x, viewRect.getTopLeft().y, viewRect.getBottomRight().x, viewRect.getBottomRight().y, color);
  UserInterface::printText(mainscreen,viewRect.getTopLeft().x, viewRect.getTopLeft().y, color,"##");
  UserInterface::printText(mainscreen,viewRect.getBottomRight().x - PICSZ/2, viewRect.getBottomRight().y - PICSZ/2 ,color, "");  
}

void Area::drawInfo( BITMAP* sidescreen, int PICSZ, int /*myPlayerNumber*/ ) const
{
  UserInterface::drawSprite(sidescreen, 48-PICSZ/2, 25, UserInterface::SpriteItem, LOGO, 3-isActive());
//   UserInterface::drawSprite(sidescreen,itempic[LOGO][3-m_active],48-PICSZ/2,25);
  for (int i = 0; i < PORT_END - PORT_START - 1; i++)
  {
    UserInterface::drawSprite(sidescreen,40,40+i*PICSZ,UserInterface::SpriteItem, i + PORT_START + 1, 1);
    UserInterface::drawSprite(sidescreen,0,40+i*PICSZ,UserInterface::SpriteItem, LOGO, 1);
    UserInterface::drawSprite(sidescreen,80,40+i*PICSZ,UserInterface::SpriteItem, LOGO, 0);
    if (m_port[i]>0)
      UserInterface::drawRectFill(sidescreen,56,40+i*PICSZ+3,55+24*m_port[i]/MAXPORT,40+(i+1)*PICSZ-4,30);
    else if (m_port[i]<0)
      UserInterface::drawRectFill(sidescreen,40+24*m_port[i]/MAXPORT,40+i*PICSZ+3,39,40+(i+1)*PICSZ-4,30);
  }
}

Area::Mode Area::areaClick(const Coord& c) const
{
  if (m_type==Serf::BUILDER)
  {
    return NOTHING;
  }
  if (c == getTopLeft())
  {
    return DRAG;
  }
  else if (c + Coord(1, 1) == getBottomRight())
  {
    return RESIZE;
  }
  return NOTHING;
}

void Area::infoClick(const Coord& pix, int PICSZ)
{
  int index = Item((pix.y-40)/PICSZ);
  if (index>=0 && index<PORT_END - PORT_START - 1)
  {
    int portnew;
    if (pix.x>0&&pix.x<PICSZ)
      portnew=m_port[index]-1;
    else if (pix.x>96-PICSZ&&pix.x<96)
      portnew=m_port[index]+1;
    else return;
    if (portnew>=-MAXPORT && portnew<=MAXPORT)
    {
      m_port[index]=portnew;
      for (Iterator it = begin(); it != end(); ++it)
      {
        m_planner.setChangeTarget( *it );
      }
    }
  }
}

bool Area::canBeUsedBy(Serf::Type serfType) const
{
  return m_type != Serf::BUILDER || serfType == Serf::BUILDER;
}

bool Area::hasBuilding() const
{
  for (Iterator it = begin(); it != end(); ++it)
  {
    if (equiv(Field::current()->getItem(*it), DOOR1))
    {
      return true;
    }
  }
  return false;
}

void Area::setAreaManager( AreaManager* areaManager )
{
  assert( !m_areaManager);
  m_areaManager = areaManager;
  m_available.assign(0);
  countItems();
}
