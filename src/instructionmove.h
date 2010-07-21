#ifndef INSTRUCTIONMOVE_H
#define INSTRUCTIONMOVE_H

#include "instruction.h"

class InstructionMove : public Instruction
{
  public:
    enum TargetType{ AREA, AREAPUT, AREATAKE };

    InstructionMove( Serf::Type setSerfType,
                 TargetType setTargetType,
                 Item setCarryBefore,
                 Item setTargetItem,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription );

    virtual Path finalize( Task& task, Planner& planner, const Coord& start ) const;

  private:
    void estimateScore( Task& task, const Coord& start, const Area& targetArea ) const;
    virtual std::auto_ptr<boost::ptr_vector<Task> > makeMyTasks( Planner& planner, const OccArea* occupies, const Coord& start ) const;
    TargetType targetType;
    Item targetItem;
};

#endif // INSTRUCTIONMOVE_H
