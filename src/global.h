#ifndef ZONGLOBAL_H
#define ZONGLOBAL_H

#include <stdexcept>
#include <string>

class ZonException : public std::runtime_error
{
  public:
    ZonException( const std::string& msg ) : std::runtime_error(msg) {}
};

int randomNum(int max);



namespace Global
{
  extern volatile int timer;
  extern unsigned int turn;
}

#endif