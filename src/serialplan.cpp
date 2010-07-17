#include "serialplan.h"

#include "occarea.h"
#include "field.h"
#include "logger.h"

#include <algorithm>
#include <cassert>
#include <iostream>

SerialPlan::SerialPlan( Planner& planner, const Serf& s)
  : Plan(),
    m_planner( planner ),
    m_serf(s),
    m_score(0),
    m_steps(0)
{
}

SerialPlan::~SerialPlan()
{
}

Coord SerialPlan::getEnd() const
{
  if (!m_tasks.empty())
  {
    return m_tasks.back().getEnd();
  }
  return m_serf.getPos();
}

Item SerialPlan::carryAfter() const
{
  if (!m_tasks.empty())
  {
    return m_tasks.back().getInstruction().carryAfter;
  }
  return m_serf.getLoad();
}

bool SerialPlan::finalize()
{
  if ( m_tasks.empty() )
  {
    return false;
  }
  for ( boost::ptr_deque<Task>::iterator task = m_tasks.begin(); task != m_tasks.end(); ++task )
  {
    task->setTarget();
  }
  m_path = m_tasks.front().finalize( m_serf.getPos() );
  if ( !m_path.isValid() )
  {
    m_tasks.clear();
    return false;
  }
  
  return true;
}

bool SerialPlan::getNextJobAndDir( Serf::JobType* job, Direction* dir )
{
  if ( m_tasks.empty() )
  {
    return false;
  }
  for (boost::ptr_deque<Task>::const_iterator it = m_tasks.begin(); it != m_tasks.end(); ++it)
  {
    if ( m_planner.hasChangeTarget( it->getEnd() ) )
    {
      return false;
    }
  }
  if ( !m_path.done() )
  {
    *job = Serf::MOVE;
    *dir = m_path.getDirection();
    m_path.next();
    return true;
  }
  if ( m_path.empty() )
  {
    // empty path: just do job and go to next tasks
    *job = getJob();
    m_tasks.pop_front();
    if ( !m_tasks.empty() )
    {
      m_path = m_tasks.front().finalize( m_serf.getPos() );
    }
    return true;
  }
  // non-empty path: re-evaluate plan after walking
  m_tasks.clear();
  m_planner.clearNoPlanFound();
  return false;
}

void SerialPlan::popTask()
{
  m_score -= m_tasks.back().getScore();
  m_steps -= m_tasks.back().getSteps();
  m_tasks.back().unsetTarget();
  m_tasks.pop_back();
}

void SerialPlan::addTask(Task* task)
{
  task->setTarget();
  m_score += task->getScore();
  m_steps += task->getSteps();
  m_tasks.push_back(task);
}

void SerialPlan::writeToLog() const
{
  Logger::getLogger() << " SerialPlan::writeToLog() : found bestPlan with " << m_tasks.size() << " tasks." << std::endl;
  for ( unsigned int i = 0 ; i < m_tasks.size(); ++i )
  {
    Logger::getLogger() << " SerialPlan::writeToLog() : task : " << m_tasks[i].getInstruction().description << std::endl;
  }
}


