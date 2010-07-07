#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

class Logger
{
  public:
    static std::ostream& getLogger() { return std::cerr; }
};

#endif // LOGGER_H
