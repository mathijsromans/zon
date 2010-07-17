#include "userinterface.h"

#include <cmath>
#include <allegro.h>
#include <list>
#include <sstream>
#include "producearea.h"
#include "areamanager.h"
#include "global.h"
#include "serf.h"
#include "field.h"
#include "player.h"
#include "path.h"
#include "zon.h"

RLE_SPRITE *UserInterface::itempic[N_OF_ITEMS][N_OF_ITPIC], *UserInterface::crpic[N_OF_SPECIES][8 * Serf::N_TYPES],
*UserInterface::qitempic[N_OF_ITEMS][4],*UserInterface::arrowin,*UserInterface::arrowout,*UserInterface::qhousepic[10][4];

inline std::string stringify(int x)
{
  std::ostringstream o;
  o << x;
  return o.str();
} 

UserInterface::UserInterface(Player& player) :
  m_player(player),
  m_sidescreenWidth ( 112 ),
  m_mousemode( VOID ),
  m_togglemode( false ),
  m_lmbPressed( false ),
  m_rmbPressed( false ),
  m_keywaitforrelease( false ),
  m_keypressedwhen( 0 ),
  m_area( 0 )
{
}

UserInterface::~UserInterface()
{
}

void UserInterface::init( const Coord& startPosition )
{
  m_viewOrigin = startPosition;
  if (allegro_init()) 
    error("allegro_init failed");
  install_timer();
  LOCK_VARIABLE(timer);
  LOCK_FUNCTION(timecounter);
  install_keyboard();
  if (install_mouse()==-1) 
    error("install_mouse failed");
  if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,1024,768,0,0))
    error ("couldn't get graphics mode with resolution:",640,480);
  loadImages();
  show_mouse(screen);
//   if (alert("Welcome to The Clicking Game", "",  "Do you want to", "play &Single-Player", "play &Multi-Player", 's', 'm')==2)
//     multiplayerinit();
//   else
  show_mouse(0);
}

void UserInterface::loadImages()
{
  int x,y;
  BITMAP *bmp,*one_pic, *qone_pic, *big;
  RGB my_palette[256];
  if (!(bmp=load_bmp("resources/map.bmp", my_palette)))
    error ("couldn't find the file 'map.bmp', you idiot!");
  for (y=0;y<bmp->h;y++)
    for (x=0;x<bmp->w;x++)
      field.setItem(Coord(x, y), static_cast<Item>(bmp->line[y][x]));
  destroy_bitmap(bmp);
  one_pic=create_bitmap(PICSZ,PICSZ);
  qone_pic=create_bitmap(QPICSZ,QPICSZ);
  if (!(bmp=load_bmp("resources/crpic.bmp", my_palette)))
    error ("couldn't find the file 'crpic.bmp'");
  for (x=0;x<N_OF_SPECIES;x++)
    for (y=0;y<8 * Serf::N_TYPES;y++) 
  {
    blit(bmp, one_pic, x*PICSZ, y*PICSZ, 0, 0, PICSZ, PICSZ);
    crpic[x][y]=get_rle_sprite(one_pic);
  }
  destroy_bitmap(bmp);
  big=create_bitmap(N_OF_ITEMS*PICSZ,N_OF_ITPIC*PICSZ);
  if (!(bmp=load_bmp("resources/terpic.bmp", my_palette)))
    error ("couldn't find the file 'terpic.bmp'");
  blit (bmp, big, 0,0,0,0,TERRAIN_END*PICSZ,N_OF_ITPIC*PICSZ);
  destroy_bitmap(bmp);
  if (!(bmp=load_bmp("resources/buildpic.bmp", my_palette)))
    error ("couldn't find the file 'buildpic.bmp'");
  blit (bmp, big, 0,0,(BUILD_START+1)*PICSZ, 0, (BUILD_END - BUILD_START - 1)*PICSZ,N_OF_ITPIC*PICSZ);
  destroy_bitmap(bmp);
  if (!(bmp=load_bmp("resources/portpic.bmp", my_palette)))
    error ("couldn't find the file 'portpic.bmp'");
  blit (bmp, big, 0,0,(PORT_START+1)*PICSZ,0,(PORT_END - PORT_START - 1)*PICSZ,N_OF_ITPIC*PICSZ);
  destroy_bitmap(bmp);
  if (!(bmp=load_bmp("resources/logopic.bmp", my_palette)))
    error ("couldn't find the file 'logopic.bmp'");
  blit (bmp, big, 0,0,(LOGO_START+1)*PICSZ,0,(LOGO_END - LOGO_START - 1)*PICSZ,N_OF_ITPIC*PICSZ);
  destroy_bitmap(bmp);  
  for (x=0;x<N_OF_ITEMS;x++) 
  {
    for (y=0;y<N_OF_ITPIC;y++) 
    {
      blit(big, one_pic, x*PICSZ, y*PICSZ, 0, 0, PICSZ, PICSZ);
      itempic[x][y]=get_rle_sprite(one_pic);
    }
    blit(big, one_pic, x*PICSZ, PICSZ, 0, 0, PICSZ, PICSZ);
    for (y=0;y<4;y++) 
    {
      blit(one_pic,qone_pic,QPICSZ*(y%2), QPICSZ*(y/2),0,0,QPICSZ,QPICSZ);
      qitempic[x][y]=get_rle_sprite(qone_pic);
    }
  }
  for (x=0;x<10;x++)
    for (y=0;y<4;y++)
  {
    blit(big, qone_pic, PICSZ*(WALL+x/5)+QPICSZ*(1-((y+1)/2)%2),PICSZ*(1+x%5)+QPICSZ*(1-y/2),0,0,QPICSZ,QPICSZ);
    qhousepic[x][y]=get_rle_sprite(qone_pic);
  }
  destroy_bitmap(qone_pic);
  destroy_bitmap(one_pic);
  destroy_bitmap(big);
  set_palette(my_palette);
  RGB black = { 0,  0,  0  };
  set_color(0,&black);
  gui_bg_color=64;
  m_mainscreen=create_bitmap(SCREEN_W-m_sidescreenWidth,SCREEN_H);
  m_sidescreen=create_bitmap(m_sidescreenWidth,SCREEN_H);
  for (x=0;x<MAPWIDTH;x++)
  {
    field.setItem(Coord(x, 0            ), SUPERROCK);
    field.setItem(Coord(x, MAPHEIGHT - 1), SUPERROCK);
  }
  for (y=0;y<MAPHEIGHT;y++) 
  {
    field.setItem(Coord(0,            y), SUPERROCK);
    field.setItem(Coord(MAPWIDTH - 1, y), SUPERROCK);
  }
}

