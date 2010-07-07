#include "plan.h"
#include "occarea.h"
#include "field.h"
#include "logger.h"

#include <algorithm>
#include <cassert>
#include <iostream>

Plan::Plan( Planner& planner, const Serf& s)
  : m_planner( planner ),
    m_serf(s),
    m_score(0),
    m_steps(0),
    m_taskCompleted( false )
{
}

Plan::~Plan()
{
}

Coord Plan::getEnd() const
{
  if (!m_tasks.empty())
  {
    return m_tasks.back().getEnd();
  }
  return m_serf.getPos();
}

Item Plan::carryAfter() const
{
  if (!m_tasks.empty())
  {
    return m_tasks.back().getInstruction().carryAfter;
  }
  return m_serf.getLoad();
}

bool Plan::finalize()
{
  m_path = Path( m_serf.getPos() );
  bool validPath = true;
  for (boost::ptr_vector<Task>::iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
  {
    Path taskPath = it->finalize( m_path.getFinish() );
    if ( !taskPath.isValid() )
    {
      validPath = false;
      break;
    }
    m_path.append( taskPath );
  }
  if ( !validPath )
  {
    m_tasks.clear();
    m_path = Path();
  }
  return validPath;
}

bool Plan::isValid( const Coord& currentPos ) const
{
  if (m_tasks.empty())
  {
    return false;
  }
  for (boost::ptr_vector<Task>::const_iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
  {
    if ( m_planner.hasChangeTarget( it->getEnd() ) )
    {
      return false;
    }
  }
  if ( currentPos != getTarget() && !field.isPassable(currentPos.next( m_path.getDirection() )) )
  {
    return false;
  }
  return true;
}

void Plan::getJobAndDir( const Coord& currentPos, Serf::JobType* job, Direction* dir )
{
  if ( currentPos == getTarget())
  {
    *job = getJob();
    m_taskCompleted = true;
  }
  else
  {
    *job = Serf::MOVE;
    *dir = m_path.getDirection();
  }
}

void Plan::next()
{
  if ( m_taskCompleted )
  {
    m_taskCompleted = false;
    m_tasks.erase( m_tasks.begin() );
  }
  else
  {
    m_path.next();
  }
}

boost::ptr_vector<Task> Plan::getNextTasks( const boost::ptr_list<Area>& areas ) const
{
  return Task::getNextTasks( m_planner, m_serf.getType(), m_serf.getOccupies(), carryAfter(), getEnd(), areas );
}

void Plan::popTask()
{
  m_score -= m_tasks.back().getScore();
  m_steps -= m_tasks.back().getSteps();
  m_tasks.back().unsetTarget();
  m_tasks.pop_back();
}

void Plan::addTask(Task* task)
{
  task->setTarget();
  m_score += task->getScore();
  m_steps += task->getSteps();
  m_tasks.push_back(task);
}

void Plan::writeToLog() const
{
  Logger::getLogger() << " Plan::writeToLog() : found bestPlan with " << m_tasks.size() << " tasks." << std::endl;
  for ( unsigned int i = 0 ; i < m_tasks.size(); ++i )
  {
    Logger::getLogger() << " Plan::writeToLog() : task : " << m_tasks[i].getInstruction().description << std::endl;
  }
}


