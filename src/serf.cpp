#include "serf.h"
#include "field.h"
#include "global.h"
#include "occarea.h"
#include "areamanager.h"
#include "producearea.h"
#include "userinterface.h"
#include "plan.h"
#include "player.h"
#include <cassert>

FieldMap<Serf*> Serf::s_sf;
CoordSet<MAPWIDTH, MAPHEIGHT> Serf::s_walkOn;

Serf::Serf(Type type, Player& player, const Coord& pos)
  : m_type(type),
    m_pos(pos),
    m_jobResult( R_SUCCESS ),
    m_player(player),
    m_becomesType(VOID_TYPE)
{
  s_sf( m_pos ) = this;
  m_status = READY;
  m_dir  =  1;
  m_job = SLEEP;
  m_pass = 0;
  m_passtot = 1;
  m_load = VOID;
  m_occupies = 0;
}

Serf::~Serf()
{
  if (m_occupies)
  {
    m_occupies->setOccupant(0);
  }
}

void Serf::setOccupies(OccArea* occupies)
{ 
  m_occupies = occupies;
  if (!m_occupies)
  {
    m_plan.reset();
  }
}

int Serf::drawOffsetX(int tick) const
{
  if (m_job == MOVE) 
    return Coord::dx[m_dir]*((((m_pass-1) < m_passtot / 2)?0:(-16))+(tick+16*(m_pass-1))/m_passtot);
  else if (m_type == WOODCUTTER && (m_job == ACT||m_job == ACTPREPARE))
    return -5;
  else
    return 0;
}

int Serf::drawOffsetY(int tick) const
{
  if (m_job == MOVE) 
    return Coord::dy[m_dir]*((((m_pass-1)<m_passtot / 2)?0:(-16))+(tick+16*(m_pass-1))/m_passtot);
  else 
    return 0;
}

void Serf::draw(BITMAP* bmp, int x, int y, int tick) const
{
  int picx, picy;
  if ( m_job == SLEEP)
    picx = 4;
  else if (m_job == MOVE||m_job == TAKE)
    if (m_dir & 1)    //diagonal
      picx = (((m_dir+1)%8)>>2)*2;
    else
      picx = m_dir>>1;
  else
    picx = 10;
  if (m_load && picx != 10)
      picx += 5;
  picx += N_OF_PIC_PER_KIND*(m_player.getNumber())%3;
  if (m_job == ACT)
    picy = 8*m_type;
  else if (m_type == GRINDER && m_job == ACTPREPARE)
    picy = (((tick+16*m_pass)/2)&7)+8*m_type;
  else if (m_type == FARMER && m_job == ACTPREPARE)
    picy = (((tick+16*m_pass)/4)&7)+8*m_type;
  else if (m_type == WOMAN && m_job == ACTPREPARE)
    picy = (((tick+16*m_pass)/(2*m_passtot))&7)+8*m_type;
  else if (m_job == ACTPREPARE)
    picy = (((tick+16*m_pass)/8)&7)+8*m_type;
  else
    picy = (((tick+16*m_pass)/m_passtot)&7)+8*m_type;
  if (picx%5 != 3&&m_job != SLEEP)
    UserInterface::drawSprite(bmp, x, y, UserInterface::SpriteCr, picx, picy);
  if (m_load && (m_type == SERF || m_type == BUILDER))
  {
    if (m_job != SLEEP)
      UserInterface::drawSprite(bmp, x, y+(((picy+1)/2)&1), UserInterface::SpriteItem, m_load, 2+(picx%11)%5 );
    else
      UserInterface::drawSprite(bmp, x, y+9, UserInterface::SpriteItem, m_load, 4);
  }
  if (picx%5 == 3||m_job == SLEEP)
    UserInterface::drawSprite(bmp, x, y, UserInterface::SpriteCr, picx, picy);
}

void Serf::makePlan()
{
  assert(!m_plan.get());
  Planner& planner = m_player.getPlanner();
  if (!m_occupies && (m_type == STONEMASON || m_type == WOODCUTTER ||
                      m_type == GRINDER    || m_type == WOMAN      ||
                      m_type == TEACHER    || m_type == FARMER))
  {
    std::vector<OccArea*>& areas = planner.getAreaManager().getOccAreas();
    for (std::vector<OccArea*>::iterator it = areas.begin(); it != areas.end(); ++it)
    {
      if ( (*it)->getType() == m_type && (*it)->isFreeBuilding() )
      {
        m_occupies = *it;
        m_occupies->setOccupant( this );
        break;
      }
    }
  }
  m_plan = planner.makeBestPlan(*this);
}

