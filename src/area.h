#ifndef ZONAREA_H
#define ZONAREA_H

#include "rectangle.h"
#include "config.h"
#include "serf.h"
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/signals.hpp>

class Planner;

class Area : public Rectangle, public boost::noncopyable
{
  public:

  enum Mode {NOTHING, DRAG, RESIZE, FLOAT };

  /**
   * Constructor for area only containing c
   * @param c coordinate
   */
  Area( Planner& planner, const Coord& c, Serf::Type type );

  /**
   * Destructor
   */
  virtual ~Area();

  /**
   * Move the area
   * @param c new left top
   */
  void moveTo(Coord c);

  /**
   * Resize the area
   * @param width new width
   * @param height new height
   */
  void resize(int width, int height);

  /**
   * Resize the area
   * @param c new coordinate
   * @param fix coordinate to fix
   */
  void resizeTo(Coord c, Coord fix);

  /**
   * Make new size a valid one
   * Returns false if there is no change
   * @param newRect the new size
   */
  bool checkNewRect( Rectangle& newRect) const;

  /**
   * Set this area to a rectangle
   * @param newRect the new size
   */
  virtual void resizeTo(const Rectangle& newRect);

  void itemChanged( const Coord& c );

  void targetChanged( const Coord& c, bool set );

  /**
   * Draw the area
   * @param mainscreen screen to draw in
   * @param PICSZ picture size
   * @param viewOrigin origin of the screen
   * @param isSelected is this area selected
   */
  virtual void draw(BITMAP* mainscreen, int PICSZ, const Coord& viewOrigin, bool isSelected) const;

  /**
   * Draw information about the area
   * @param sidescreen screen to draw in
   * @param PICSZ picture size
   * @param myPlayerNumber player number
   */
  virtual void drawInfo(BITMAP* sidescreen, int PICSZ, int myPlayerNumber) const;

  /**
   * Find nearest item that is not a target
   * @param from from this point
   * @param load the item
   * @return a coordinate in the area
   */
  Coord findNearestItem (const Coord& from, Item load) const;

  /**
   * What happens if you click on this area
   * @param c the coordinate that is clicked
   * @return the new mode, NORMAL if nothing happens
   */
  Mode areaClick(const Coord& c) const;

  /**
   * Click on the info screen
   * @param pix clicked pixel
   * @param PICSZ picture size
   */
  virtual void infoClick(const Coord& pix, int PICSZ);

  /**
   * Count number of available items
   */
  void countAvailable();

  bool hasAvailable(Item load) const { return m_available[load] != 0; }
  int getAvailable(Item load) const { return m_available[load]; }
  void decreaseAvailable(Item load) { --m_available[load]; }
  void increaseAvailable(Item load) { ++m_available[load]; }

  Serf::Type getType() const { return m_type; }
 
  int getPort( Item item ) const { return m_port[item - PORT_START - 1]; }

  virtual bool isActive() const { return true; }

  /**
   * This area can be used by serf for a plan
   * @param serfType type of serf
   * @return true or false
   */
  bool canBeUsedBy(Serf::Type serfType) const;

  /**
   * Whether there are building activities in this area
   */
  bool hasBuilding() const;

  protected:
    void setPort( Item item, int value ) { m_port[item - PORT_START - 1] = value; }

  private:
    const Area& operator=(const Area& rect);
    Planner& m_planner;
    boost::array<int, N_OF_ITEMS> m_available;
    boost::array<int, PORT_END - PORT_START - 1> m_port;
    const Serf::Type m_type;
    boost::signals::scoped_connection m_con1, m_con2;
};

#endif