void UserInterface::drawSquare(const Coord& c, unsigned int turn) const
{
  Item item = field.getItem(c);
  Coord pix = (c - m_viewOrigin) * PICSZ;
  if (item < WALL)
  {
    drawSprite(m_mainscreen, pix.x, pix.y, SpriteItem, item, 0);
    //draw_rle_sprite(m_mainscreen,itempic[item][0],pix.x,pix.y);
    if ((item!=VOID)&&(item!=SUPERROCK))
    {
      for (int d = 0; d < 4; ++d)
      {
        if (squareof(c + Coord((d&1) - 1, (d/2) - 1), item))
        {
          drawSprite(m_mainscreen, pix.x+QPICSZ*(d&1), pix.y+QPICSZ*(d/2), SpriteQItem, item, d);
        }
      }
    }
  }
  else if (item==WALL)
  {
    for (int d = 0; d < 8; d += 2)
    {
      int sort;;
      if (equiv(field.getItem(c.next(d)),DOOR2) && equiv(field.getItem(c.next(d + 1)),DOOR2) && equiv(field.getItem(c.next(d + 2)),DOOR2))
      {
        sort = 4;
      }
      else
      {
        sort = 5*(equiv(field.getItem(c.next(d)),FLOOR) || equiv(field.getItem(c.next(d + 1)),FLOOR) || equiv(field.getItem(c.next(d + 2)),FLOOR))+2*(1-equiv(field.getItem(c.next(d + 2)),DOOR2))+(1-equiv(field.getItem(c.next(d)),DOOR2));
      }
      drawSprite(m_mainscreen, pix.x+QPICSZ*(d==0 || d==6), pix.y+QPICSZ*(d==0 || d==2), SpriteQHouse, sort, d / 2);
    }
  }
  else if (item==DOOR||item==DOOR1||item==DOOR2)
  {
    drawSprite(m_mainscreen, pix.x, pix.y, SpriteItem, item, (Serf::getSerf(c) != 0 ? 1 : 0)+2*(equiv(field.getItem(c.next(2)),DOOR2)));
  }
  else 
  {
    drawSprite(m_mainscreen, pix.x, pix.y, SpriteItem, item, 0);
  }
  if (item==RIVER && ((turn/2+c.x)&1))
  {
    int d;
    for (d=0;d<8;d++)
      if (!equiv(field.getItem(c.next(d)),RIVER))
        break;
    if (d==8)
    {
      drawSprite(m_mainscreen, pix.x, pix.y, SpriteItem, RIVER, 3);
    }
  }
  if (m_togglemode)
  {
    const Planner& planner = m_player.getPlanner();
    if ( planner.hasChangeTarget( c ) )
    {
      rectfill(m_mainscreen,pix.x+4,pix.y+6,pix.x+10,pix.y+12,13);
    }
    if ( planner.hasTarget( c ) )
    {
      rectfill(m_mainscreen,pix.x+4,pix.y+6,pix.x+9,pix.y+12,15);
    }
  }
}

