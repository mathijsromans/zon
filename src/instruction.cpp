#include "instruction.h"
#include "task.h"

Instruction::Instruction( Serf::Type setSerfType,
               Item setCarryBefore,
               Item setCarryAfter,
               Serf::JobType setJob,
               std::string setDescription )
    : serfType(setSerfType),
      carryBefore(setCarryBefore),
      carryAfter(setCarryAfter),
      job(setJob),
      description(setDescription)
{
}

boost::ptr_vector<Task> Instruction::makeTasks( Planner& planner, Serf::Type type, const OccArea* occupies, Item carry, const Coord& start ) const
{
  if ( serfType == type && carryBefore == carry )
  {
    return makeMyTasks( planner, occupies, start );
  }
  return boost::ptr_vector<Task>();
}
