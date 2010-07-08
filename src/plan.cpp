#include "plan.h"
#include "occarea.h"
#include "field.h"
#include "logger.h"

#include <algorithm>
#include <cassert>
#include <iostream>

Plan::Plan()
 : m_currentJob( Serf::SLEEP ),
   m_currentDir( 0 ),
   m_firstJobSet( false )
{
}

Plan::~Plan()
{
}

void Plan::getJobAndDir( Serf::JobType* job, Direction* dir )
{
  if ( !m_firstJobSet )
  {
    gotoNextJob();
    m_firstJobSet = true;
  }
  *job = m_currentJob;
  *dir = m_currentDir;
}

bool Plan::gotoNextJob()
{
  return getNextJobAndDir( &m_currentJob, &m_currentDir );
}

