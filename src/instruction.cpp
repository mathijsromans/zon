#include "instruction.h"
#include "task.h"
#include "area.h"
#include "field.h"
#include "planner.h"
#include "path.h"
#include <auto_ptr.h>
#include <cassert>

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

boost::ptr_vector<Task> Instruction::makeTasks( Planner& planner, Serf::Type type, const Area* occupies, Item carry, const Coord& planEnd, const boost::ptr_list<Area>& areas ) const
{
  if ( serfType == type && carryBefore == carry )
  {
    return makeMyTasks( planner, occupies, planEnd, areas );
  }
  return boost::ptr_vector<Task>();
}