void UserInterface::drawScreen( unsigned int turn, int tick ) const
{
  clear_to_color(m_sidescreen, 68);
  rect(m_sidescreen,0,0,m_sidescreen->w - 1,m_sidescreen->h-1,13);
  Rectangle viewRect(m_viewOrigin, m_viewOrigin + Coord( (SCREEN_W-m_sidescreenWidth)/PICSZ,SCREEN_H/PICSZ) );
  for (Rectangle::Iterator posIt = viewRect.begin(); posIt != viewRect.end(); ++posIt)
  {
    drawSquare(*posIt, turn);
  }
  for (Rectangle::Iterator posIt = viewRect.begin(); posIt != viewRect.end(); ++posIt)
  {
    const Serf *s = Serf::getSerf(*posIt);
    if (s)
    {
      s->draw(m_mainscreen,(posIt->x - m_viewOrigin.x)*PICSZ+s->drawOffsetX(tick),(posIt->y - m_viewOrigin.y)*PICSZ+s->drawOffsetY(tick), tick);
    }
  }
  if (m_togglemode)
  {
    for (std::vector<Serf*>::const_iterator it = m_player.serfBegin(); it != m_player.serfEnd(); ++it)
    {
      Serf *s = *it;
      const Path* path = s->getPath();
      if (path && path->getBoundingBox().intersects(viewRect))
      {
        unsigned long identity = reinterpret_cast<unsigned long>(s);
        int offset = identity % 7 - 3;
        RGB rgb = default_palette[ identity % 256];
        const int color = makecol(4 * rgb.r, 4 * rgb.g, 4 * rgb.b);
        const float thickness = 1;
        const std::vector<Direction>& dirs= path->getDirs();
        Coord pos = path->getStart() - m_viewOrigin;
        for (std::vector<Direction>::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
        {
          Coord newPos = pos.next(*dir);
          thick_line(m_mainscreen,    pos.x * PICSZ + PICSZ / 2 + offset,    pos.y * PICSZ + PICSZ / 2 + offset,
                     newPos.x * PICSZ + PICSZ / 2 + offset, newPos.y * PICSZ + PICSZ / 2 + offset, thickness, color);
          pos = newPos;
        }
      }
    }
  }
  const boost::ptr_list<Area>& areas = m_player.getPlanner().getAreaManager().getAreas();
  for (boost::ptr_list<Area>::const_iterator it = areas.begin(); it != areas.end(); ++it)
  {
    const Area *a = &*it;
    if ( m_togglemode || a->getType() != Serf::BUILDER )
    {
      a->draw(m_mainscreen, PICSZ, m_viewOrigin, a == m_area );
    }
  }
  if (m_area)
  {
    m_area->drawInfo(m_sidescreen, PICSZ, m_player.getNumber());
  }
  printText(m_sidescreen,3,15,1, ( std::string("Serfs:") + stringify(m_player.nSerf())).c_str() );
  printText(m_sidescreen,3,25,1, ( std::string("Turn:") + stringify(Global::turn)).c_str() );
  OccArea* occArea = dynamic_cast<OccArea*>( m_area );
  for ( int i = Serf::STONEMASON; i < Serf::N_TYPES; i++)
  {
    if ( !occArea || occArea->getType() != i || !occArea->getOccupant() )
    {
      drawSprite(m_sidescreen, (m_sidescreenWidth-3*PICSZ)/2+PICSZ*((i-2)%3),SCREEN_H-140+((i-2)/3)*PICSZ, SpriteCr, 10+11*(m_player.getNumber()%3), 8*i);
//       draw_rle_sprite(m_sidescreen,crpic[10+11*(me.getNumber()%3)][8*i],(m_sidescreenWidth-3*PICSZ)/2+PICSZ*((i-2)%3),SCREEN_H-140+((i-2)/3)*PICSZ);
    }
  }
  if ( occArea )
  {
    rect(m_sidescreen,(m_sidescreenWidth-3*PICSZ)/2+PICSZ*((occArea->getType()-2)%3)-1,SCREEN_H-140+((occArea->getType()-2)/3)*PICSZ-1,(m_sidescreenWidth-3*PICSZ)/2+PICSZ*((occArea->getType()-2)%3)+PICSZ,SCREEN_H-140+((occArea->getType()-2)/3)*PICSZ+PICSZ,5);
    if (occArea->getOccupant())
    {
      drawSprite(m_sidescreen, (m_sidescreenWidth-3*PICSZ)/2+PICSZ*((occArea->getType()-2)%3),SCREEN_H-140+((occArea->getType()-2)/3)*PICSZ, SpriteCr,10+11*(m_player.getNumber()%3), ((tick+16*turn)/8&7)+8*occArea->getType());
  //       draw_rle_sprite(m_sidescreen,crpic[10+11*(me.getNumber()%3)][((tick+16*turn)/8&7)+8*m_area->getType()],(m_sidescreenWidth-3*PICSZ)/2+PICSZ*((m_area->getType()-2)%3),SCREEN_H-140+((m_area->getType()-2)/3)*PICSZ);
    }
  }
  drawSprite(m_sidescreen, 5,SCREEN_H-50, SpriteItem, VOID, 0);
//   draw_rle_sprite(m_sidescreen,itempic[VOID][0],5,SCREEN_H-50);
  for ( int i = 0; i < BUILD_END - BUILD_START - 1; ++i)
  {
    drawSprite(m_sidescreen, 5+(i+1)*PICSZ,SCREEN_H-50, SpriteItem, WALL + i, 0);
//     draw_rle_sprite(m_sidescreen,itempic[WALL + i][0],5+(i+1)*PICSZ,SCREEN_H-50);
  }
  if ( m_mousemode != VOID )
  {
    rect(m_sidescreen,5+(m_mousemode-BUILD_START)*PICSZ-1,SCREEN_H-51,5+(m_mousemode-BUILD_START+1)*PICSZ,SCREEN_H-50+PICSZ,5);
    if (mouse_x<m_mainscreen->w)
    {
      int x = mouse_x/PICSZ;
      int y = mouse_y/PICSZ;
      if (m_mousemode==BUILD_START)
      {
        drawSprite(m_mainscreen, x*PICSZ,y*PICSZ, SpriteItem, VOID, 0);
//         draw_rle_sprite(m_mainscreen,itempic[VOID][0],x*PICSZ,y*PICSZ);
      }
      else
      {
        drawSprite(m_mainscreen, x*PICSZ,y*PICSZ, SpriteItem, m_mousemode-BUILD_START - 1 + WALL, 0);
//         draw_rle_sprite(m_mainscreen,itempic[m_mousemode-BUILD_START - 1 + WALL][0],x*PICSZ,y*PICSZ);
      }
      rect(m_mainscreen,x*PICSZ,y*PICSZ,(x+1)*PICSZ,(y+1)*PICSZ,30);
    }
  }
  show_mouse(0);
  blit(m_mainscreen,screen,0,0,0,0,SCREEN_W-m_sidescreenWidth,SCREEN_H);
  blit(m_sidescreen,screen,0,0,SCREEN_W-m_sidescreenWidth,0,m_sidescreenWidth,SCREEN_H);
  show_mouse(screen);
}

void UserInterface::thick_line(BITMAP *bmp, float x, float y, float x_, float y_, float thickness, int color) const
{
  float dx = x - x_;
  float dy = y - y_;
  float d = sqrt(dx * dx + dy * dy);
  if (!d)
    return;
 
  int v[4 * 2];
 
  /* left up */
  v[0] = x - thickness * dy / d;
  v[1] = y + thickness * dx / d;
  /* right up */
  v[2] = x + thickness * dy / d;
  v[3] = y - thickness * dx / d;
  /* right down */
  v[4] = x_ + thickness * dy / d;
  v[5] = y_ - thickness * dx / d;
  /* left down */
  v[6] = x_ - thickness * dy / d;
  v[7] = y_ + thickness * dx / d;
 
  polygon(bmp, 4, v, color);
}

void UserInterface::setViewPort()
{
  Coord oldViewOrigin = m_viewOrigin;
  Coord relMousePos( mouse_x / PICSZ, mouse_y / PICSZ );
  m_mousePos = relMousePos + m_viewOrigin;

  if ( (mouse_b&2) )
  {
    if ( !m_rmbPressed )
    {
      m_rmbPressed = true;
      m_moveFrom = m_mousePos;
    }
    else
    {
      m_viewOrigin = m_moveFrom - relMousePos;
    }
  }
  else if ( !(mouse_b&2) )
  {
    m_rmbPressed = false;
  }
  m_viewOrigin = Rectangle( 0, 0, MAPWIDTH-(SCREEN_W-m_sidescreenWidth)/PICSZ + 1, MAPHEIGHT-SCREEN_H/PICSZ + 1).project( m_viewOrigin );
  m_mousePos = relMousePos + m_viewOrigin;
}

void UserInterface::userInput( Zon* theZon )
{
  if (m_keypressedwhen && m_keywaitforrelease && Global::timer > m_keypressedwhen + 10)
  {
    m_keywaitforrelease = false;
    m_keypressedwhen=0;
  }
  if (!m_keywaitforrelease)
  {
    if (key[KEY_Q])
    {
      if (alert("Do you want to exit this program?","","", "Continue", "Discontinue", 'c', 'd')==1)
        alert("Do you mean","continue quitting","or continue playing?","Ok", "Cancel",'o', 'c');
      else
        alert("Do you mean","discontinue quitting","or discontinue playing?","Ok", "Cancel",'o', 'c');
      alert("Sorry","but I don't understand you", "Try again later","Ok", "Cancel",'o', 'c');
    }
    if (key[KEY_PLUS_PAD]) theZon->speedUp();
    if (key[KEY_MINUS_PAD]) theZon->speedDown();
    if (key[KEY_ESC]) theZon->quit();
    if (key[KEY_T]) m_togglemode = !m_togglemode;
  }
  m_keywaitforrelease=key[KEY_Q]||key[KEY_PLUS_PAD]||key[KEY_MINUS_PAD]||key[KEY_T];

  if (key[KEY_DEL] && m_area)
  {
    
    m_player.getPlanner().getAreaManager().removeArea( m_area );
    m_area = 0;
  }

  if ((mouse_b&1) && !m_lmbPressed && m_areamode==Area::FLOAT)
  {
    m_areamode=Area::Area::NOTHING;
    m_lmbPressed = true;
  }

  setViewPort();
  if ( mouse_b&1 )
  {
    if ( m_area && m_areamode==Area::DRAG )
    {
      m_area->moveTo(m_mousePos);
    }
    else if ( m_area && m_areamode==Area::RESIZE )
    {
      m_area->resizeTo(m_mousePos, m_fixCorner);
    }
    else if (mouse_x < SCREEN_W-m_sidescreenWidth)
    {
      if (m_mousemode != VOID)
      {
        if ((Item) m_mousemode > BUILD_START && equiv(field.getItem(m_mousePos),VOID))
        {
          field.setItem(m_mousePos, m_mousemode);
          m_player.getPlanner().newBuilding(m_mousePos);
        }
        else if ((Item) m_mousemode == BUILD_START && field.getItem(m_mousePos) > BUILD_START && field.getItem(m_mousePos) < BUILD_END)
        {
          field.setItem(m_mousePos, VOID);
          m_player.getPlanner().getAreaManager().removeBuildAreas(m_mousePos);
        }
      }
      else
      {
        m_area=0;
        boost::ptr_list<Area>& areas = m_player.getPlanner().getAreaManager().getAreas();
        for (boost::ptr_list<Area>::iterator area = areas.begin(); area != areas.end(); ++area)
        {
          m_areamode = area->areaClick(m_mousePos);
          if ( m_areamode != Area::NOTHING)
          {
            m_area = &*area;
            m_fixCorner = m_area->getTopLeft();
            break;
          }
        }
        if (!m_area)
        {
          m_area = m_player.getPlanner().getAreaManager().addNewArea( new Area( m_player.getPlanner(), m_mousePos, Serf::SERF) );
          m_fixCorner = m_area->getTopLeft();
          m_areamode=Area::RESIZE;
        }
      }
    }
    else if ( !m_lmbPressed )
    {
      m_lmbPressed = true;
      if (mouse_x>=SCREEN_W-m_sidescreenWidth+5 && mouse_y >=SCREEN_H-50 && mouse_y<SCREEN_H-50+PICSZ)
      {
        Item pushed = Item((mouse_x-(SCREEN_W-m_sidescreenWidth+5))/PICSZ+BUILD_START);
        if (pushed >= BUILD_START && pushed < BUILD_END)
        {
          if (m_mousemode == pushed)
          {
            m_mousemode = VOID;
          }
          else
          {
            m_mousemode = pushed;
          }
        }
      }
      else if (mouse_y>=SCREEN_H-140)
      {
        Serf::Type pushed = Serf::Type(((mouse_y-SCREEN_H+140)/PICSZ)*3+(mouse_x-SCREEN_W+m_sidescreenWidth-(m_sidescreenWidth-3*PICSZ)/2)/PICSZ+2);
        if (pushed > Serf::BUILDER && pushed < Serf::N_TYPES)
        {
          OccArea* area;
          if ( pushed == Serf::TEACHER )
          {
            area = new ProduceArea( m_player.getPlanner(), m_viewOrigin, pushed );
          }
          else
          {
            area = new OccArea( m_player.getPlanner(), m_viewOrigin, pushed );
          }
          m_area = m_player.getPlanner().getAreaManager().addNewArea( area );
          m_area->resize(3, 3);
          m_fixCorner = m_viewOrigin;
          m_areamode = Area::FLOAT;
        }
      }
      else if (m_area)
      {
        m_area->infoClick(Coord(mouse_x-SCREEN_W+m_sidescreenWidth,mouse_y), PICSZ);
      }
    }
  }
  else
  {
    m_lmbPressed = false;
    if (m_area&&m_areamode==Area::FLOAT&&mouse_x<SCREEN_W-m_sidescreenWidth) 
      m_area->moveTo(m_mousePos);
    if ( m_areamode == Area::DRAG || m_areamode == Area::RESIZE )
      m_areamode = Area::NOTHING;
  }
}

bool UserInterface::squareof(const Coord& c, Item item1) const
{
  if (field.getItem(c)==RIVER || field.getItem(c)==FORD1 || field.getItem(c)==FORD2 || field.getItem(c)==SHIP)
  {
    return equiv(field.getItem(c),RIVER)&&equiv(field.getItem(Coord(c.x + 1, c.y)),RIVER)&&equiv(field.getItem(Coord(c.x, c.y + 1)),RIVER)&&equiv(field.getItem(Coord(c.x + 1, c.y + 1)),RIVER);
  }
  return field.getItem(c)==item1&&field.getItem(Coord(c.x + 1, c.y))==item1&&field.getItem(Coord(c.x, c.y + 1))==item1&&field.getItem(Coord(c.x + 1, c.y + 1))==item1;
}

void UserInterface::error(const char* code,int var1, int var2, int var3, int var4)
{
  set_gfx_mode(GFX_TEXT,0,0,0,0);
  if (!var1)
    allegro_message("\nDamn! An error has occurred:\n%s\n\n", code);
  else if (!var2)
    allegro_message("\nDamn! An error has occurred:\n%s\n%i\n",code,var1);
  else if (!var3)
    allegro_message("\nDamn! An error has occurred:\n%s\n%i %i\n",code,var1,var2);
  else if (!var4)
    allegro_message("\nDamn! An error has occurred:\n%s\n%i %i %i\n",code,var1,var2,var3);
  else
    allegro_message("\nDamn! An error has occurred:\n%s\n%i %i %i %i\n",code,var1,var2,var3,var4);
  zon.quit();
}

void UserInterface::drawSprite(BITMAP *bmp, int x, int y, SpriteSet set, int sort, int n)
{
  switch ( set )
  {
    case SpriteItem:
      draw_rle_sprite(bmp, itempic[sort][n], x, y);
      break;
    case SpriteQItem:
      draw_rle_sprite(bmp, qitempic[sort][n], x, y);
      break;
    case SpriteQHouse:
      draw_rle_sprite(bmp, qhousepic[sort][n], x, y);
      break;
    case SpriteCr:
      draw_rle_sprite(bmp, crpic[sort][n], x, y);
      break;
    default:
      assert(false);
      break;
  }
}

void UserInterface::drawRect(BITMAP *bmp, int x1, int y1, int x2, int y2, int color)
{
  rect( bmp, x1, y1, x2, y2, color );
}

void UserInterface::drawRectFill(BITMAP *bmp, int x1, int y1, int x2, int y2, int color)
{
  rectfill( bmp, x1, y1, x2, y2, color );
}

void UserInterface::printText(BITMAP *bmp, int x, int y, int color, const char *s)
{
  textout_ex(bmp, font, s, x, y, color, 68); 
}