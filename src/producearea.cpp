#include "producearea.h"
#include "userinterface.h"

ProduceArea::ProduceArea( Planner& planner, const Coord& c, Serf::Type type )
  : OccArea( planner, c, type )
{
  for (int i = 0; i < MAXPRODUCE; i++)
    m_produce[i] = Serf::SERF;
}


ProduceArea::~ProduceArea()
{
}

bool ProduceArea::hasProduce()
{
  return m_produce[0] != Serf::SERF;
}

Serf::Type ProduceArea::shiftProduce()
{
  Serf::Type result = m_produce[0];
  for (int i = 0; i < MAXPRODUCE - 1; i++)
  {
    m_produce[i] = m_produce[i + 1];
  }
  m_produce[MAXPRODUCE - 1] = Serf::SERF;
  return result;
}

void ProduceArea::infoClick(const Coord& pix, int PICSZ)
{
  Serf::Type pushed = Serf::Type(((pix.y-80)/PICSZ)*3+(pix.x-(96-3*PICSZ)/2)/PICSZ+1);
  if (pushed>=Serf::BUILDER && pushed<Serf::N_TYPES) // todo fix this comparison
    for (int i=0;i<MAXPRODUCE;i++)
      if (!m_produce[i])
  {
    m_produce[i]=pushed;
    break;
  }
}

void ProduceArea::drawInfo(BITMAP* sidescreen, int PICSZ, int myPlayerNumber) const
{
  int i;
  UserInterface::drawSprite(sidescreen,48-PICSZ/2,25,UserInterface::SpriteItem, LOGO, isActive() ? 2 : 3);
  if (getType() == Serf::TEACHER)
  {
    for (i=0;i<MAXPRODUCE;i++)
    {
      UserInterface::drawRect(sidescreen,48-MAXPRODUCE*(PICSZ+1)/2+i*(PICSZ+1)-1,49,48-MAXPRODUCE*(PICSZ+1)/2+(i+1)*(PICSZ+1)-1,50+PICSZ,13);
      if (m_produce[i])
        UserInterface::drawSprite(sidescreen,48-MAXPRODUCE*(PICSZ+1)/2+i*(PICSZ+1),50,UserInterface::SpriteCr, 10+11*(myPlayerNumber%3), 8*m_produce[i]);
    }
    for (i=1;i<Serf::N_TYPES;i++)
      UserInterface::drawSprite(sidescreen,(96-3*PICSZ)/2+PICSZ*((i-1)%3),80+((i-1)/3)*PICSZ,UserInterface::SpriteCr, 10+11*(myPlayerNumber%3), 8*i);
  }
}