// sets m_wantsJob
void Serf::chooseJob()
{
  if ( m_status != READY )
  {
    return;
  }
  if ( m_becomesType != VOID_TYPE )
  {
    m_type = m_becomesType;
    m_becomesType = VOID_TYPE;
  }
  m_status = JOBCHOSEN;
  if (m_job == ACTPREPARE)
  {
    m_job = ACT;
    return;
  }
  if ( m_plan.get() )
  {
    if ( m_jobResult == R_FAILED ||
         ( m_jobResult == R_SUCCESS && !m_plan->gotoNextJob() ) )
    {
      m_plan.reset();
    }
  }
  if (!m_plan.get())
  {
    makePlan();
  }
  if (m_plan.get())
  {
    m_plan->getJobAndDir(&m_job, &m_dir );
  }
  else
  {
    m_job = MOVE;
    m_dir = randomNum(8);
  }
}

// determine how long job will take, if it is a move, check if it is possible, otherwise sleep
void Serf::checkJob()
{
  if (m_status != JOBCHOSEN)
  {
    return;
  }
  m_status = JOBCHECKING; // being checked now
  m_pass = 0;
  if ( m_jobResult == R_FAILED )
  {
    m_job = MOVE;
    m_dir = randomNum(8);
  }
  m_jobResult = R_SUCCESS; // in principle
  if (m_job != MOVE)
  {
    if (m_job == SLEEP || m_job == TAKE || m_job == ACT)
      m_passtot = 1;
    else if ( m_job == ACTPREPARE )
    {
      if ( m_type == SERF )
        m_passtot = 100;
      else if ( m_type == WOODCUTTER )
        m_passtot = 200;
      else if ( m_type == STONEMASON )
        m_passtot = 200;
      else 
        m_passtot = 50;
    }
    else
      m_passtot = 3;
    m_status = JOBCHECKED;
    return;
  }
  Coord nextPos = m_pos.next(m_dir);
  if ( !field.isPassable(nextPos) )
  {
    m_jobResult = R_FAILED;
    m_job = SLEEP;
    m_passtot = 1;
    m_status = JOBCHECKED;
    return;
  }
  if ( s_walkOn.has( nextPos ) )
  {
    m_jobResult = R_DELAYED;
    m_job = SLEEP;
    m_passtot = 1;
    m_status = JOBCHECKED;
    return;
  }
  m_passtot = field.passingTime( m_pos, m_dir );
  if ( Serf *other = s_sf(nextPos ) )
  {
    if ( other->m_status == JOBCHECKING ) // other is waiting for me
    {
      m_status = JOBCHECKED;
      m_passtot = other->maxpasstime();
      if ( other->m_pos.next(other->m_dir) == m_pos &&
           ( field.getItem(m_pos) != ROAD ||
             field.getItem(other->m_pos) != ROAD ) )
      {
        m_passtot *= 2;            //passing costs more time
      }
    }
    else
    {
      if ( other->m_status == JOBCHOSEN )
      {
        m_status = JOBCHECKING;
        other->checkJob();       // find out what other wants first
        assert(other->m_status == JOBCHECKED);
        m_status = JOBCHECKED;   // will check now
      }
      if ( other->m_job != MOVE ||  // other is not moving
            s_walkOn.has( nextPos ) ||  // somebody else will walk on nextPos
            other->m_pass > other->m_passtot / 2 ) // other is arriving on nextPos
      {
        m_jobResult = R_DELAYED;
        m_job = SLEEP;
        m_passtot = 1;
        m_status = JOBCHECKED;
        return;
      }
      m_passtot = std::max(m_passtot, other->m_passtot-2*other->m_pass);
    }
  }
  if ( m_job == MOVE )
  {
    s_walkOn.add( nextPos );  //will walk there for sure
  }
  m_status = JOBCHECKED;
}

void Serf::doJob()
{
  if ( m_pass == m_passtot / 2)
  {
    executeJob();
  }
  ++m_pass;
  if (m_pass >= m_passtot)
  {
    m_status = READY;
  }
}

