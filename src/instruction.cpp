#include "instruction.h"
#include <cassert>
#include <boost/assign.hpp>

Instruction::Instruction( Serf::Type setSerfType,
                 TargetType setTargetType,
                 Item setCarryBefore,
                 Item setTargetItem,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription )
    : serfType(setSerfType),
      targetType(setTargetType),
      carryBefore(setCarryBefore),
      targetItem(setTargetItem),
      targetItems(),
      carryAfter(setCarryAfter),
      job(setJob),
      description(setDescription)
{
  assert( targetType != ANY);
}

Instruction::Instruction( Serf::Type setSerfType,
                 Item setCarryBefore,
                 const std::vector<Item>& setTargetItems,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription )
    : serfType(setSerfType),
      targetType( ANY ),
      carryBefore(setCarryBefore),
      targetItem( VOID ), // should be undefined
      targetItems(setTargetItems),
      carryAfter(setCarryAfter),
      job(setJob),
      description(setDescription)
{
}

const std::vector<Instruction>& Instruction::getInstructions()
{
  static std::vector<Instruction> instructions;
  if ( instructions.empty() )
  {
    for (int i = PORT_START + 1; i < PORT_END; ++i)
    {
      instructions.push_back(Instruction(Serf::SERF, AREATAKE, VOID, static_cast<Item>(i), static_cast<Item>(i), Serf::TAKE, "Serf take item"));
      instructions.push_back(Instruction(Serf::SERF, AREAPUT, static_cast<Item>(i), FLOOR, VOID, Serf::TAKE, "Serf put item"));
    }
    instructions.push_back(Instruction(Serf::BUILDER, AREA, VOID, STONE, STONE, Serf::TAKE, "Builder take stone"));
    instructions.push_back(Instruction(Serf::BUILDER, AREA, VOID, BOARD, BOARD, Serf::TAKE, "Builder take board"));
    instructions.push_back(Instruction(Serf::BUILDER, AREA, STONE, ROADBUILD, VOID, Serf::BUILDROAD, "Builder build road"));
    instructions.push_back(Instruction(Serf::BUILDER, AREA, STONE, DOORBUILD, VOID, Serf::BUILDDOOR, "Builder build door 0"));
    instructions.push_back(Instruction(Serf::BUILDER, AREA, STONE, DOOR1, VOID, Serf::BUILDDOOR, "Builder build door 1"));
    instructions.push_back(Instruction(Serf::BUILDER, AREA, BOARD, DOOR2, VOID, Serf::BUILDDOOR, "Builder build door 2"));
    instructions.push_back(Instruction(Serf::BUILDER, AREA, STONE, WALLBUILD, VOID, Serf::BUILDWALL, "Builder build wall"));
    instructions.push_back(Instruction(Serf::BUILDER, AREA, BOARD, FLOORBUILD, VOID, Serf::BUILDFLOOR, "Builder build floor"));
    instructions.push_back(Instruction(Serf::STONEMASON, VOID, boost::assign::list_of(ROCK1)(ROCK2)(ROCK3), ROCK1, Serf::ACTPREPARE, "Mason get rock"));
    instructions.push_back(Instruction(Serf::STONEMASON, HOME, ROCK1, FLOOR, VOID, Serf::ACTPREPARE, "Mason prepare rock"));
    instructions.push_back(Instruction(Serf::WOODCUTTER, VOID, boost::assign::list_of(TREE), TREE, Serf::ACTPREPARE, "Woodcutter cut tree"));
    instructions.push_back(Instruction(Serf::WOODCUTTER, HOME, TREE, FLOOR, VOID, Serf::ACTPREPARE, "Woodcutter prepare tree"));
    instructions.push_back(Instruction(Serf::GRINDER, AREA, VOID, GRAIN, GRAIN, Serf::TAKE, "Grinder take grain"));
    instructions.push_back(Instruction(Serf::GRINDER, HOME, GRAIN, FLOOR, VOID, Serf::ACTPREPARE, "Grinder prepare grain"));
    instructions.push_back(Instruction(Serf::FARMER, VOID, boost::assign::list_of(GRASS), GRAIN, Serf::ACTPREPARE, "Farmer take grass"));
    instructions.push_back(Instruction(Serf::FARMER, HOME, GRAIN, FLOOR, VOID, Serf::ACTPREPARE, "Farmer prepare grass"));
    instructions.push_back(Instruction(Serf::WOMAN, AREA, VOID, FLOUR, FLOUR, Serf::TAKE, "Woman take flour"));
    instructions.push_back(Instruction(Serf::WOMAN, HOME, FLOUR, FLOOR, VOID, Serf::ACTPREPARE, "Woman bear child"));
    instructions.push_back(Instruction(Serf::TEACHER, AREA, VOID, FLOUR, FLOUR, Serf::TAKE, "Teacher take flour"));
    instructions.push_back(Instruction(Serf::TEACHER, HOME, FLOUR, FLOOR, VOID, Serf::ACTPREPARE, "Teacher teach"));
    instructions.push_back(Instruction(Serf::SERF, AREA, VOID, CLASSROOMFLOOR, ENDPLAN, Serf::ACTPREPARE, "Serf get education"));

//     instructions.push_back(Plan(Serf::TEACHER, ANY2HOME, FLOUR, Serf::ACTPREPARE, FLOUR, FLOOR, Serf::ACTPREPARE));
  }
  return instructions;
}

bool Instruction::iterateOverAreas() const
{
  return targetType == AREA ||
         targetType == AREAPUT ||
         targetType == AREATAKE;
}

