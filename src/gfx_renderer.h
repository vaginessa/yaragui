#ifndef __GFX_RENDERER_H__
#define __GFX_RENDERER_H__

#include "gfx_math.h"
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <vector>

class GfxRenderer
{
public:

  typedef boost::shared_ptr<GfxRenderer> Ref;

  ~GfxRenderer();

  GfxRenderer(boost::asio::io_service& io, size_t threadCount = 0);

  struct Frame
  {
    typedef boost::shared_ptr<Frame> Ref;
    std::vector<char> pixels;
    int width, height;
    int frameNumber;
  };

  class Shader
  {
  public:
    typedef boost::shared_ptr<Shader> Ref;
    virtual ~Shader() {}
    Shader() {}
    virtual GfxMath::vec3 shade(const GfxMath::vec2& uv, const float time) = 0;
  };

  bool idle() const {return m_frameCounter >= m_numFrames;}

  typedef boost::function<void (Frame::Ref frame)> FrameCallback;

  void setFrameCallback(FrameCallback callback) {m_frameCallback = callback;}

  void render(const int width, const int height, const int numFrames, Shader::Ref shader);

private:

  struct ThreadData
  {
    typedef boost::shared_ptr<ThreadData> Ref;
    boost::asio::io_service io;
    size_t threadIndex;
  };

  void thread(ThreadData::Ref td);
  void threadRender(ThreadData::Ref td);

  boost::asio::io_service& m_io;
  boost::thread_group m_threads;
  std::vector<ThreadData::Ref> m_threadData;
  boost::atomic<int> m_frameCounter;
  boost::atomic<bool> m_shutdownRequest;
  Shader::Ref m_shader;
  FrameCallback m_frameCallback;
  int m_numFrames;
  int m_width;
  int m_height;

};

#endif // __GFX_RENDERER_H__
