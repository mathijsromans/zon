#include "instructionanswerrequest.h"

#include "instructionmove.h"
#include "task.h"
#include "area.h"
#include "path.h"
#include "field.h"
#include "planner.h"

InstructionAnswerRequest::InstructionAnswerRequest( Serf::Type setSerfType,
                 Item setCarryBefore,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription )
    : Instruction( setSerfType, setCarryBefore, setCarryAfter, setJob, setDescription )
{
}

Path InstructionAnswerRequest::finalize( Task& task, Planner& planner, const Coord& start ) const
{
  Planner::Request request = { serfType, carryBefore, task.getEnd() };
  planner.takeRequest( request );
  return field.getPathFinder().findPath( start, task.getEnd() );
}

std::auto_ptr<boost::ptr_vector<Task> > InstructionAnswerRequest::makeMyTasks( Planner& planner, const OccArea* /*occupies*/, const Coord& start ) const
{
  boost::ptr_vector<Task> tasks;
  if ( const Planner::Request* request = planner.findNearestAvailableRequest( serfType, carryBefore, start ) )
  {
    Task* task = new Task( *this, planner );
    task->setEnd( request->pos );
    task->setScore( 100 );
    task->setSteps( start.minDistanceTo( request->pos ) + 1 );
    tasks.push_back( task );
  }
  return tasks.release();
}