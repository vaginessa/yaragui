#include "stats_calculator.h"
#include <boost/make_shared.hpp>
#include <iostream>

StatsCalculator::~StatsCalculator()
{
  m_thread_io.stop();
  m_thread->join();
}

StatsCalculator::StatsCalculator(boost::asio::io_service& io) : m_io(io)
{
  m_thread = boost::make_shared<boost::thread>(boost::bind(&StatsCalculator::statsThread, this));
}

void StatsCalculator::reset()
{
  m_abort = false;
}

void StatsCalculator::abort()
{
  m_abort = true;
}

void StatsCalculator::getStats(const std::string& file)
{
  m_thread_io.post(boost::bind(&StatsCalculator::computeStats, this, file));
}

void StatsCalculator::statsThread()
{
  boost::asio::io_service::work keep_alive(boost::ref(m_thread_io));
  m_thread_io.run();
}

void StatsCalculator::computeStats(const std::string& file)
{
  /* in the stats thread */
  FileStats::Ref stats = boost::make_shared<FileStats>(file, m_abort);
  m_io.post(boost::bind(&StatsCalculator::reportStats, this, stats));
}

void StatsCalculator::reportStats(FileStats::Ref stats)
{
  /* in the main thread */
  onFileStats(stats);
}
