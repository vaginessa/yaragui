#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "ruleset.h"
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

class Settings
{
public:

  ~Settings();
  Settings();

  bool saveToDisk();

  std::vector<Ruleset::Ref> getRules() const;
  void setRules(const std::vector<Ruleset::Ref>& rules);

  std::string getMainWindowGeometry() const;
  void setMainWindowGeometry(const std::string& state);

  std::string getRuleWindowGeometry() const;
  void setRuleWindowGeoemtry(const std::string& state);

private:

  boost::property_tree::ptree m_tree;

  std::string m_file;

};

#endif // __SETTINGS_H__
