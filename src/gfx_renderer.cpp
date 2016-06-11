#include "gfx_renderer.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <iostream>

GfxRenderer::~GfxRenderer()
{
  m_shutdownRequest = true;
  BOOST_FOREACH(ThreadData::Ref td, m_threadData) {
    td->io.stop();
  }
  m_threads.join_all();
}

GfxRenderer::GfxRenderer(boost::asio::io_service& io, size_t threadCount) : m_io(io)
{
  if (!threadCount) {
    threadCount = boost::thread::hardware_concurrency();
  }

  for (size_t i = 0; i < threadCount; ++i) {
    ThreadData::Ref td = boost::make_shared<ThreadData>();
    td->threadIndex = i;
    m_threadData.push_back(td);
  }

  m_shutdownRequest = false;

  BOOST_FOREACH(ThreadData::Ref td, m_threadData) {
    m_threads.create_thread(boost::bind(&GfxRenderer::thread, this, td));
  }
}

void GfxRenderer::render(const int width, const int height, const int numFrames, Shader::Ref shader)
{
  m_width = width;
  m_height = height;
  m_numFrames = numFrames;
  m_shader = shader;
  m_frameCounter = 0;
  BOOST_FOREACH(ThreadData::Ref td, m_threadData) {
    td->io.post(boost::bind(&GfxRenderer::threadRender, this, td));
  }
}

void GfxRenderer::thread(ThreadData::Ref td)
{
  boost::asio::io_service::work keep_alive(td->io);
  td->io.run();
}

void GfxRenderer::threadRender(ThreadData::Ref td)
{
  while (!m_shutdownRequest) {
    int frameNumber = m_frameCounter++;
    if (frameNumber >= m_numFrames) {
      break;
    }

    float time = frameNumber / double(m_numFrames);
    Frame::Ref frame = boost::make_shared<Frame>();
    frame->width = m_width;
    frame->height = m_height;
    frame->frameNumber = frameNumber;
    frame->pixels = std::vector<char>(m_width * m_height * 3);
    for (int y = 0; y < m_height; ++y) {
      for (int x = 0; x < m_width; ++x) {
        GfxMath::vec2 fragCoord(x + 0.5, y + 0.5);
        GfxMath::vec2 fragRes(m_width, m_height);
        GfxMath::vec3 pixel = m_shader->shade(fragCoord, fragRes, time);
        size_t pixelOffset = (y * m_width + x) * 3;
        frame->pixels[pixelOffset] = uint8_t(GfxMath::clamp(pixel[0], 0.0, 1.0) * 255);
        frame->pixels[pixelOffset+1] = uint8_t(GfxMath::clamp(pixel[1], 0.0, 1.0) * 255);
        frame->pixels[pixelOffset+2] = uint8_t(GfxMath::clamp(pixel[2], 0.0, 1.0) * 255);
      }
      if (m_shutdownRequest) {
        break;
      }
    }
    m_io.post(boost::bind(m_frameCallback, frame));
  }
}
