#include "about_window.h"
#include <boost/version.hpp>
#include <yara.h>
#include <QImage>
#include <QPixmap>
#include <QPainter>

using namespace GfxMath;

AboutWindow::AboutWindow(boost::asio::io_service& io) : m_frameIndex(0)
{
  m_ui.setupUi(this);
  setWindowIcon(QIcon(":/glyphicons-196-info-sign.png"));

  m_gfxRenderer = boost::make_shared<GfxRenderer>(boost::ref(io));
  m_gfxRenderer->setFrameCallback(boost::bind(&AboutWindow::handleFrameRendered, this, _1));
  m_frames = std::vector<QPixmap>(25 * 2);
  m_gfxRenderer->render(320, 100, m_frames.size(), boost::make_shared<Shader>());

  m_player = new QTimer(this);
  connect(m_player, SIGNAL(timeout()), this, SLOT(handleFrameFlip()));

  m_ui.ver->setText("gui 0.1");
  m_ui.yaraver->setText(QString("libyara ") + YR_VERSION);
  m_ui.qtver->setText(QString("qt ") + qVersion());
  m_ui.boostver->setText(QString("boost ") + BOOST_LIB_VERSION);

  show();
}

void AboutWindow::closeEvent(QCloseEvent *event)
{
  m_gfxRenderer.reset();
  m_frames.clear();
  QMainWindow::closeEvent(event);
}

void AboutWindow::handleFrameRendered(GfxRenderer::Frame::Ref frame)
{
  QImage image((uchar *)&frame->pixels[0], frame->width, frame->height, QImage::Format_RGB888);
  QPixmap pixmap = QPixmap::fromImage(image);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.setFont(QFont("Courier New"));
  QPen pen(QColor(255, 255, 255));
  painter.setPen(pen);
  painter.drawText(QPoint(220, 94), "dila/regroup");

  if (m_frames.size() > frame->frameNumber) {
    m_frames[frame->frameNumber] = pixmap;
    m_frameIndex++;
  }

  std::stringstream ss;
  int progress = m_frameIndex / double(m_frames.size()) * 100;
  ss << "Loading " << progress << "%...";
  m_ui.gfx->setText(ss.str().c_str());

  if (m_frameIndex == m_frames.size()) {
    m_player->start(1000 / 25);
  }
}

void AboutWindow::handleFrameFlip()
{
  if (!m_frames.size()) {
    return;
  }
  m_frameIndex = (m_frameIndex + 1) % m_frames.size();
  m_ui.gfx->setPixmap(m_frames[m_frameIndex]);
  m_ui.gfx->setText("");
}

mat3 xrot(float t)
{
  return mat3(1.0, 0.0, 0.0,
              0.0, cos(t), -sin(t),
              0.0, sin(t), cos(t));
}

mat3 yrot(float t)
{
  return mat3(cos(t), 0.0, -sin(t),
              0.0, 1.0, 0.0,
              sin(t), 0.0, cos(t));
}

mat3 zrot(float t)
{
  return mat3(cos(t), -sin(t), 0.0,
              sin(t), cos(t), 0.0,
              0.0, 0.0, 1.0);
}

float sdBox(vec3 p, vec3 b)
{
  vec3 d = abs(p) - b;
  return min(max(d[0],max(d[1],d[2])),0.0) +
         length(max(d,vec3(0.0)));
}

float map(vec3 p)
{
  return sdBox(p, vec3(1.0));
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
  vec3 r = normalize(vec3(uv, 1.0));

  r = r * yrot(time * 6.28);

  vec3 o = vec3(0.0, 0.0, -3.0);

  o = o * yrot(time * 6.28);

  float t = trace(o, r);
  vec3 w = o + r * t;
  float fd = map(w);

  float fog = 1.0 / (1.0 + t * t * 0.1 + fd * 100.0);

  vec3 fc = vec3(fog);

  return fc;
}
