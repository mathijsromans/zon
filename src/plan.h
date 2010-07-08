#ifndef PLAN_H
#define PLAN_H

#include "serf.h"
class Path;

class Plan
{
  public:
    Plan();
    virtual ~Plan();
    void getJobAndDir( Serf::JobType* job, Direction* dir );
    bool gotoNextJob();
    virtual const Path& getPath() = 0;
  protected:
    virtual bool getNextJobAndDir( Serf::JobType* job, Direction* dir ) = 0;
  private:
    Serf::JobType m_currentJob;
    Direction m_currentDir;
    bool m_firstJobSet;
};

#endif
