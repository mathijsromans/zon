#include "zon.h"
#include "global.h"
#include "field.h"
#include "player.h"
#include <stdio.h>
#include <algorithm>
#include <time.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <allegro.h>


#define N_OF_SERF_START 30

Zon zon;

static void timecounter(void) 
{
  ++Global::timer;
}
END_OF_FUNCTION(ticker);

Zon::Zon() :
  m_victoryAchieved(false)
{
}

Zon::~Zon()
{
}

void Zon::init()
{
  m_players.push_back( new Player(0) );
  m_players.push_back( new Player(1) );
  m_me = &m_players.front();
  m_userInterface.reset( new UserInterface(*m_me, m_players) );
  m_userInterface->init();
  install_int_ex(timecounter, BPS_TO_TIMER(FPS));
  m_speed = 35;
  m_seedrand = 0;
  srand(m_seedrand);
  for (unsigned int i = 0; i < m_players.size(); ++i)
  {
    int victoryPoints = 0;
    Rectangle world = field.getInteriorWorldRect();
    for ( Rectangle::Iterator p = world.begin(); p != world.end(); ++p )
    {
      if ( field.getItem( *p ) == SPECIALFLOOR_START + 1 + static_cast<int>(i) )
      {
        ++victoryPoints;
      }
    }
    m_players[i].setVictoryPointsNeeded( victoryPoints );
    Coord startPosition = Coord( 50 + (i & 1) * 100, 50 + ( ( (i + 1) / 2) & 1) * 100);
    for ( int j = 0; j < N_OF_SERF_START; j++ )
    {
      Coord pos;
      do
      {
        pos = startPosition + Coord(randomNum(50), randomNum(50));
      } while (Serf::getSerf(pos) || !field.isPassable(pos));
      Serf::Type type = Serf::Type( j % Serf::N_TYPES );
      if (j < N_OF_SERF_START / 5)
      {
        type = Serf::BUILDER;
      }
      else if (j < N_OF_SERF_START / 2)
      {
        type = Serf::SERF;
      }
      m_players[i].createSerf( type, pos);
    }
    if ( &m_players[i] == m_me )
    {
      m_userInterface->setViewOrigin( startPosition );
    }
    else
    {
      m_players[i].getPlanner().initAI();
    }
  }
  show_mouse(screen);
}

void Zon::setSpeed(int speed)
{
  m_speed = std::max( speed, 0 );
}

void Zon::speedUp()
{
  setSpeed( std::min( m_speed - 1, int(m_speed / 1.2) ) );
}

void Zon::speedDown()
{
  setSpeed( std::max( m_speed + 1, int(m_speed * 1.2) ) );
}

void Zon::quit()
{
  allegro_exit();
  exit(0);
}

void Zon::mainLoop()
{
  while (!key[KEY_ESC])
  {
    m_tick = 0;
    m_now = Global::timer;
    for (boost::ptr_vector<Player>::iterator pl = m_players.begin(); pl != m_players.end(); ++pl)
    {
      pl->chooseJobs();
    }
    for (boost::ptr_vector<Player>::iterator pl = m_players.begin(); pl != m_players.end(); ++pl)
    {
      pl->checkJobs();
    }
    for (boost::ptr_vector<Player>::iterator pl = m_players.begin(); pl != m_players.end(); ++pl)
    {
      pl->doJobs();
    }
    m_userInterface->userInput(this);
    m_userInterface->drawScreen(Global::turn, m_tick);
    for (m_tick=0;m_tick<16;m_tick++)
    {
      while (Global::timer < m_now+((m_tick+1)*m_speed)/50)
      {
        m_userInterface->userInput(this);
        m_userInterface->drawScreen(Global::turn, m_tick);                 //  *  MAIN  *
      }
    }
    if ( !m_victoryAchieved )
    {
      for (boost::ptr_vector<Player>::iterator pl = m_players.begin(); pl != m_players.end(); ++pl)
      {
        if ( pl->getVictoryPointsNeeded() <= 0 )
        {
          m_userInterface->victory( *pl );
          m_victoryAchieved = true;
          break;
        }
      }
    }
    ++Global::turn;
  }
  quit();
}



