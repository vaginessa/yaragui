#include "asio_events.h"
#include <boost/make_shared.hpp>
#include <iostream>

AsioEvents::AsioEvents(boost::asio::io_service& io) : m_io(io)
{
  m_timer = boost::make_shared<QTimer>();
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(timeout()));
  m_timer->setSingleShot(true);
  timeout();
}

void AsioEvents::timeout()
{
  boost::system::error_code error;
  size_t count = m_io.poll_one(error);
  m_io.reset();

  /* if we are busy, poll quickly */
  if (count) {
    m_timer->start(0);
  } else {
    m_timer->start(1000/25);
  }
}
