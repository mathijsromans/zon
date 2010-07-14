#include "instructiongohome.h"

#include "instructionmove.h"
#include "task.h"
#include "area.h"
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

bool InstructionGoHome::estimateScore( Task& task, Planner& planner, const Coord& start, const Area* targetArea ) const
{
  if ( !targetArea || planner.hasTarget( task.getEnd() ) )
  {
    return false;
  }
  task.setEnd( targetArea->center() );
  task.setScore( 100 + ( carryAfter == VOID || carryAfter == ENDPLAN ) ? 50 : -50 );
  task.setSteps( start.minDistanceTo( task.getEnd() ) + 1 );
  return true;
}

Path InstructionGoHome::finalize( Task& task, Planner& planner, const Coord& start ) const
{
  Path path = field.getPathFinder().findPath( start, task.getEnd() );
  return path;
}

boost::ptr_vector<Task> InstructionGoHome::makeMyTasks( Planner& planner, const Area* occupies, const Coord& planEnd, const boost::ptr_list<Area>& areas ) const
{
  boost::ptr_vector<Task> tasks;
  std::auto_ptr<Task> task( new Task( *this, planner, &*occupies ) );
  if ( estimateScore( *task, planner, planEnd, &*occupies ) )
  {
    tasks.push_back( task.release() );
  }
  return tasks;
}