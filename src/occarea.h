#ifndef OCCAREA_H
#define OCCAREA_H

#include "area.h"

class OccArea : public Area
{
  public:
    OccArea( Planner& planner, const Coord& c, Serf::Type type);
    ~OccArea();
    Serf* getOccupant() const { return m_occupant; }
    void setOccupant(Serf* occupant) { m_occupant = occupant; }
    void resetOccupant();

  /**
   * Set this area to a rectangle
   * @param rect the rectangle
   */
  virtual void resizeTo(const Rectangle& newRect);

  virtual void draw(BITMAP* mainscreen, int PICSZ, const Coord& viewOrigin, bool isSelected) const;

  /**
   * Is the area filled with empty floor
   * @return true if it is clear
   */
  bool hasFreeFloor() const;

  /**
   * Can this area accomodate a serf
   * @return true or false
   */
  bool isFreeBuilding() const;

  private:
    Serf *m_occupant;
};

#endif // OCCAREA_H
