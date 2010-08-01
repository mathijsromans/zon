#include "game.h"

#include "global.h"
#include "field.h"
#include "player.h"
#include "resources.h"
#include <stdio.h>
#include <algorithm>
#include <time.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <allegro.h>

Game::Game( const Resources::Level& level, UserInterface& userInterface ) :
  m_level(level),
  m_seedrand(0),
  m_tick(0),
  m_speed(35),
  m_userInterface(userInterface),
  m_victoryAchieved(false)
{
  srand(m_seedrand);
}

Game::~Game()
{
}

void Game::init()
{
  std::auto_ptr<Field> field( new Field( m_level.map->w, m_level.map->h ) );
  for (int y=0;y<m_level.map->h;y++)
  {
    for (int x=0;x<m_level.map->w;x++)
    {
      if ( x == 0 || y == 0 || x == m_level.map->w - 1 || y == m_level.map->h - 1 )
      {
        field->setItem(Coord(x, y), SUPERROCK);
      }
      else
      {
        field->setItem(Coord(x, y), static_cast<Item>(m_level.map->line[y][x]));
      }
    }
  }
  Field::setCurrent( field );
  m_players.push_back( new Player(0) );
  m_players.push_back( new Player(1) );
  m_me = &m_players.front();
  m_userInterface.setPlayers(m_me, &m_players);
  m_userInterface.setViewOrigin( getStartPosition(0) );
  countVictoryPoints();
  createSerfs();
  for (unsigned int i = 1; i < m_players.size(); ++i)
  {
    m_players[i].getPlanner().initAI();
  }
}

void Game::countVictoryPoints()
{
  for (unsigned int i = 0; i < m_players.size(); ++i)
  {
    int victoryPoints = 0;
    Rectangle world = Field::current()->getInteriorWorldRect();
    for ( Rectangle::Iterator p = world.begin(); p != world.end(); ++p )
    {
      if ( Field::current()->getItem( *p ) == SPECIALFLOOR_START + 1 + static_cast<int>(i) )
      {
        ++victoryPoints;
      }
    }
    m_players[i].setVictoryPointsNeeded( victoryPoints );
  }
}

void Game::createSerfs()
{
  Rectangle worldRect = Field::current()->getInteriorWorldRect();
  for (unsigned int i = 0; i < m_players.size(); ++i)
  {
    Coord startPosition = getStartPosition(i);
    for ( int type = 0; type < Serf::N_TYPES; type++ )
    {
      for ( int k = 0; k < m_level.numSerfs[type]; k++ )
      {
        Coord pos;
        do
        {
          pos = startPosition + Coord(randomNum(50), randomNum(50));
        } while ( !worldRect.contains(pos) || Field::current()->getSerf(pos) || !Field::current()->isPassable(pos));
        m_players[i].createSerf( static_cast<Serf::Type>(type), pos);
      }
    }
  }
}

Coord Game::getStartPosition( unsigned int i ) const
{
  return Coord( 50 + (i & 1) * 100, 50 + ( ( (i + 1) / 2) & 1) * 100);
}

void Game::setSpeed(int speed)
{
  m_speed = std::max( speed, 0 );
}

void Game::speedUp()
{
  setSpeed( std::min( m_speed - 1, int(m_speed / 1.2) ) );
}

void Game::speedDown()
{
  setSpeed( std::max( m_speed + 1, int(m_speed * 1.2) ) );
}

void Game::quit()
{
  allegro_exit();
  exit(0);
}

void Game::mainLoop()
{
  m_userInterface.drawScreen(Global::turn, m_tick);
  m_userInterface.message( m_level.text );
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
    m_userInterface.userInput(this);
    m_userInterface.drawScreen(Global::turn, m_tick);
    for (m_tick=0;m_tick<16;m_tick++)
    {
      while (Global::timer < m_now+((m_tick+1)*m_speed)/50)
      {
        m_userInterface.userInput(this);
        m_userInterface.drawScreen(Global::turn, m_tick);                 //  *  MAIN  *
      }
    }
    if ( !m_victoryAchieved )
    {
      for (boost::ptr_vector<Player>::iterator pl = m_players.begin(); pl != m_players.end(); ++pl)
      {
        if ( pl->getVictoryPointsNeeded() <= 0 )
        {
          m_userInterface.victory( &*pl );
          m_victoryAchieved = true;
          break;
        }
      }
    }
    ++Global::turn;
  }
  quit();
}



