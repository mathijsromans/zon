#include "planner.h"

#include "field.h"
#include "global.h"
#include "area.h"
#include "serialplan.h"
#include "occarea.h"
#include "logger.h"
#include "instructionmove.h"
#include "instructionfinditem.h"
#include "instructiongohome.h"
#include "instructionanswerrequest.h"
#include <boost/bind.hpp>
#include <boost/signal.hpp>
#include <boost/assign.hpp>

Planner::Planner( int playerNumber ) :
  m_playerNumber( playerNumber ),
  m_targets( Field::current()->getWidth(), Field::current()->getHeight() ),
  m_changeTargets( Field::current()->getWidth(), Field::current()->getHeight() ),
  m_turnNoSerfPlanFound( 0 )
{
  m_connection = Field::current()->addItemChangedCallback(bind(&Planner::itemChanged, this, _1, _2));
  createInstructions();
}

void Planner::createInstructions()
{
  m_instructions.clear();
  for (int i = PORT_START + 1; i < PORT_END; ++i)
  {
    m_instructions.push_back(new InstructionMove(Serf::SERF, InstructionMove::AREATAKE, VOID, static_cast<Item>(i), static_cast<Item>(i), Serf::TAKE, "Serf take item"));
    m_instructions.push_back(new InstructionMove(Serf::SERF, InstructionMove::AREAPUT, static_cast<Item>(i), FLOOR, VOID, Serf::TAKE, "Serf put item"));
  }
  m_instructions.push_back(new InstructionMove(Serf::SERF, InstructionMove::AREAPUT, FLOUR, static_cast<Item>(SPECIALFLOOR_START + 1 + m_playerNumber), VOID, Serf::TAKE, "Serf put flour to special floor"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, VOID, STONE, STONE, Serf::TAKE, "Builder take stone"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, VOID, BOARD, BOARD, Serf::TAKE, "Builder take board"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, STONE, ROADBUILD, VOID, Serf::BUILDROAD, "Builder build road"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, STONE, DOORBUILD, VOID, Serf::BUILDDOOR, "Builder build door 0"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, STONE, DOOR1, VOID, Serf::BUILDDOOR, "Builder build door 1"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, BOARD, DOOR2, VOID, Serf::BUILDDOOR, "Builder build door 2"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, STONE, WALLBUILD, VOID, Serf::BUILDWALL, "Builder build wall"));
  m_instructions.push_back(new InstructionMove(Serf::BUILDER, InstructionMove::AREA, BOARD, FLOORBUILD, VOID, Serf::BUILDFLOOR, "Builder build floor"));
  m_instructions.push_back(new InstructionFindItem(Serf::STONEMASON, VOID, boost::assign::list_of(ROCK1)(ROCK2)(ROCK3), ROCK1, Serf::ACTPREPARE, "Mason get rock"));
  m_instructions.push_back(new InstructionGoHome(Serf::STONEMASON, ROCK1, VOID, Serf::ACTPREPARE, "Mason prepare rock"));
  m_instructions.push_back(new InstructionFindItem(Serf::WOODCUTTER, VOID, boost::assign::list_of(TREE), TREE, Serf::ACTPREPARE, "Woodcutter cut tree"));
  m_instructions.push_back(new InstructionGoHome(Serf::WOODCUTTER, TREE, VOID, Serf::ACTPREPARE, "Woodcutter prepare tree"));
  m_instructions.push_back(new InstructionMove(Serf::GRINDER, InstructionMove::AREA, VOID, GRAIN, GRAIN, Serf::TAKE, "Grinder take grain"));
  m_instructions.push_back(new InstructionGoHome(Serf::GRINDER, GRAIN, VOID, Serf::ACTPREPARE, "Grinder prepare grain"));
  m_instructions.push_back(new InstructionFindItem(Serf::FARMER, VOID, boost::assign::list_of(GRASS), GRAIN, Serf::ACTPREPARE, "Farmer take grass"));
  m_instructions.push_back(new InstructionGoHome(Serf::FARMER, GRAIN, VOID, Serf::ACTPREPARE, "Farmer prepare grass"));
  m_instructions.push_back(new InstructionMove(Serf::WOMAN, InstructionMove::AREA, VOID, FLOUR, FLOUR, Serf::TAKE, "Woman take flour"));
  m_instructions.push_back(new InstructionGoHome(Serf::WOMAN, FLOUR, VOID, Serf::ACTPREPARE, "Woman bear child"));
  m_instructions.push_back(new InstructionMove(Serf::TEACHER, InstructionMove::AREA, VOID, FLOUR, FLOUR, Serf::TAKE, "Teacher take flour"));
  m_instructions.push_back(new InstructionGoHome(Serf::TEACHER, FLOUR, VOID, Serf::ACTPREPARE, "Teacher teach"));
  m_instructions.push_back(new InstructionAnswerRequest(Serf::SERF, VOID, ENDPLAN, Serf::ACTPREPARE, "Serf get education" ));
}

void Planner::itemChanged( const Coord& c, Item oldItem )
{
  m_areaManager.itemChanged( c, oldItem );
  m_changeTargets.add(c);
  m_areaManager.removeBuildAreas( c );
}

void Planner::setTarget( const Coord& c )
{
  assert(!m_targets.has(c));
  m_changeTargets.remove(c);
  m_targets.add(c);
  m_areaManager.targetChanged( c );
}

