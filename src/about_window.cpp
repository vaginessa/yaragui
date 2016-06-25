#include "about_window.h"
#include <boost/make_shared.hpp>
#include <boost/version.hpp>
#include <yara.h>
#include <QtGui/QKeyEvent>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

using namespace GfxMath;

AboutWindow::AboutWindow(boost::asio::io_service& io, const QRect& parentGeometry) : m_frameIndex(0)
{
  m_ui.setupUi(this);
  setWindowIcon(QIcon(":/yaragui.png"));

  QRect selfGeometry = geometry();
  selfGeometry.moveCenter(parentGeometry.center());
  setGeometry(selfGeometry);

  m_gfxRenderer = boost::make_shared<GfxRenderer>(boost::ref(io));
  m_gfxRenderer->setFrameCallback(boost::bind(&AboutWindow::handleFrameRendered, this, _1));
  m_frames = std::vector<QPixmap>(25 * 5);
  m_gfxRenderer->render(320, 100, m_frames.size(), boost::make_shared<Shader>());

  m_player = new QTimer(this);
  connect(m_player, SIGNAL(timeout()), this, SLOT(handleFrameFlip()));

  std::stringstream info;
  info << "<p align=\"center\">";
  info << "<b>YARA GUI 0.0.1</b><br>";
  info << "Bult on " << __DATE__ << " at " << __TIME__ << "<br><br>";
  info << "Written in C++ by dila<br>";
  info << "For updates see:<br>";
  info << "<a href=\"http://sigint9.github.io/yaragui/\">http://sigint9.github.io/yaragui/</a><br><br>";
  info << "Yara version " << YR_VERSION << "<br>";
  info << "Qt version " << qVersion() << "<br>";
  info << "Boost version " << BOOST_LIB_VERSION << "<br>";
  info << "Icons by glyphicons.com";
  info << "</p>";
  m_ui.info->setHtml(info.str().c_str());
}

void AboutWindow::closeEvent(QCloseEvent *event)
{
  m_gfxRenderer.reset();
  m_frames.clear();
  QMainWindow::closeEvent(event);
}

QFont AboutWindow::getFont()
{
  QFont font("Courier New");
  font.setWeight(QFont::DemiBold);
  font.setPixelSize(32);
  return font;
}

void AboutWindow::handleFrameRendered(GfxRenderer::Frame::Ref frame)
{
  QImage image((uchar *)&frame->pixels[0], frame->width, frame->height, QImage::Format_RGB888);
  QPixmap pixmap = QPixmap::fromImage(image);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.setFont(getFont());
  QPen pen(QColor(255, 255, 255));
  painter.setPen(pen);
  painter.drawText(pixmap.rect(), Qt::AlignCenter, "{} YARA GUI");

  if (frame->frameNumber == 0) {
    m_ui.gfx->setPixmap(pixmap);
    //setFixedSize(width(), height());
    setFixedWidth(width());
    show();
  }

  if (m_frames.size() > frame->frameNumber) {
    m_frames[frame->frameNumber] = pixmap;
    m_frameIndex++;
  }

  if (m_frameIndex == m_frames.size()) {
    m_player->start(1000 / 25);
  }
}

void AboutWindow::keyPressEvent(QKeyEvent *event)
{
  switch(event->key())
  {
  case Qt::Key_Escape:
    close();
    break;
  default:
    QMainWindow::keyPressEvent(event);
  }
}

void AboutWindow::handleFrameFlip()
{
  if (!m_frames.size()) {
    return;
  }
  m_frameIndex = (m_frameIndex + 1) % m_frames.size();
  m_ui.gfx->setPixmap(m_frames[m_frameIndex]);
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

float udRoundBox( vec3 p, vec3 b, float r )
{
  return length(max(abs(p)-b,vec3(0.0)))-r;
}

float tube( vec3 p, float b )
{
  return length(p.permute(0,1)) - b;
}

float map(vec3 p)
{
	vec3 q = (fract(p/2.5) - 0.5) * 2.5;
  float tr = 0.25;
  float d = tube(q, tr);
  d = min(d, tube(q.permute(1,2,0), tr));
  d = min(d, tube(q.permute(0,2,1), tr));
  d = min(d, udRoundBox(q, vec3(0.45), 0.2));
  return d;
}

vec3 normal(vec3 p)
{
	vec3 o = vec3(0.001, 0.0, 0.0);
  return normalize(vec3(map(p+o.permute(0,1,1))/*xyy*/ - map(p-o.permute(0,1,1))/*xyy*/,
                        map(p+o.permute(1,0,1))/*yxy*/ - map(p-o.permute(1,0,1))/*yxy*/,
                        map(p+o.permute(1,1,0))/*yyx*/ - map(p-o.permute(1,1,0))/*yyx*/));
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

GfxMath::vec3 AboutWindow::Shader::shade(const GfxMath::vec2& fragCoord, const GfxMath::vec2& fragRes, const float time)
{
  vec2 uv = fragCoord / fragRes;
  uv = uv * 2.0 - 1.0;
  uv[1] *= fragRes[1] / fragRes[0];

  vec3 r = normalize(vec3(uv, 0.5 - dot(uv, uv) * 0.66));
  
  r = r * yrot(0.5 + time * 3.14159 * 2.0);
  r = r * zrot(time * 3.14159 * 4.0);
  
  vec3 o = vec3(0.0, 0.0, 5.0) * time * 4.0;
  
  float t = trace(o, r);
  
  vec3 w = o + r * t;
  vec3 sn = normal(w);
  
  vec3 s = vec3(0.5);
  
  vec3 lv = normalize(vec3(1.0));
  
  float lp = max(dot(sn, lv), 0.0);
  vec3 lc = vec3(1.0, 0.5, 0.0);
  
  float lp2 = max(dot(sn, -lv), 0.0);
  vec3 lc2 = vec3(1.0, 0.6, 0.1);
  
  s += lp * lc;
  s += lp2 * lc2;
  s *= max(dot(r, -sn), 0.0);
  
  vec3 ref = reflect(r, sn);
  float spec = max(dot(lv, ref), 0.0);
  float spec2 = max(dot(-lv, ref), 0.0);
  
  spec = pow(spec, 16.0);
  spec2 = pow(spec2, 16.0);
  
  float fog = 1.0 / (1.0 + t * t * 0.03);
  
  vec3 fc = s;
  fc += spec * lc;
  fc += spec2 * lc2;

  return fc * fog;
}
