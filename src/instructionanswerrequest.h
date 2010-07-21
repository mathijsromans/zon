#ifndef INSTRUCTIONANSWERREQUEST_H
#define INSTRUCTIONANSWERREQUEST_H

#include "instruction.h"

class InstructionAnswerRequest : public Instruction
{
  public:
    InstructionAnswerRequest( Serf::Type setSerfType,
                 Item setCarryBefore,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription );
    virtual Path finalize( Task& task, Planner& planner, const Coord& start ) const;

  private:
    virtual std::auto_ptr<boost::ptr_vector<Task> > makeMyTasks( Planner& planner, const OccArea* occupies, const Coord& start ) const;
};

#endif // INSTRUCTIONANSWERREQUEST_H
