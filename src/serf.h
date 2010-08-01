#ifndef ZONSERF_H
#define ZONSERF_H

#include "item.h"
#include "coord.h"
#include <vector>
#include <memory>
#include <boost/optional.hpp>
#include <boost/array.hpp>
class Area;
class OccArea;
class Player;
class Plan;
class Serf;
class Path;
class BITMAP;

class Serf
{
  public:
    enum Type { SERF, BUILDER, STONEMASON, WOODCUTTER, GRINDER, WOMAN, FARMER, TEACHER, N_TYPES };  // don't forget to update s_serfTypeNames!
    enum JobType { SLEEP, MOVE, EAT, TAKE, ACT, BUILD_START, BUILDWALL, BUILDFLOOR, BUILDDOOR, BUILDROAD, BUILD_END, ACTPREPARE };
    Serf(Type type, Player& player, const Coord& pos);
    ~Serf();
    Player& getPlayer() const { return m_player; }
    Type getType() const { return m_type; }
    void setType(const Type& type)  { m_type = type; }
    JobType getJob() const { return m_job; }
    void setJob(const JobType& job) { m_job = job; }
    OccArea* getOccupies() const { return m_occupies; }
    void setOccupies(OccArea* occupies);
    const Coord& getPos() const { return m_pos; }
    Item getLoad() const { return m_load; }
    const Path* getPath() const;
    int drawOffsetX(int tick) const; 
    int drawOffsetY(int tick) const;
    void chooseJob();
    void checkJob();
    void doJob();
    void startTurn();
    void draw(BITMAP* bmp, int x, int y, int tick) const;
    static const boost::array<std::string, N_TYPES>& getSerfTypeNames() { return s_serfTypeNames; }
  private:
    enum StatusType { NONEXIST, READY, JOBCHOSEN, JOBCHECKING, JOBCHECKED };
    void makePlan();
    int rankplan(Area* a1, Area*al2, const Plan& plan);
    int maxpasstime();
    void executeJob();
    static const std::vector<Plan>& getPlans();
    static const Plan* otherPlan();
    Type m_type;
    Coord m_pos;
    OccArea *m_occupies;
    Item m_load;
    StatusType m_status;
    Direction m_dir;
    int m_pass, m_passtot;
    std::auto_ptr<Plan> m_plan;
    JobType m_job;
    enum { R_SUCCESS, R_DELAYED, R_FAILED } m_jobResult;
    Player& m_player;
    boost::optional<Type> m_becomesType;
    static const boost::array<std::string, N_TYPES> s_serfTypeNames;
};

#endif
