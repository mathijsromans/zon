cmake_minimum_required(VERSION 2.8)

PROJECT(zon)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_CXX_COMPILER g++)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -Wextra -Wshadow -O0 -g)

#SET(CMAKE_EXE_LINKER_FLAGS -pg)

#list all source files here
ADD_EXECUTABLE(zon
src/serialplan.cpp
src/logger.cpp
src/instruction.cpp
src/occarea.cpp
src/area.cpp
src/field.cpp
src/global.cpp
src/main.cpp
src/plan.cpp
src/task.cpp
src/player.cpp
src/serf.cpp
src/zon.cpp
src/coord.cpp
src/pathfinder.cpp
src/path.cpp
src/planner.cpp
src/rectangle.cpp
src/userinterface.cpp
src/producearea.cpp
src/areamanager.cpp)

#need to link to some other libraries ? just add them here
target_link_libraries(zon alleg-4.2.2 net boost_signals)

