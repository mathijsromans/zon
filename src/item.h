#ifndef ITEM_H
#define ITEM_H

enum Item {
  VOID = 0,
  SUPERROCK = 1,
  RIVER = 2,
  SHIP = 3,
  WATERPLANT = 4,
  TREE = 5,
  FIRE = 7,
  GRASS = 8,
  ROCK3 = 9,
  ROCK2 = 10,
  ROCK1 = 11,
  WALL = 12,
  FLOOR = 13,
  DOOR = 14,
  ROAD = 15,
  DOOR1 = 16,
  DOOR2 = 17,
  UNSAFEFLOOR = 18,
  UNSAFEFLOORCHECK = 19,
  CLASSROOMFLOOR = 20,
  ENDPLAN = 21,
  FORD1 = 33,
  FORD2 = 34,
  TERRAIN_END,
  BUILD_START,
  WALLBUILD,
  FLOORBUILD,
  DOORBUILD,
  ROADBUILD,
  BUILD_END,
  PORT_START=67,
  BOARD,
  STONE,
  GRAIN,
  GRANDFATHER,
  FLOUR,
  PORT_END,
  LOGO_START,
  LOGO,
  LOGO_END,
  N_OF_ITEMS
};

inline bool equiv(Item item2, Item item1)
{
  if (item1==item2)
    return true;
  if (item1==VOID && item2 > BUILD_START && item2 < BUILD_END)
    return true;
  if (item1==WALL && item2==DOOR)
    return true;
  if (item1==FLOOR && (item2==UNSAFEFLOOR || (item2 > PORT_START && item2 < PORT_END)))
    return true;
  if (item1==DOOR1 && (item2==DOOR2 || (item2 > BUILD_START && item2 < BUILD_END)))
    return true;
  if (item1==DOOR2 && (item2==DOOR1 || item2==DOOR || item2==WALL || item2==WALLBUILD || item2==DOORBUILD))
    return true;
  if (item1==ROCK1 && (item2==ROCK2 || item2==ROCK3))
    return true;
  if (item1==RIVER && (item2==FORD1 || item2==FORD2 || item2==SHIP))
    return true;
  return false;
}

#endif