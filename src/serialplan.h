#ifndef SERIALPLAN_H
#define SERIALPLAN_H

#include "instruction.h"
#include "plan.h"
#include "serf.h"
#include "path.h"
#include "task.h"
#include "planner.h"
#include <memory>
#include <boost/ptr_container/ptr_deque.hpp>

class Rectangle;

class SerialPlan : public Plan
{
  public:
    SerialPlan( Planner& planner, const Serf& s);
    virtual ~SerialPlan();

    bool getNextJobAndDir( Serf::JobType* job, Direction* dir );

    virtual const Path& getPath() { return m_path; }
    double efficiency() const { return m_score / (m_steps + 1); }
    bool finalize();
    unsigned int getTasksSize() const { return m_tasks.size(); }
    const Serf& getSerf() const { return m_serf; }
    void popTask();
    void addTask(Task* task);
    void writeToLog() const;
    Coord getEnd() const;
    Item carryAfter() const;

  private:

    Coord getTarget() const { return m_tasks.front().getEnd(); }
    Serf::JobType getJob() const { return m_tasks.front().getInstruction().job; }


    void clearTasks() { m_tasks.clear(); m_score = 0; m_steps = 0; }

    Planner& m_planner;
    boost::ptr_deque<Task> m_tasks;
    Path m_path;
    const Serf& m_serf;
    double m_score;
    unsigned int m_steps;
};

#endif // SERIALPLAN_H
