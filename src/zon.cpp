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


#define N_OF_SERF_START 60

Zon zon;

static void timecounter(void) 
{
  timer++;
}
END_OF_FUNCTION(ticker);

Zon::Zon()
{
}

Zon::~Zon()
{
}

void Zon::init()
{
  m_players.push_back( new Player(0) );
  m_me = &m_players.back();
  int playerNumber = m_me->getNumber();
  Coord startPosition = Coord( 50+((playerNumber%3)&1)*100, 50+(((playerNumber%3)/2)&1)*100);
  m_userInterface.reset( new UserInterface(*m_me) );
  m_userInterface->init( startPosition );
  install_int_ex(timecounter, BPS_TO_TIMER(FPS));
  initsetvariables();
  for (boost::ptr_vector<Player>::iterator it = m_players.begin(); it != m_players.end(); ++it)
  {
    if (&*it == m_me)
    {
      for ( int i = 0; i < N_OF_SERF_START; i++ )
      {
        Coord pos;
        do
        {
          pos = startPosition + Coord(randomNum(50), randomNum(50));
        } while (Serf::getSerf(pos) || !field.isPassable(pos));
        Serf::Type type = Serf::Type( i % Serf::N_TYPES );
        if (i < N_OF_SERF_START / 2)
        {
          type = Serf::SERF;
        }
        it->createSerf( type, pos);
      }
    }
  }
  show_mouse(screen);
}

void Zon::setSpeed(int speed)
{
  m_speed = speed;
  if (speed < 1)
  {
    speed = 1;
  }
  else if ( speed > 32000 )
  {
    speed = 32000;
  }
}

void Zon::speedUp()
{
  setSpeed( std::min( m_speed - 1, int(m_speed / 1.2) ) );
}

void Zon::speedDown()
{
  setSpeed( std::max( m_speed + 1, int(m_speed * 1.2) ) );
}

void Zon::initsetvariables()
{
//   if (!m_seedrand) 
//     m_seedrand=time(0);
  m_seedrand = 0;
  srand(m_seedrand);
  m_turn=0;
  m_speed=50;

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
    m_tick=0;
    m_now=timer;
    m_me->chooseJobs();

    for (boost::ptr_vector<Player>::iterator pl = m_players.begin(); pl != m_players.end(); ++pl)
    {
      pl->checkJobs();
    }
    for (boost::ptr_vector<Player>::iterator pl = m_players.begin(); pl != m_players.end(); ++pl)
    {
      pl->doJobs();
    }
    m_userInterface->userInput(this);
    m_userInterface->drawScreen(m_turn, m_tick);
    for (m_tick=0;m_tick<16;m_tick++) 
    {
      while (timer < m_now+((m_tick+1)*m_speed)/50)
      {
        m_userInterface->userInput(this);
        m_userInterface->drawScreen(m_turn, m_tick);                 //  *  MAIN  *
      }
    }
    m_turn++;
  }
  quit();
}



