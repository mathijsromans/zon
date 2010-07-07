#ifndef PLANNER_H
#define PLANNER_H

#include "config.h"
#include "areamanager.h"
#include "coordset.h"
#include <utility>
#include <cassert>
#include <boost/function.hpp>
#include <boost/signals.hpp>
class Plan;
class Serf;

class Planner
{
  public:
    Planner();
    void itemChanged( const Coord& c );
    /**
     * Make or adjust areas so building can take place
     * @param pos position
     */
    void newBuilding(const Coord& pos);
    void setChangeTarget( const Coord& c ) { m_changeTargets.add(c); }
    void unsetChangeTarget( const Coord& c ) { m_changeTargets.remove(c); }
    bool hasChangeTarget( const Coord& c ) const { return m_changeTargets.has(c); }
    bool hasTarget( const Coord& c ) const { return m_targets.has(c); }
    void setTarget( const Coord& c );
    void unsetTarget( const Coord& c );
    const CoordSet<MAPWIDTH, MAPHEIGHT>& getTargets() const { return m_targets; }
    std::auto_ptr<Plan> makeBestPlan( const Serf& s );
    void findBestContinuation( Plan& currentPlan, std::auto_ptr< Plan >& bestPlan );
    AreaManager& getAreaManager() { return m_areaManager; }
    boost::signals::connection addTargetChangedCallback( const boost::signal<void (const Coord&, bool)>::slot_type& slot )
    {
      return m_targetChangedSignal.connect( slot );
    }
  private:
    AreaManager m_areaManager;
    CoordSet<MAPWIDTH, MAPHEIGHT> m_targets;
    CoordSet<MAPWIDTH, MAPHEIGHT> m_changeTargets;
    boost::signal<void (const Coord&, bool)> m_targetChangedSignal;
};

#endif