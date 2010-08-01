#include "resources.h"
#include "global.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <allegro.h>

const std::string Resources::s_path = "resources/";
const std::string Resources::s_file = "data.xml";

Resources& Resources::getInstance()
{
  static Resources resources;
  return resources;
}

void Resources::load()
{
  RGB my_palette[256];
  boost::property_tree::ptree pt;
  boost::property_tree::read_xml(s_path + s_file, pt);
  BOOST_FOREACH(boost::property_tree::ptree::value_type& v, pt.get_child("zon.levels"))
  {
    std::auto_ptr<Level> level( new Level );
    boost::property_tree::ptree ptLevel = v.second;
    level->text = ptLevel.get<std::string>("text", "No description");
    std::string filename = ptLevel.get<std::string>("filename");
    BITMAP* map = load_bmp((s_path + filename).c_str(), my_palette);
    if ( !map )
    {
      throw ZonException(std::string("Couldn't find the file '") +  filename +"'");
    }
    level->map.reset( map );
    boost::property_tree::ptree ptSerfs = ptLevel.get_child("serfs");
    level->numSerfs.assign( ptSerfs.get("default", 0 ) );
    const boost::array<std::string, Serf::N_TYPES>& serfNames = Serf::getSerfTypeNames();
    BOOST_FOREACH(boost::property_tree::ptree::value_type& v2, ptSerfs)
    {
      for ( unsigned int i = 0; i < Serf::N_TYPES; ++i )
      {
        if ( serfNames[i] == v2.first )
        {
          level->numSerfs[i] = v2.second.get_value<int>();
          break;
        }
      }
    }
    m_levels.push_back( level );
  }
}
