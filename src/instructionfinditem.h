#ifndef INSTRUCTIONFINDITEM_H
#define INSTRUCTIONFINDITEM_H

#include "instruction.h"

class InstructionFindItem : public Instruction
{
  public:
    InstructionFindItem( Serf::Type setSerfType,
                 Item setCarryBefore,
                 const std::vector<Item>& setTargetItems,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription );
    virtual Path finalize( Task& task, Planner& planner, const Coord& start ) const;
  private:
    virtual boost::ptr_vector<Task> makeMyTasks( Planner& planner, const Area* occupies, const Coord& planEnd, const boost::ptr_list<Area>& areas ) const;
    bool estimateScore( Task& task, const Coord& start ) const;
    std::vector<Item> targetItems;
};

#endif // INSTRUCTIONFINDITEM_H
