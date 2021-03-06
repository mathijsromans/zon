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
    Planner( int playerNumber );
    void createInstructions();
    void itemChanged( const Coord& c, Item oldItem );
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
    const CoordSet& getTargets() const { return m_targets; }
    std::auto_ptr<Plan> makeBestPlan( const Serf& s );
    AreaManager& getAreaManager() { return m_areaManager; }
    const AreaManager& getAreaManager() const { return m_areaManager; }
    void clearNoPlanFound() { m_turnNoSerfPlanFound = 0; }
    void initAI();


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
    int m_playerNumber;
    AreaManager m_areaManager;
    CoordSet m_targets;
    CoordSet m_changeTargets;
    boost::ptr_vector<Instruction> m_instructions;
    std::vector<Request> m_requests;

    unsigned int m_turnNoSerfPlanFound;
    boost::signals::scoped_connection m_connection;
};

#endif