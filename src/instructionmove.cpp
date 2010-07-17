#include "instructionmove.h"
#include "task.h"
#include "area.h"
#include "path.h"
#include "field.h"
#include "planner.h"

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

InstructionMove::InstructionMove( Serf::Type setSerfType,
                 TargetType setTargetType,
                 Item setCarryBefore,
                 Item setTargetItem,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription )
    : Instruction( setSerfType, setCarryBefore, setCarryAfter, setJob, setDescription ),
      targetType(setTargetType),
      targetItem(setTargetItem)
{
}

bool InstructionMove::estimateScore( Task& task, const Coord& start, const Area& targetArea ) const
{
  double score = ( carryAfter == VOID || carryAfter == ENDPLAN ) ? 50 : -50;
  unsigned int steps;
  bool success = false;
//     case REQUEST:
//       assert(targetArea);
//       if ( boost::optional<Coord> request = targetArea->getRequest( serfType, carryBefore ) )
//       {
//         task.setEnd( *request );
//         steps = start.minDistanceTo( task.getEnd() ) + 1;
//         success = true;
//       }
//       break;

  if (targetArea.canBeUsedBy( serfType ) && targetArea.hasAvailable( targetItem ))
  {
    task.setEnd( targetArea.findNearestItem( start, targetItem ) );
    steps = start.minDistanceTo( task.getEnd() ) + 1;
    if ( targetType == AREA )
    {
      score += 10;
    }
    else
    {
      Item item =      ( targetType == AREAPUT ) ? carryBefore : targetItem;
      int multiplier = ( targetType == AREAPUT ) ? +1 : -1;
      int portParam = targetArea.getPort(item);
      score += multiplier * 10 * pow(2, abs(portParam)) * sign(portParam);
    }
    success = true;
  }
  task.setScore( score );
  task.setSteps( steps );
  return success;
}

Path InstructionMove::finalize( Task& task, Planner& /*planner*/, const Coord& start ) const
{
  return field.getPathFinder().findPath( start, task.getEnd() );
}

boost::ptr_vector<Task> InstructionMove::makeMyTasks( Planner& planner, const OccArea* /*occupies*/, const Coord& start ) const
{
  boost::ptr_vector<Task> tasks;
  const boost::ptr_list<Area>& areas = planner.getAreaManager().getAreas();
  for ( boost::ptr_list<Area>::const_iterator areaIt = areas.begin(); areaIt != areas.end(); ++areaIt )
  {
    std::auto_ptr<Task> task( new Task( *this, planner ) );
    if ( estimateScore( *task, start, *areaIt ) )
    {
      tasks.push_back( task.release() );
    }
  }
  return tasks;
}