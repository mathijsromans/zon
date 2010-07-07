#include "areamanager.h"
#include "occarea.h"
#include "field.h"

AreaManager::AreaManager()
{
}

AreaManager::~AreaManager()
{
}

Area* AreaManager::addNewArea(Area* area)
{
  m_allAreas.push_back( area );
  if ( OccArea* occArea = dynamic_cast<OccArea*>(area) )
  {
    m_occAreas.push_back( occArea );
  }
  return area;
}

void AreaManager::removeArea(Area* area)
{
  for (boost::ptr_list<Area>::iterator it = m_allAreas.begin(); it !=  m_allAreas.end(); ++it)
  {
    if ( &*it == area )
    {
      m_allAreas.erase( it );
      break;
    }
  }
  std::vector<OccArea*>::iterator it = std::find( m_occAreas.begin(), m_occAreas.end(), area );
  if ( it != m_occAreas.end() )
  {
    m_occAreas.erase( it );
  }
}

void AreaManager::removeBuildAreas(const Coord& c)
{
  for (boost::ptr_list<Area>::iterator it = m_allAreas.begin(); it != m_allAreas.end();)
  {
    if ( it->getType() == Serf::BUILDER && it->contains(c) && !it->hasBuilding() )
    {
      it = m_allAreas.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void AreaManager::doBuildMerge(Area* area)
{
  assert(area->getType() == Serf::BUILDER);
  
  Rectangle fieldRect(0, 0, MAPWIDTH, MAPHEIGHT);
  Coord topLeft = area->getTopLeft() - Coord(2, 2);
  Coord bottomRight = area->getBottomRight() + Coord(2, 2);
  Rectangle surrounding(topLeft, bottomRight);
  surrounding = fieldRect.intersection( surrounding );
  boost::ptr_list<Area>::iterator it;
  do
  {
    Rectangle rect;
    for ( it = m_allAreas.begin(); it != m_allAreas.end(); ++it )
    {
      Area* other = &*it;
      if ( other != area && other->getType() == Serf::BUILDER )
      {
        if ( !surrounding.intersection(*other).empty() )
        {
          int n = 0;
          rect = area->unionWith(*other);
          for ( Rectangle::Iterator c = rect.begin(); c != rect.end(); ++c)
          {
            Item item = field.getItem(*c);
            if (item > BUILD_START && item < BUILD_END)
            {
              n++;
            }
          }
          if ((float) n/rect.area() > 0.75)  //do merge
          {
            break;
          }
        }
      }
    }
    if ( it != m_allAreas.end() )
    {
      area->resizeTo( rect );
      m_allAreas.erase(it);
    }
  }
  while ( it != m_allAreas.end() );
}



