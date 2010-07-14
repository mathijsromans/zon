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
    Path finalize( const Coord& start );
    void setTarget();
    void unsetTarget();
    const Instruction& getInstruction() const { return m_instruction; }
    Coord getEnd() const { return m_end; }
    double getScore() const { return m_score; }
    double getSteps() const { return m_steps; }
    void setScore( double score ) { m_score = score; }
    void setSteps( unsigned int steps ) { m_steps = steps; }
    void setGuessedEnd(const Coord& end) { m_end = end; }
    void setEnd(const Coord& end) { m_endIsSet = true; m_end = end; }
    const Area* getTargetArea() const { return m_targetArea; }

  private:
    const Instruction& m_instruction;
    Planner& m_planner;
    const Area* m_targetArea;
    bool m_endIsSet;
    bool m_targetSet;
    Coord m_end;
    double m_score;
    unsigned int m_steps;

};

#endif
