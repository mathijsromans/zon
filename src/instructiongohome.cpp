#include "instructiongohome.h"

#include "instructionmove.h"
#include "task.h"
#include "occarea.h"
#include "path.h"
#include "field.h"
#include "planner.h"

InstructionGoHome::InstructionGoHome( Serf::Type setSerfType,
                 Item setCarryBefore,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription )
    : Instruction( setSerfType, setCarryBefore, setCarryAfter, setJob, setDescription )
{
}

Path InstructionGoHome::finalize( Task& task, Planner& planner, const Coord& start ) const
{
  if ( serfType == Serf::TEACHER && job == Serf::ACTPREPARE )
  {
    Planner::Request request;
    request.type = Serf::SERF;
    request.carry = VOID;
    request.pos = task.getEnd().next();
    planner.addRequest( request );
  }
  return field.getPathFinder().findPath( start, task.getEnd() );
}

boost::ptr_vector<Task> InstructionGoHome::makeMyTasks( Planner& planner, const OccArea* occupies, const Coord& start ) const
{
  boost::ptr_vector<Task> tasks;
  if ( occupies && occupies->isActive() )
  {
    Coord end = occupies->center();
    if ( !planner.hasTarget( end ) )
    {
      Task* task = new Task( *this, planner );
      task->setEnd( end );
      task->setScore( 100 + ( carryAfter == VOID || carryAfter == ENDPLAN ? 50 : -50 ) );
      task->setSteps( start.minDistanceTo( end ) + 1 );
      tasks.push_back( task );
    }
  }
  return tasks;
}