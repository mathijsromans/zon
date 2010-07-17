#ifndef INSTRUCTIONGOHOME_H
#define INSTRUCTIONGOHOME_H

#include "instruction.h"

class InstructionGoHome : public Instruction
{
  public:
    InstructionGoHome( Serf::Type setSerfType,
                 Item setCarryBefore,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription );
    virtual Path finalize( Task& task, Planner& planner, const Coord& start ) const;

  private:
    virtual boost::ptr_vector<Task> makeMyTasks( Planner& planner, const OccArea* occupies, const Coord& start ) const;
};

#endif // INSTRUCTIONGOHOME_H
