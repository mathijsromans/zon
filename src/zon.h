#ifndef ZON_H
#define ZON_H

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include "userinterface.h"
class Player;

class Zon
{
  public:
    Zon();
    ~Zon();

    void init();
    void mainLoop();
    void quit();
    int getTick() const { return m_tick; }
    void setSpeed(int speed);
    void speedDown();
    void speedUp();

  private:
    int m_seedrand, m_tick;
    int m_now, m_speed;
    boost::scoped_ptr<UserInterface> m_userInterface;
    boost::ptr_vector<Player> m_players;
    Player* m_me;
    bool m_victoryAchieved;
};

extern Zon zon;

#endif
