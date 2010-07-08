#include "planner.h"

#include "field.h"
#include "area.h"
#include "serialplan.h"
#include "logger.h"
#include <boost/bind.hpp>
#include <boost/signal.hpp>

Planner::Planner()
{
  field.addItemChangedCallback(bind(&Planner::itemChanged, this, _1));
}

void Planner::itemChanged( const Coord& c )
{
  m_changeTargets.add(c);
}

void Planner::setTarget( const Coord& c )
{
//   Logger::getLogger() << "Planner::setTarget" << std::endl;
  assert(!m_targets.has(c));
  m_changeTargets.remove(c);
  m_targets.add(c);
  m_targetChangedSignal(c, true);
}

void Planner::unsetTarget( const Coord& c )
{
//   Logger::getLogger() << "Planner::unsetTarget" << std::endl;
  m_targets.remove(c);
  m_targetChangedSignal(c, false);
}

void Planner::newBuilding(const Coord& pos)
{
  for (boost::ptr_list<Area>::const_iterator it = m_areaManager.getAreas().begin(); it != m_areaManager.getAreas().end(); ++it)
  {
    if ( it->getType() == Serf::BUILDER && it->contains(pos) )
    {
      return;
    }
  }
  Area* area = m_areaManager.addNewArea( new Area( *this, pos, Serf::BUILDER) );
  m_areaManager.doBuildMerge(area);
}

std::auto_ptr<Plan> Planner::makeBestPlan( const Serf& s)
{
//   Logger::getLogger() << "Planner::makeBestPlan" << std::endl;
  SerialPlan currentPlan( *this, s );
  std::auto_ptr<SerialPlan> bestPlan;
  findBestContinuation( currentPlan, bestPlan );
  if ( bestPlan->efficiency() > 0 )
  {
    if ( bestPlan->finalize() )
    {
//       bestPlan->writeToLog();
      return std::auto_ptr<Plan>( bestPlan );
    }
  }
  return std::auto_ptr<Plan>();
}

void Planner::findBestContinuation( SerialPlan& currentPlan, std::auto_ptr<SerialPlan>& bestPlan )
{
  const unsigned int maxDepth = 2;
  if ( !bestPlan.get() || currentPlan.efficiency() > bestPlan->efficiency() )
  {
    bestPlan.reset( new SerialPlan(currentPlan) );
  }
  if ( currentPlan.getTasksSize() >= maxDepth )
  {
    return;
  }
  boost::ptr_vector<Task> tasks = currentPlan.getNextTasks( m_areaManager.getAreas() );
  while ( !tasks.empty() )
  {
    currentPlan.addTask( tasks.pop_back().release() );
    findBestContinuation( currentPlan, bestPlan );
    currentPlan.popTask();
  }
}