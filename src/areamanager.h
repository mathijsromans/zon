#ifndef AREAMANAGER_H
#define AREAMANAGER_H

#include "item.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/array.hpp>
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
     */
    void addNewArea(Area* area);

    /**
     * Delete an area
     */
    void removeArea(Area* area);

    boost::ptr_vector<Area>& getAreas() { return m_allAreas; }
    const boost::ptr_vector<Area>& getAreas() const { return m_allAreas; }

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

    void itemChanged( const Coord& c, Item oldItem );
    void targetChanged( const Coord& c );

    void addToAvailable( Item item, int diff );
    void addToAvailable( const boost::array<int, N_OF_ITEMS>& diff );
    int getAvailable( Item item ) const { return m_available[item]; }
    int getAvailableTotal() const { return m_availableTotal; }

  private:
    boost::ptr_vector<Area> m_allAreas;
    std::vector<OccArea*> m_occAreas;
    boost::array<int, N_OF_ITEMS> m_available;
    int m_availableTotal;
};

#endif
