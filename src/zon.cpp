#include "zon.h"
#include "global.h"
#include "game.h"
#include "userinterface.h"
#include "resources.h"
#include <allegro.h>

#define FPS 50

static void timecounter(void)
{
  ++Global::timer;
}
END_OF_FUNCTION(timecounter);

Zon::Zon()
{
  if (allegro_init())
    UserInterface::error("allegro_init failed");
  install_timer();
  LOCK_VARIABLE(timer);
  LOCK_FUNCTION(timecounter);
  install_int_ex(timecounter, BPS_TO_TIMER(FPS));
  install_keyboard();
  if (install_mouse()==-1) 
    UserInterface::error("install_mouse failed");
  if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,1024,768,0,0))
    UserInterface::error ("couldn't get graphics mode with resolution:",640,480);
  Resources::getInstance().load();
  m_userInterface.init();
  show_mouse(screen);
}

Zon::~Zon()
{
}

void Zon::run()
{
  const boost::ptr_vector<Resources::Level>& levels = Resources::getInstance().getLevels();
  for ( unsigned int level = 0; level < levels.size(); ++level)
  {
    Game game( levels[level], m_userInterface );
    game.init();
    game.mainLoop();
  }
}



