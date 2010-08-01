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
    void run();
  private:
    UserInterface m_userInterface;
};

#endif
