#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "serf.h"
#include <string>

struct Instruction
{
  enum TargetType{ ANY, HOME, AREA, AREAPUT, AREATAKE };

  Instruction( Serf::Type setSerfType,
                 TargetType setTargetType,
                 Item setCarryBefore,
                 Item setTargetItem,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription );

  Instruction( Serf::Type setSerfType,
                 Item setCarryBefore,
                 const std::vector<Item>& setTargetItems,
                 Item setCarryAfter,
                 Serf::JobType setJob,
                 std::string setDescription );

  static const std::vector<Instruction>& getInstructions();
  bool iterateOverAreas() const;

  Serf::Type serfType;
  TargetType targetType;
  Item carryBefore;
  Item targetItem;
  std::vector<Item> targetItems;
  Item carryAfter;
  Serf::JobType job;
  std::string description;
};

#endif // INSTRUCTION_H
