#include "player.h"
#include "serf.h"
#include <algorithm>

Player::Player(int n)
  : m_number(n),
    m_planner(n),
    m_victoryPointsNeeded(0)
{
}

Player::~Player()
{
}

Serf* Player::createSerf( Serf::Type type, const Coord& pos )
{
  Serf* s = new Serf( type, *this, pos );
  m_serfs.push_back( s );
  return s;
}

void Player::chooseJobs() const
{
  std::for_each(m_serfs.begin(), m_serfs.end(), std::mem_fun(&Serf::chooseJob) );
}

void Player::checkJobs() const
{
  std::for_each(m_serfs.begin(), m_serfs.end(), std::mem_fun(&Serf::checkJob) );
}

void Player::doJobs() const
{
  std::for_each(m_serfs.begin(), m_serfs.end(), std::mem_fun(&Serf::doJob) );
}


