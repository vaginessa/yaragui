#include "target_panel.h"
#include <sstream>
#include <QtWidgets/QToolBar>
#include <QtGui/QPainter>

TargetPanel::TargetPanel(QWidget* parent)
{
  m_ui.setupUi(this);
  hide();

  m_ui.layout->setContentsMargins(0,0,0,0);
  QToolBar* tb = new QToolBar("Visualization", this);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16, 16));
  m_ui.layout->insertWidget(0, tb);

  m_histogramButton = tb->addAction("Histogram");
  connect(m_histogramButton, SIGNAL(triggered()), this, SLOT(showHistogram()));
  m_histogramButton->setIcon(QIcon("res/glyphicons-42-charts.png"));

  m_lineGraphButton = tb->addAction("Graph");
  connect(m_lineGraphButton, SIGNAL(triggered()), this, SLOT(showLineGraph()));
  m_lineGraphButton->setIcon(QIcon("res/glyphicons-41-stats.png"));

  showHistogram();
}

void TargetPanel::show(FileStats::Ref stats)
{
  m_stats = stats;
  prepareHistogram();
  renderView();
  QWidget::show();
}

void TargetPanel::showHistogram()
{
  m_viewMode = ViewModeHistogram;
  m_histogramButton->setEnabled(false);
  m_lineGraphButton->setEnabled(true);

  m_ui.infoFrame->setVisible(false);
  m_ui.rightFrame->setVisible(true);

  renderView();
}

void TargetPanel::showLineGraph()
{
  m_viewMode = ViewModeLineGraph;
  m_histogramButton->setEnabled(true);
  m_lineGraphButton->setEnabled(false);

  m_ui.infoFrame->setVisible(true);
  m_ui.rightFrame->setVisible(false);

  renderView();
}

void TargetPanel::renderView()
{
  if (!m_stats) {
    return;
  }

  switch (m_viewMode) {
  case ViewModeHistogram:
    renderHistogram();
    break;
  case ViewModeLineGraph:
    renderLineGraph();
    break;
  default:
    break;
  }
}

void TargetPanel::renderHistogram()
{
  m_ui.leftGraph->setPixmap(m_histogramPixmap.scaled(m_ui.leftGraph->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void TargetPanel::renderLineGraph()
{
  const int gridDensity = 16;
  const double heightPercent = 0.95; /* dont use the entire graph height */

  QPixmap pixmap(m_ui.leftGraph->size());
  QPainter painter(&pixmap);

  painter.setRenderHints(QPainter::Antialiasing);
  painter.fillRect(pixmap.rect(), QColor(255, 255, 255));

  const double width = pixmap.width();
  const double height = pixmap.height();

  /* pen for grid */
  QPen gridPen = painter.pen();
  gridPen.setWidthF(0.25);
  gridPen.setColor(QColor(0, 0, 0));
  painter.setPen(gridPen);

  /* draw horizontal grid */
  for(int i = 0; i < gridDensity; ++i)
  {
      double fi = i / double(gridDensity - 1);
      double ypos = fi * height;
      painter.drawLine(0, ypos, width, ypos);
  }

  /* draw vertical grid */
  double aspectRatio = pixmap.width() / double(pixmap.height());
  int gridDenistyVertical = gridDensity * aspectRatio;
  for(int i = 0; i < gridDenistyVertical; ++i)
  {
      double fi = i / double(gridDenistyVertical - 1);
      double xpos = fi * width;
      painter.drawLine(xpos, 0, xpos, height);
  }

  /* get graph limits for scaling */
  std::vector<double> data = m_stats->entropy1d();
  double dataMin = std::numeric_limits<double>::max();
  double dataMax = -std::numeric_limits<double>::max();
  for(size_t i = 0; i < data.size(); ++i)
  {
      dataMin = std::min(dataMin, data[i]);
      dataMax = std::max(dataMax, data[i]);
  }

  /* use this pen for drawing the graph edge */
  QPen graphPen = painter.pen();
  graphPen.setWidthF(1.0);
  graphPen.setColor(QColor(0, 0, 0));
  painter.setPen(graphPen);

  /* filled slightly transparent so you can see the grid underneath */
  painter.setBrush(QColor(0, 0, 255, 128));

  /* setup polyon points */
  std::vector<QPointF> polygon;
  polygon.push_back(QPointF(0, height));
  for(size_t i = 0; i < data.size(); ++i)
  {
      double fi = i / double(data.size() - 1);
      double norm = (data[i] - dataMin) / (dataMax - dataMin);
      double ypos = 1 - (1 - norm) * heightPercent;
      polygon.push_back(QPointF(fi * width, ypos * height));
  }

  /* draw graph as a filled polygon */
  polygon.push_back(QPointF(width, height));
  painter.drawPolygon(&polygon[0], polygon.size());

  m_ui.leftGraph->setPixmap(pixmap);
  m_ui.rightGraph->setPixmap(pixmap);
}

int pack(double r, double g, double b)
{
  int k = 255 << 24;
  k |= int(r * 255) << 16;
  k |= int(g * 255) << 8;
  k |= int(b * 255);
  return k;
}

void TargetPanel::prepareHistogram()
{
  const std::vector<double>& entropy2d = m_stats->entropy2d();
  m_histogramBuffer = std::vector<int>(256 * 256);
  for (int y = 0; y < 256; ++y) {
    for (int x = 0; x < 256; ++x) {
      double h = entropy2d[y*256+x] / 8;
      m_histogramBuffer[y*256+x] = pack(h, h, h);
    }
  }
  QImage image((uchar*)&m_histogramBuffer[0], 256, 256, QImage::Format_RGB32);
  m_histogramPixmap = QPixmap::fromImage(image);
}

void TargetPanel::resizeEvent(QResizeEvent* event)
{
  renderView();
  QWidget::resizeEvent(event);
}
