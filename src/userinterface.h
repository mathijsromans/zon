#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "coord.h"
#include "item.h"
#include "area.h"
#include "serf.h"
#include <boost/ptr_container/ptr_vector.hpp>

class BITMAP;
class RLE_SPRITE;
class Player;
class Game;

class UserInterface
{
  public:
    enum SpriteSet { SpriteItem, SpriteQItem, SpriteQHouse, SpriteCr };
    UserInterface();
    void setPlayers(Player* player, const boost::ptr_vector<Player>* players);
    ~UserInterface();
    void init();
    void setViewOrigin( const Coord& pos );
    void userInput( Game* zon );
    void drawScreen( unsigned int turn, int tick ) const;
    void message( const std::string& msg );
    void victory( const Player* player );
    static void error(const char* code,int var1=0, int var2=0, int var3=0, int var4=0);
    static void drawSprite(BITMAP *bmp, int x, int y, SpriteSet set, int sort, int n);
    static void drawRect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
    static void drawRectFill(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);
    static void printText(BITMAP *bmp, int x, int y, int color, const char *s);
    static const int PICSZ = 16;
    static const int QPICSZ = PICSZ / 2;
  private:
    void thick_line(BITMAP *bmp, float x, float y, float x_, float y_, float thickness, int color) const;
    void drawSquare(const Coord& c, unsigned int turn) const;
    void setViewPort();
    bool squareof(const Coord& c, Item item1) const;

    Player* m_player;
    const boost::ptr_vector<Player>* m_players;
    const int m_sidescreenWidth;
    static RLE_SPRITE *itempic[N_OF_ITEMS][N_OF_ITPIC], *crpic[N_OF_SPECIES][8 * Serf::N_TYPES], *qitempic[N_OF_ITEMS][4],*arrowin,*arrowout,*qhousepic[10][4];
    Item m_mousemode;
    bool m_togglemode, m_lmbPressed, m_rmbPressed, m_keywaitforrelease;
    int m_keypressedwhen;
    BITMAP *m_mainscreen, *m_sidescreen;
    Area* m_area;
    Area::Mode m_myareamode, m_areamode;
    Coord m_moveFrom, m_viewOrigin, m_mousePos, m_fixCorner;
};

#endif
