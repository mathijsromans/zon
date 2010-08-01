#ifndef PLAYER_H
#define PLAYER_H

#include "planner.h"
#include "serf.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>

class Serf;

class Player : public boost::noncopyable
{
public:
  Player(int n);
  ~Player();
  void chooseJobs();
  void checkJobs();
  void doJobs();
  typedef boost::ptr_vector<Serf>::const_iterator SerfIter;
  SerfIter serfBegin() const { return m_serfs.begin(); }
  SerfIter serfEnd() const { return m_serfs.end(); }
  Serf* createSerf( Serf::Type type, const Coord& pos );
  unsigned int nSerf() const { return m_serfs.size(); }
  int getNumber() const { return m_number; }
  Planner& getPlanner() { return m_planner; }
  const Planner& getPlanner() const { return m_planner; }
  void setVictoryPointsNeeded( int value ) { m_victoryPointsNeeded = value; }
  int getVictoryPointsNeeded() const { return m_victoryPointsNeeded; }
private:
  int m_number;
  Planner m_planner;
  boost::ptr_vector<Serf> m_serfs;
  int m_victoryPointsNeeded;

};

#endif //PLAYER_H
