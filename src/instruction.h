#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "serf.h"
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
class Task;
class Planner;

class Instruction
{
  public:

    Instruction( Serf::Type setSerfType,
                Item setCarryBefore,
                Item setCarryAfter,
                Serf::JobType setJob,
                std::string setDescription );

    virtual ~Instruction() {}
    boost::ptr_vector<Task> makeTasks( Planner& planner, Serf::Type type, const Area* occupies, Item carry, const Coord& planEnd, const boost::ptr_list<Area>& areas ) const;
    virtual Path finalize( Task& task, Planner& planner, const Coord& start ) const = 0;

    Serf::Type serfType;
    Item carryBefore;


    Item carryAfter;
    Serf::JobType job;
    std::string description;
  private:
    virtual boost::ptr_vector<Task> makeMyTasks( Planner& planner, const Area* occupies, const Coord& planEnd, const boost::ptr_list<Area>& areas ) const = 0;

};

#endif // INSTRUCTION_H
