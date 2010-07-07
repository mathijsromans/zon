#ifndef PLAN_H
#define PLAN_H

#include "instruction.h"
#include "serf.h"
#include "path.h"
#include "task.h"
#include "planner.h"
#include <memory>
#include <deque>
class Rectangle;

class Plan
{
  public:
    Plan( Planner& planner, const Serf& s);
    ~Plan();

    /**
     * Whether the plan is still valid
     * @param currentPos current position
     * @return true if it is valid
     */
    bool isValid( const Coord& currentPos ) const;

    void getJobAndDir( const Coord& currentPos, Serf::JobType* job, Direction* dir );
    void next();

    const Path& getPath() { return m_path; }
    double efficiency() const { return m_score / (m_steps + 1); }
    bool finalize();
    unsigned int getTasksSize() const { return m_tasks.size(); }
    boost::ptr_vector<Task> getNextTasks( const boost::ptr_list<Area>& areas ) const;
    void popTask();
    void addTask(Task* task);
    void writeToLog() const;

  private:

    Coord getTarget() const { return m_tasks.front().getEnd(); }
    Serf::JobType getJob() const { return m_tasks.front().getInstruction().job; }

    Coord getEnd() const;
    Item carryAfter() const;
    void clearTasks() { m_tasks.clear(); m_score = 0; m_steps = 0; }

    Planner& m_planner;
    boost::ptr_vector<Task> m_tasks;
    Path m_path;
    const Serf& m_serf;
    double m_score;
    unsigned int m_steps;
    bool m_taskCompleted;
};


#endif
