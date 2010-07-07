#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "item.h"
#include "fieldmap.h"
#include "coordset.h"
class Field;
class Path;

class PathFinder
{
  public:
    PathFinder(const Field& field) : m_field(field) {}

    /**
     * Find shortest route from start to end
     * If no path is found, an empty path is returned
     * @param start start
     * @param end end
     * @return path resulting path
     */
    Path findPath(const Coord& start, const Coord& end);

    /**
     * Find shortest route from start to item
     * If no path is found, an empty path is returned
     * @param start start
     * @param item item
     * @param doNotUse end cannot be here
     * @param end resulting end
     * @return path resulting path
     */
    Path findPath( const Coord& start, Item item, const CoordSet<MAPWIDTH, MAPHEIGHT>& doNotUse, Coord* end );
    Path findPath( const Coord& start, const std::vector<Item>& items, const CoordSet<MAPWIDTH, MAPHEIGHT>& doNotUse, Coord* end );

  private:
    template<typename Heuristic, typename EndCondition> bool makeMap( const Coord& start, Heuristic heur, EndCondition endCondition, Coord* end );

    /**
     * Create a path from start to end using the calculated map
     * @param start start
     * @param end end
     * @return the path
     */
    Path getPath(const Coord& start, const Coord& end) const;

    FieldMap<unsigned int> g_score; // the actual shortest distance traveled from initial node to current node
    FieldMap<unsigned int> h_score; // the estimated (or "heuristic") distance from current node to goal
    const Field& m_field;
    CoordSet<MAPWIDTH, MAPHEIGHT> m_openSet, m_closedSet;
};

#endif
