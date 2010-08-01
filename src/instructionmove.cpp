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

void InstructionMove::estimateScore( Task& task, const Coord& start, const Area& targetArea ) const
{
  task.setEnd( targetArea.findNearestItem( start, targetItem ) );
  double score = ( carryAfter == VOID || carryAfter == ENDPLAN ) ? 50 : -50;
  unsigned int steps = start.minDistanceTo( task.getEnd() ) + 1;
  if ( targetType == AREA )
  {
    score += 10;
  }
  else
  {
    Item item =      targetType == AREAPUT ? carryBefore : targetItem;
    int multiplier = targetType == AREAPUT ? +1 : -1;
    int portParam = targetArea.getPort(item);
    int areaTotal = targetArea.getAvailable(item);
    score += multiplier * 10 * pow(2, abs(portParam)) * sign(portParam);
    int areaTotalTotal = targetArea.getArea();
    double oldFr = static_cast<double>(areaTotal) / areaTotalTotal;
    int total = targetArea.getAreaManager()->getAvailable(item);
    int totaltotal = targetArea.getAreaManager()->getAvailableTotal();
    double oldTotalFr = static_cast<double>(total) / totaltotal;
    double newFr = static_cast<double>(areaTotal + multiplier) / areaTotalTotal;
    double newTotalFr = static_cast<double>(total + multiplier) / totaltotal;
    if ( std::abs( oldFr - oldTotalFr ) > 0.1 ||
         std::abs( newFr - newTotalFr ) > 0.1)   // more than 10% off
    {
      score += 0.1 * ( std::abs( oldFr - oldTotalFr ) - std::abs( newFr - newTotalFr) );
    }
  }
  task.setScore( score );
  task.setSteps( steps );
}

Path InstructionMove::finalize( Task& task, Planner& /*planner*/, const Coord& start ) const
{
  return Field::current()->getPathFinder().findPath( start, task.getEnd() );
}

std::auto_ptr<boost::ptr_vector<Task> > InstructionMove::makeMyTasks( Planner& planner, const OccArea* /*occupies*/, const Coord& start ) const
{
  boost::ptr_vector<Task> tasks;
  const boost::ptr_vector<Area>& areas = planner.getAreaManager().getAreas();
  for ( boost::ptr_vector<Area>::const_iterator areaIt = areas.begin(); areaIt != areas.end(); ++areaIt )
  {
    if ( areaIt->canBeUsedBy( serfType ) && areaIt->hasAvailable( targetItem ) )
    {
      Task* task = new Task( *this, planner );
      estimateScore( *task, start, *areaIt );
      tasks.push_back( task );
    }
  }
  return tasks.release();
}