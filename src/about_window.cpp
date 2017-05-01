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
  m_frames = std::vector<QPixmap>(30 * 5);
  m_gfxRenderer->render(320, 100, m_frames.size(), boost::make_shared<Shader>());

  m_player = new QTimer(this);
  connect(m_player, SIGNAL(timeout()), this, SLOT(handleFrameFlip()));

  std::stringstream info;
  info << "<p align=\"center\">";
  info << "<b>YARA GUI 0.4</b><br>";
  info << "Built on " << __DATE__ << " at " << __TIME__ << "<br><br>";
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
  painter.drawText(pixmap.rect(), Qt::AlignCenter, "{ YARA GUI");

  if (frame->frameNumber == 0) {
    m_ui.gfx->setPixmap(pixmap);
    setFixedWidth(width());
    show();
  }

  if (m_frames.size() > frame->frameNumber) {
    m_frames[frame->frameNumber] = pixmap;
    m_frameIndex++;
  }

  if (m_frameIndex == m_frames.size()) {
    m_player->start(1000 / 30);
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
              0.0, cos(t), sin(t),
              0.0, -sin(t), cos(t));
}

mat3 yrot(float t)
{
  return mat3(cos(t), 0.0, -sin(t),
              0.0, 1.0, 0.0,
              sin(t), 0.0, cos(t));
}

mat3 zrot(float t)
{
  return mat3(cos(t), sin(t), 0.0,
              -sin(t), cos(t), 0.0,
              0.0, 0.0, 1.0);
}

float sdBoxXY( vec3 p, vec3 b )
{
  vec2 d = abs(p.permute(0, 1)) - b.permute(0, 1);
  vec2 k = vec2(max(d[0], 0), max(d[1], 0));
  return min(max(d[0],d[1]),0.0) + length(k);
}

float smin( float a, float b, float k )
{
  float res = exp( -k*a ) + exp( -k*b );
  return -log( res )/k;
}

float udRoundBox( vec3 p, vec3 b, float r )
{
  return length(max(abs(p)-b,vec3(0.0)))-r;
}

float map(vec3 p)
{
  float k = 0.5 * 2.0;
  vec3 q = (fract((p - vec3(0.25, 0.0, 0.25))/ k) - 0.5) * k;
  vec3 s = vec3(q[0], p[1], q[2]);
  float d = udRoundBox(s, vec3(0.1, 1.0, 0.1), 0.05);

  k = 0.5;
  q = (fract(p / k) - 0.5) * k;
  s = vec3(q[0], fabs(p[1]) - 1.5, q[2]);
  float g = udRoundBox(s, vec3(0.17, 0.5, 0.17), 0.2);

  float sq = sqrt(0.5);
  vec3 u = p;
  u[0] = p[0] * sq + p[2] * sq;
  u[1] = p[1];
  u[2] = p[0] * sq - p[2] * sq;
  d = max(d, -sdBoxXY(u, vec3(0.8, 1.0, 0.8)));

  return smin(d, g, 16.0);
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
      t += map(o + r * t);
  }
  return t;
}

GfxMath::vec3 AboutWindow::Shader::shade(const GfxMath::vec2& fragCoord, const GfxMath::vec2& fragRes, const float time)
{
  vec2 uv = fragCoord / fragRes;
  uv = uv * 2.0 - 1.0;
  uv[0] *= fragRes[0] / fragRes[1];

  vec3 r = normalize(vec3(uv, 1.7 - dot(uv, uv) * 0.1));
  float sgt = sin(time * 3.141592 * 2.0);
  r = r * zrot(sgt * 3.141592 / 8.0);
  r = r * yrot(time * 3.141592 * 2.0);
  r = r * yrot(3.141592 * -0.25);

  vec3 o = vec3(0.0, 0.0, time * 5.0 * sqrt(2.0) * 2.0);
  o = o * yrot(3.141592 * -0.25);

  float t = trace(o, r);
  vec3 w = o + r * t;
  vec3 sn = normal(w);
  float fd = map(w);

  vec3 col = vec3(0.514, 0.851, 0.933) * 0.5;
  vec3 ldir = normalize(vec3(-1, 0.5, 1.1));

  float fog = 1.0 / (1.0 + t * t * 0.1 + fd * 100.0);
  float front = max(dot(r, -sn), 0.0);
  float ref = max(dot(r, reflect(-ldir, sn)), 0.0);
  float grn = pow(fabs(sn[1]), 3.0);

  vec3 cl = vec3(grn);
  cl += mix(col*vec3(1.5), vec3(0.25), grn) * pow(ref, 16.0);
  cl = mix(col, cl, fog);
  return cl;
}
