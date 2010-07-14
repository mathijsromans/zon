#include "instructionfinditem.h"
#include "task.h"
#include "planner.h"
#include "area.h"
#include "path.h"
#include "field.h"

InstructionFindItem::InstructionFindItem( Serf::Type setSerfType,
                 Item setCarryBefore,
                 const std::vector<Item>& setTargetItems,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription )
    : Instruction( setSerfType, setCarryBefore, setCarryAfter, setJob, setDescription ),
      targetItems(setTargetItems)
{
}

bool InstructionFindItem::estimateScore( Task& task, const Coord& start ) const
{
  task.setScore( ( carryAfter == VOID || carryAfter == ENDPLAN ) ? 50 : -50 );
  task.setSteps( 20 ); // just assume we can find the target item(s)
  task.setGuessedEnd( start ); // somewhere around here
  return true;
}

Path InstructionFindItem::finalize( Task& task, Planner& planner, const Coord& start ) const
{
  Coord newEnd;
  Path path = field.getPathFinder().findPath(start, targetItems, planner.getTargets(), &newEnd);
  if ( path.isValid() )
  {
    task.setEnd( newEnd );
    task.setTarget();
  }
  return path;
}

boost::ptr_vector<Task> InstructionFindItem::makeMyTasks( Planner& planner, const Area* occupies, const Coord& planEnd, const boost::ptr_list<Area>& areas ) const
{
  boost::ptr_vector<Task> tasks;
  std::auto_ptr<Task> task( new Task( *this, planner, &*occupies ) );
  if ( estimateScore( *task, planEnd ) )
  {
    tasks.push_back( task.release() );
  }
  return tasks;
}