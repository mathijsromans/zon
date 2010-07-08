#include "task.h"
#include "instruction.h"
#include "area.h"
#include "areamanager.h"
#include "path.h"
#include "field.h"
#include "planner.h"
#include "logger.h"

#include <cmath>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>

static int sign(int x)
{
  if (x > 0)
  {
    return +1;
  }
  else if (x < 0)
  {
    return -1;
  }
  return 0;
}

Task::Task( const Instruction& instruction, Planner& planner, const Area* setTargetArea )
  : m_instruction( instruction ),
    m_planner( planner ),
    m_targetArea( setTargetArea ),
    m_endIsSet( false ),
    m_targetSet( false ),
    end(),
    m_score(0),
    m_steps(0)
{
}

// todo: stop using this copy-constructor, since the program should not depend on return value optimization in the compiler
Task::Task( const Task& t )
  : m_instruction( t.m_instruction ),
    m_planner( t.m_planner ),
    m_targetArea( t.m_targetArea ),
    m_endIsSet( t.m_endIsSet ),
    m_targetSet( false ),
    end( t.end ),
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

boost::ptr_vector<Task> Task::getNextTasks( Planner& planner, Serf::Type serfType, const Area* occupies, Item carry, const Coord& planEnd, const boost::ptr_list<Area>& areas )
{
  boost::ptr_vector<Task> result;
  const std::vector<Instruction>& instructions = Instruction::getInstructions();
  for (std::vector<Instruction>::const_iterator instruction = instructions.begin(); instruction != instructions.end(); ++instruction)
  {
    if ( instruction->serfType == serfType && instruction->carryBefore == carry )
    {
      if ( instruction->iterateOverAreas() )
      {
        for ( boost::ptr_list<Area>::const_iterator areaIt = areas.begin(); areaIt != areas.end(); ++areaIt )
        {
          std::auto_ptr<Task> task( new Task( *instruction, planner, &*areaIt ) );
          if ( task->estimateScore( planEnd ) )
          {
            result.push_back( task.release() );
          }
        }
      }
      else
      {
        std::auto_ptr<Task> task( new Task( *instruction, planner, &*occupies ) );
        if ( task->estimateScore( planEnd ) )
        {
          result.push_back( task.release() );
        }
      }
    }
  }
//   Logger::getLogger() << "Carry: " << carry << std::endl;
//   for ( boost::ptr_vector<Task>::const_iterator it = result.begin(); it != result.end(); ++it )
//   {
//     Logger::getLogger() << it->getInstruction().description << std::endl;
//   }
//   Logger::getLogger() << " : returned  " << result.size() << " tasks." << std::endl;
  return result;
}

bool Task::estimateScore( const Coord& start )
{
  assert( !m_targetSet );
  m_score = ( m_instruction.carryAfter == VOID || m_instruction.carryAfter == ENDPLAN ) ? 50 : -50;
  bool success = false;
  switch( m_instruction.targetType )
  {
    case Instruction::ANY:
      m_steps = 20; // just assume we can find the any target item(s)
      end = start; // somewhere around here
      success = true;
      break;
    case Instruction::HOME:
      if ( m_targetArea )
      {
        end = m_targetArea->center();
        m_endIsSet = true;
        if ( !m_planner.hasTarget( end ) )
        {
          m_score += 100;
          m_steps = start.minDistanceTo( end );
          success = true;
        }
      }
      break;
    case Instruction::AREA:
    case Instruction::AREAPUT:
    case Instruction::AREATAKE:
    {
      assert(m_targetArea);
      if (m_targetArea->canBeUsedBy( m_instruction.serfType ) && m_targetArea->hasAvailable( m_instruction.targetItem ))
      {
        end = m_targetArea->findNearestItem( start, m_instruction.targetItem );
        m_endIsSet = true;
        m_steps = start.minDistanceTo( end ) + 1;
        if ( m_instruction.targetType == Instruction::AREA )
        {
          m_score += 10;
        }
        else
        {
          Item item =      ( m_instruction.targetType == Instruction::AREAPUT ) ? m_instruction.carryBefore : m_instruction.targetItem;
          int multiplier = ( m_instruction.targetType == Instruction::AREAPUT ) ? +1 : -1;
          int portParam = m_targetArea->getPort(item);
          m_score += multiplier * 10 * pow(2, abs(portParam)) * sign(portParam);
        }
        success = true;
      }
      break;
    }
    default:
      assert(false);
      break;
  }
  return success;
}

Path Task::finalize( const Coord& start )
{
  assert( !m_targetSet );
  Path path;
  switch ( m_instruction.targetType )
  {
    case Instruction::ANY:
      path = field.getPathFinder().findPath(start, m_instruction.targetItems, m_planner.getTargets(), &end);
      m_endIsSet = true;
      break;
    case Instruction::HOME:
    case Instruction::AREA:
    case Instruction::AREAPUT:
    case Instruction::AREATAKE:
      path = field.getPathFinder().findPath(start, end);
      break;
    default:
      assert(false);
      break;
  }
  if ( path.isValid() )
  {
    setTarget();
  }
  return path;
}

void Task::setTarget()
{
  if ( m_endIsSet )
  {
    assert( !m_targetSet );
    m_targetSet = true;
    m_planner.setTarget( end );
  }
}

void Task::unsetTarget()
{
  if ( m_endIsSet )
  {
    assert( m_targetSet );
    m_targetSet = false;
    m_planner.unsetTarget( end );
  }
}