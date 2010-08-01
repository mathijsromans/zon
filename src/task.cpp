#include "task.h"
#include "instruction.h"
#include "path.h"
#include "planner.h"
#include "logger.h"

#include <cmath>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>

Task::Task( const Instruction& instruction, Planner& planner )
  : m_instruction( instruction ),
    m_planner( planner ),
    m_endIsSet( false ),
    m_targetSet( false ),
    m_end(),
    m_score(0),
    m_steps(0)
{
}

Task::Task( const Task& t )
  : m_instruction( t.m_instruction ),
    m_planner( t.m_planner ),
    m_endIsSet( t.m_endIsSet ),
    m_targetSet( false ),
    m_end( t.m_end ),
    m_score( t.m_score ),
    m_steps( t.m_steps )
{
}

Task::~Task()
{
  if ( m_targetSet )
  {
    unsetTarget();
  }
}

Path Task::finalize( const Coord& start )
{
  return m_instruction.finalize( *this, m_planner, start );
}

void Task::setTarget()
{
  if ( m_endIsSet )
  {
    assert( !m_targetSet );
    m_targetSet = true;
    m_planner.setTarget( m_end );
  }
}

void Task::unsetTarget()
{
  if ( m_endIsSet )
  {
    assert( m_targetSet );
    m_targetSet = false;
    m_planner.unsetTarget( m_end );
  }
}