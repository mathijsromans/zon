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

void Player::chooseJobs() {
  std::for_each(m_serfs.begin(), m_serfs.end(), std::mem_fun_ref(&Serf::chooseJob) );
}

void Player::checkJobs() {
  std::for_each(m_serfs.begin(), m_serfs.end(), std::mem_fun_ref(&Serf::checkJob) );
}

void Player::doJobs()
{
  std::for_each(m_serfs.begin(), m_serfs.end(), std::mem_fun_ref(&Serf::doJob) );
}


