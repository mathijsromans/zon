#ifndef AREAMANAGER_H
#define AREAMANAGER_H

#include <boost/ptr_container/ptr_list.hpp>
#include <vector>

class Coord;
class Area;
class OccArea;

class AreaManager : boost::noncopyable
{
  public:

    AreaManager();
    ~AreaManager();

    /**
     * Register area and transfer ownership
     * @param area the area
     * @return pointer to area
     */
    Area* addNewArea(Area* area);

    /**
     * Delete an area
     */
    void removeArea(Area* area);

    boost::ptr_list<Area>& getAreas() { return m_allAreas; }

    std::vector<OccArea*>& getOccAreas() { return m_occAreas; }

    /**
     * Remove all building areas that contain c that are no longer needed
     * @param c coordinate
     */
    void removeBuildAreas(const Coord& c);

    /**
     * Merge building areas if convenient
     * @param area recently made new building area
     */
    void doBuildMerge(Area* area);

  private:
    boost::ptr_list<Area> m_allAreas;
    std::vector<OccArea*> m_occAreas;
};

#endif
