#include "about_window.h"
#include <QImage>
#include <QPixmap>

using namespace GfxMath;

AboutWindow::AboutWindow(boost::asio::io_service& io)
{
  m_gfxRenderer = boost::make_shared<GfxRenderer>(boost::ref(io));
  m_gfxRenderer->setFrameCallback(boost::bind(&AboutWindow::displayFrame, this, _1));
  m_ui.setupUi(this);
  m_gfxRenderer->render(320, 100, 1000, boost::make_shared<Shader>());
}

void AboutWindow::closeEvent(QCloseEvent *event)
{
  m_gfxRenderer.reset();
  QMainWindow::closeEvent(event);
}

void AboutWindow::displayFrame(GfxRenderer::Frame::Ref frame)
{
  std::cout << "Got frame" << std::endl;
  QImage image((uchar *)&frame->pixels[0], frame->width, frame->height, QImage::Format_RGB888);
  m_ui.gfx->setPixmap(QPixmap::fromImage(image));
  if (m_gfxRenderer) {
    show();
  }
}

float map(vec3 p)
{
  return length(p) - 1.0;
}

float trace(vec3 o, vec3 r)
{
  float t = 0.0;
  for (int i = 0; i < 16; ++i) {
    vec3 p = o + r * t;
    float d = map(p);
    t += d;
  }
  return t;
}

GfxMath::vec3 AboutWindow::Shader::shade(const GfxMath::vec2& uv, const float time)
{
  vec3 r = vec3(uv, 1.0);

  vec3 o = vec3(0.0, 0.0, -3.0);

  float t = trace(o, r);

  float fog = 1.0 / (1.0 + t * t * 0.1);

  vec3 fc = vec3(fog);

  return fc;
}
