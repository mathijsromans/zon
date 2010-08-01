#ifndef RESOURCES_H
#define RESOURCES_H

#include "serf.h"
#include <string>
#include <boost/array.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

class BITMAP;

class Resources : boost::noncopyable
{
  public:
    static Resources& getInstance();
    void load();
    struct Level
    {
      std::string text;
      boost::shared_ptr<BITMAP> map;
      boost::array<int, Serf::N_TYPES> numSerfs;
    };
    const boost::ptr_vector<Level>& getLevels() const { return m_levels; }
  private:
     Resources() {}
    ~Resources() {}
    boost::ptr_vector<Level> m_levels;
    static const std::string s_path;
    static const std::string s_file;

};

#endif // RESOURCES_H