void Serf::executeJob()
{
  switch(m_job)
  {
    case MOVE:
      if(s_sf(m_pos) == this)
      {
        s_sf(m_pos) = 0;
      }
      m_pos = m_pos.next(m_dir);
      s_sf(m_pos) = this;
      s_walkOn.remove( m_pos );
    break;
    case TAKE:
      if (m_load && field.getItem(m_pos) == FLOOR)
      {
        field.setItem(m_pos, m_load);
        m_load = VOID;
      }
      else if ( !m_load && field.getItem(m_pos) )
      {
        m_load = field.getItem(m_pos);
        assert(m_load > PORT_START && m_load < PORT_END);
        field.setItem( m_pos, FLOOR );
      }
    break;
    case ACT:
      switch (m_type)
      {
        case SERF:
          // do nothing
        break;
        case WOMAN:
          if ( !s_sf( m_pos.next() ) )
          {
            getPlayer().createSerf( SERF, m_pos.next() );
            m_load = VOID;
          }
          else
          {
            m_jobResult = R_FAILED;
          }
        break;
        case TEACHER:
        {
          Serf* student = s_sf( m_pos.next() );
          if ( student &&
               student->m_type == SERF &&
               ( student->m_job == ACT || student->m_job == ACTPREPARE ) &&
               m_load == FLOUR &&
               m_occupies &&
               m_occupies->isActive() )
          {
            m_load = VOID;
            student->m_becomesType = static_cast<ProduceArea*>(m_occupies)->shiftProduce();
          }
          else
          {
            m_jobResult = R_FAILED;
          }
        }
        break;
        case GRINDER:
          if ( m_occupies && m_occupies->hasFreeFloor() )
          {
            field.setItem( m_pos.next(), FLOUR );
            m_load = VOID;
          }
          else
          {
            m_jobResult = R_FAILED;
          }
        break;
        case FARMER:
          if ( !m_load && field.getItem(m_pos) == GRASS )
          {
            m_load = GRAIN;
            field.setItem(m_pos, VOID);
          }
          else if ( m_occupies && m_occupies->hasFreeFloor() )
          {
            field.setItem( m_pos.next(), GRAIN );
            m_load = VOID;
          }
          else
          {
            m_jobResult = R_FAILED;
          }
        break;
        case STONEMASON:
          if ( !m_load )
          {
            switch ( field.getItem(m_pos) )
            {
              case ROCK1:
                field.setItem(m_pos, VOID);
                m_load = ROCK1;
                break;
              case ROCK2:
                field.setItem(m_pos, ROCK1);
                m_load = ROCK1;
                break;
              case ROCK3:
                field.setItem(m_pos, ROCK2);
                m_load = ROCK1;
                break;
              default:
                break;
            }
          }
          else if ( m_occupies && m_occupies->hasFreeFloor() )
          {
            for (int i = 0; i < 8; i++)
            {
              field.setItem(m_pos.next(i), STONE);
            }
            m_load = VOID;
          }
          else
          {
            m_jobResult = R_FAILED;
          }
        break;
        case WOODCUTTER:
          if ( !m_load && field.getItem(m_pos) == TREE )
          {
            m_load = TREE;
            field.setItem(m_pos, VOID);
          }
          else if ( m_occupies && m_occupies->hasFreeFloor() )
          {
            for (int i = 0; i < 8; i++)
            {
              field.setItem(m_pos.next(i), BOARD);
            }
            m_load = VOID;
          }
          else
          {
            m_jobResult = R_FAILED;
          }
        break;
        default:
        break;
      }
    break;
    case BUILDWALL:
      if (m_load == STONE && equiv(field.getItem(m_pos), VOID))
      {
        field.setItem(m_pos, WALL);
        m_load = VOID;
        for(int i = 0; i < 8; i++)
        {
          field.checkFloor(m_pos.next(i));
        }
      }
    break;
    case BUILDFLOOR:
      if (m_load == BOARD && equiv(field.getItem(m_pos), VOID))
      {
        field.setItem(m_pos, UNSAFEFLOOR);
        m_load = VOID;
        field.checkFloor(m_pos);
      }
    break;
    case BUILDDOOR:
      if (m_load == STONE && equiv(field.getItem(m_pos), VOID))
      {
        field.setItem(m_pos, DOOR1);
        m_load = VOID;
      }
      else if (m_load == STONE && field.getItem(m_pos) == DOOR1)
      {
        field.setItem(m_pos, DOOR2);
        m_load = VOID;
      }
      else if (m_load == BOARD && field.getItem(m_pos) == DOOR2)
      {
        field.setItem(m_pos, DOOR);
        m_load = VOID;
      }
      for(int i = 0; i < 8; i++)
      {
        field.checkFloor(m_pos.next(i));
      }
    break;
    case BUILDROAD:
      if (m_load == STONE && equiv(field.getItem(m_pos), VOID))
      {
        field.setItem(m_pos, ROAD);
        m_load = VOID;
      }
    break;
    default:
    break;
  }
}

int Serf::maxpasstime()
{
  if (m_status == JOBCHECKED)
  {
    return m_passtot;
  }
  Serf* other  = s_sf(m_pos.next(m_dir));
  assert(other);
  return std::max(m_passtot, other->maxpasstime());
}

const Path* Serf::getPath() const
{
  if (m_plan.get())
  {
    return &m_plan->getPath();
  }
  return 0;
}