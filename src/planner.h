#ifndef PLANNER_H
#define PLANNER_H

#include "config.h"
#include "areamanager.h"
#include "coordset.h"
#include "instruction.h"
#include <utility>
#include <cassert>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>
#include <boost/signals.hpp>

class Plan;
class SerialPlan;
class Serf;

/// Owns areas, makes new plans and coordinates plans
class Planner
{
  public:
    Planner();
    void createInstructions();
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
    AreaManager& getAreaManager() { return m_areaManager; }
    boost::signals::connection addTargetChangedCallback( const boost::signal<void (const Coord&, bool)>::slot_type& slot );
    void clearNoPlanFound() { m_turnNoSerfPlanFound = 0; }

    struct Request
    {
      Serf::Type type;
      Item carry;
      Coord pos;
      bool operator==( const Request& rhs ) const { return type == rhs.type && carry == rhs.carry && pos == rhs.pos; }
    };

    void addRequest( const Request& request );
    const Request* findNearestAvailableRequest( Serf::Type type, Item carry, const Coord& start ) const;
    void takeRequest( const Request& request );

  private:
    void findBestContinuation( SerialPlan& currentPlan, std::auto_ptr<SerialPlan>& bestPlan );
    std::auto_ptr<SerialPlan> findBestPlan( const Serf& s);
    AreaManager m_areaManager;
    CoordSet<MAPWIDTH, MAPHEIGHT> m_targets;
    CoordSet<MAPWIDTH, MAPHEIGHT> m_changeTargets;
    boost::signal<void (const Coord&, bool)> m_targetChangedSignal;
    boost::ptr_vector<Instruction> m_instructions;
    std::vector<Request> m_requests;

    unsigned int m_turnNoSerfPlanFound;

};

#endif