void Planner::unsetTarget( const Coord& c )
{
  m_targets.remove(c);
  m_areaManager.targetChanged( c );
}

void Planner::newBuilding(const Coord& pos)
{
  for (boost::ptr_vector<Area>::const_iterator it = m_areaManager.getAreas().begin(); it != m_areaManager.getAreas().end(); ++it)
  {
    if ( it->getType() == Serf::BUILDER && it->contains(pos) )
    {
      return;
    }
  }
  Area* area = new Area( *this, pos, Serf::BUILDER );
  m_areaManager.addNewArea( area );
  m_areaManager.doBuildMerge( area );
}

std::auto_ptr<Plan> Planner::makeBestPlan( const Serf& s )
{
  if ( s.getType() == Serf::SERF &&
       s.getLoad() == VOID &&
       m_turnNoSerfPlanFound == Global::turn )
  {
    return std::auto_ptr<Plan>();
  }
  std::auto_ptr<SerialPlan> plan = findBestPlan( s );
  if ( plan->efficiency() > 0 && plan->finalize() )
  {
//     plan->writeToLog();
    return std::auto_ptr<Plan>( plan );
  }
  if ( s.getType() == Serf::SERF &&
       s.getLoad() == VOID )
  {
    m_turnNoSerfPlanFound = Global::turn;
//     Logger::getLogger() << "No best plan in turn " << Global::turn << "\n";
  }
  return std::auto_ptr<Plan>();
}

std::auto_ptr<SerialPlan> Planner::findBestPlan( const Serf& s )
{
  SerialPlan currentPlan( *this, s );
  std::auto_ptr<SerialPlan> bestPlan ( new SerialPlan( currentPlan ) );
  findBestContinuation( currentPlan, bestPlan );
  return bestPlan;
}

void Planner::findBestContinuation( SerialPlan& currentPlan, std::auto_ptr<SerialPlan>& bestPlan )
{
  const unsigned int maxDepth = 2;
  if ( currentPlan.efficiency() > bestPlan->efficiency() )
  {
    bestPlan.reset( new SerialPlan(currentPlan) );
  }
  if ( currentPlan.getTasksSize() >= maxDepth )
  {
    return;
  }
  for (boost::ptr_vector<Instruction>::const_iterator instruction = m_instructions.begin(); instruction != m_instructions.end(); ++instruction)
  {
    boost::ptr_vector<Task> tasks( instruction->makeTasks( *this, currentPlan.getSerf().getType(), currentPlan.getSerf().getOccupies(), currentPlan.carryAfter(), currentPlan.getEnd() ) );
    while ( !tasks.empty() )
    {
      currentPlan.addTask( tasks.pop_back().release() );
      findBestContinuation( currentPlan, bestPlan );
      currentPlan.popTask();
    }
  }
}

void Planner::addRequest( const Request& request )
{
  if ( std::find( m_requests.begin(), m_requests.end(), request ) == m_requests.end() )
  {
    m_requests.push_back( request );
  }
}

const Planner::Request* Planner::findNearestAvailableRequest( Serf::Type type, Item carry, const Coord& start ) const
{
  const Request* best = 0;
  unsigned int bestDistance = 0;
  for ( std::vector<Request>::const_iterator it = m_requests.begin(); it != m_requests.end(); ++it )
  {
    if ( it->type == type && it->carry == carry && !hasTarget( it->pos ) )
    {
      unsigned int distance = start.minDistanceTo( it->pos );
      if ( !best || distance < bestDistance )
      {
        best = &*it;
        bestDistance = distance;
      }
    }
  }
  return best;
}

void Planner::takeRequest( const Request& request )
{
  std::vector<Request>::iterator it = std::find( m_requests.begin(), m_requests.end(), request );
  if ( it != m_requests.end() )
  {
    m_requests.erase( it );
  }
}

// crude - will fix later
void Planner::initAI()
{
  Area* area;
  area = new Area( *this, Coord(0, 0), Serf::SERF );
  area->resizeTo( Rectangle(175, 171, 182, 178 ) );
  m_areaManager.addNewArea( area );
  area = new Area( *this, Coord(0, 0), Serf::SERF );
  area->resizeTo( Rectangle(172, 165, 185, 168 ) );
  m_areaManager.addNewArea( area );
  area = new OccArea( *this, Coord(0, 0), Serf::FARMER );
  area->resizeTo( Rectangle(168, 176, 171, 179 ) );
  m_areaManager.addNewArea( area );
  area = new OccArea( *this, Coord(0, 0), Serf::GRINDER );
  area->resizeTo( Rectangle(175, 182, 178, 185 ) );
  m_areaManager.addNewArea( area );
  Rectangle victoryRect;
  Rectangle world = Field::current()->getInteriorWorldRect();
  for ( Rectangle::Iterator p = world.begin(); p != world.end(); ++p )
  {
    if ( Field::current()->getItem( *p ) == SPECIALFLOOR_START + 1 + m_playerNumber )
    {
      victoryRect.add( *p );
    }
  }
  area = new Area( *this, Coord(0, 0), Serf::SERF );
  area->resizeTo( victoryRect );
  area->setPort( FLOUR, MAXPORT);
  m_areaManager.addNewArea( area );

}

