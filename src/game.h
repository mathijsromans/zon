#ifndef GAME_H
#define GAME_H

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include "userinterface.h"
#include "resources.h"
class Player;

class Game
{
  public:
    Game( const Resources::Level& level, UserInterface& userInterface );
    ~Game();
    void init();
    void mainLoop();
    void quit();
    int getTick() const { return m_tick; }
    void setSpeed(int speed);
    void speedDown();
    void speedUp();

  private:
    void countVictoryPoints();
    void createSerfs();
    Coord getStartPosition( unsigned int i ) const;
    const Resources::Level& m_level;
    int m_seedrand, m_tick;
    int m_now, m_speed;
    UserInterface& m_userInterface;
    boost::ptr_vector<Player> m_players;
    Player* m_me;
    bool m_victoryAchieved;
};

#endif // GAME_H
