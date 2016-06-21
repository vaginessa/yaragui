#include "stats_calculator.h"
#include <boost/make_shared.hpp>

StatsCalculator::~StatsCalculator()
{
}

StatsCalculator::StatsCalculator(boost::asio::io_service& io) : m_io(io)
{
}

void StatsCalculator::getStats(const std::string& file)
{
  onFileStats(boost::make_shared<FileStats>(file));
}
