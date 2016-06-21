#ifndef __STATS_CALCULATOR_H__
#define __STATS_CALCULATOR_H__

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>
#include "file_stats.h"

class StatsCalculator
{
public:

  typedef boost::shared_ptr<StatsCalculator> Ref;

  ~StatsCalculator();
  StatsCalculator(boost::asio::io_service& io);

  boost::signals2::signal<void (FileStats::Ref stats)> onFileStats;

  void getStats(const std::string& file);

private:

  boost::asio::io_service& m_io;

};

#endif // __STATS_CALCULATOR_H__
