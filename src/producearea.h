#ifndef PRODUCEAREA_H
#define PRODUCEAREA_H

#include "occarea.h"
#include "serf.h"

class ProduceArea : public OccArea
{
  public:
    ProduceArea( Planner& planner, const Coord& c, Serf::Type type );
    ~ProduceArea();
    Serf::Type shiftProduce();
    virtual void drawInfo(BITMAP* sidescreen, int PICSZ, int myPlayerNumber) const;
    virtual void infoClick(const Coord& pix, int PICSZ);
    virtual bool isActive() const { return m_produce[0] != Serf::SERF; }
  private:
    static const int MAXPRODUCE = 5;
    Serf::Type m_produce[MAXPRODUCE];
};

#endif
