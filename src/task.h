#ifndef TASK_H
#define TASK_H

#include "serf.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_list.hpp>
class Planner;
class Instruction;

class Task
{
  public:
    Task( const Instruction& instruction, Planner& planner, const Area* setTargetArea = 0 );
    Task( const Task& t );
    ~Task();

    static boost::ptr_vector<Task> getNextTasks( Planner& planner, Serf::Type serfType, const Area* occupies, Item carry, const Coord& planEnd, const boost::ptr_list<Area>& areas );

    /**
    * Make an estimate of score of this task
    * @post must set end if successful
    * @param from start coordinate
    * @param occupies area occupied by the serf
    * @return false if the task is impossible
    */
    bool estimateScore( const Coord& start );
    Path finalize( const Coord& start );
    void setTarget();
    void unsetTarget();
    const Instruction& getInstruction() const { return m_instruction; }
    Coord getEnd() const { return end; }
    double getScore() const { return m_score; }
    double getSteps() const { return m_steps; }

  private:
    const Instruction& m_instruction;
    Planner& m_planner;
    const Area* m_targetArea;
    bool m_endIsSet;
    bool m_targetSet;
    Coord end;
    double m_score;
    unsigned int m_steps;

};

#endif
