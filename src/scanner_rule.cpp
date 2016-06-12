#include "scanner_rule.h"
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <yara.h>

#ifdef WIN32
  #undef min
  #undef max
#endif

ScannerRule::ScannerRule(YR_RULE* rule)
{
  identifier = rule->identifier;
  if (rule->ns) {
    ns = rule->ns->name;
  }

  const char* tagName = 0;
  yr_rule_tags_foreach(rule, tagName) {
    tags.push_back(tagName);
  }

  YR_META* meta = 0;
  yr_rule_metas_foreach(rule, meta) {
    Meta::Ref mref = boost::make_shared<Meta>();
    mref->identifier = meta->identifier;
    switch (meta->type) {
    case META_TYPE_STRING:
      mref->value = meta->string;
      break;
    case META_TYPE_INTEGER: {
      std::stringstream ss;
      ss << meta->integer;
      mref->value = ss.str();
      break;
    }
    case META_TYPE_BOOLEAN:
      mref->value = meta->integer ? "true" : "false";
      break;
    default:
      break;
    }
    metas.push_back(mref);
  }

  YR_STRING* string = 0;
  yr_rule_strings_foreach(rule, string) {
    String::Ref sref = boost::make_shared<String>();
    sref->identifier = string->identifier;
    sref->value = std::vector<uint8_t>(string->string, string->string + string->length);
    YR_MATCH* match = 0;
    yr_string_matches_foreach(string, match) {
      Match::Ref mref = boost::make_shared<Match>();
      mref->data = std::vector<uint8_t>(match->data, match->data + match->length);
      mref->base = match->base;
      mref->offset = match->offset;
      sref->matches.push_back(mref);
    }
    strings.push_back(sref);
  }
}

std::ostream& operator <<(std::ostream& os, ScannerRule::Ref rule)
{
  BOOST_FOREACH(ScannerRule::String::Ref string, rule->strings) {
    BOOST_FOREACH(ScannerRule::Match::Ref match, string->matches) {
      std::stringstream offset;
      offset << "0x" << std::hex << std::setw(8) << std::setfill('0') << match->offset;

      std::stringstream bytes;
      size_t maxBytes = std::min(match->data.size(), size_t(16));
      for (size_t i = 0; i < maxBytes; ++i) {
        bytes << (i != 0 ? " " : "") << std::hex << std::setw(2) << std::setfill('0') << int(match->data[i]);
      }

      if (maxBytes != match->data.size()) {
        bytes << "...";
      }

      os << offset.str() << ":" << string->identifier << " " << bytes.str() << std::endl;
    }
  }

  return os;
}
