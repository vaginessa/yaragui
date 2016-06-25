#include "target_panel.h"
#include <sstream>
#include <QtWidgets/QToolBar>
#include <QtGui/QPainter>
#include "gfx_math.h"

TargetPanel::TargetPanel(QWidget* parent)
{
  m_ui.setupUi(this);
  hide();

  m_ui.layout->setContentsMargins(0,0,0,0);
  QToolBar* tb = new QToolBar("Visualization", this);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16, 16));
  m_ui.layout->insertWidget(0, tb);

  connect(m_ui.slider, SIGNAL(valueChanged(int)), this, SLOT(handleSlider()));

  m_lineGraphButton = tb->addAction("1D Entropy");
  connect(m_lineGraphButton, SIGNAL(triggered()), this, SLOT(showLineGraph()));
  m_lineGraphButton->setIcon(QIcon(":/glyphicons-460-header.png"));

  m_histogramButton = tb->addAction("2D Entropy");
  connect(m_histogramButton, SIGNAL(triggered()), this, SLOT(showHistogram()));
  m_histogramButton->setIcon(QIcon(":/glyphicons-157-show-thumbnails.png"));

  m_barGraphButton = tb->addAction("Histogram");
  connect(m_barGraphButton, SIGNAL(triggered()), this, SLOT(showBarGraph()));
  m_barGraphButton->setIcon(QIcon(":/glyphicons-42-charts.png"));

  showLineGraph();
}

void TargetPanel::show(const std::string& filename, FileStats::Ref stats)
{
  m_filename = filename;

  if (!stats || stats->accessError()) {
    /* do not try to render failed stats */
    m_stats = FileStats::Ref();
  } else {
    m_stats = stats;
  }

  updateInfo();
  prepareHistogram();
  QWidget::show();
  renderView();
}

void TargetPanel::showHistogram()
{
  m_viewMode = ViewModeHistogram;
  m_histogramButton->setEnabled(false);
  m_lineGraphButton->setEnabled(true);
  m_barGraphButton->setEnabled(true);
  renderView();
}

void TargetPanel::showLineGraph()
{
  m_viewMode = ViewModeLineGraph;
  m_histogramButton->setEnabled(true);
  m_lineGraphButton->setEnabled(false);
  m_barGraphButton->setEnabled(true);
  renderView();
}

void TargetPanel::showBarGraph()
{
  m_viewMode = ViewModeBarGraph;
  m_histogramButton->setEnabled(true);
  m_lineGraphButton->setEnabled(true);
  m_barGraphButton->setEnabled(false);
  renderView();
}

void TargetPanel::handleSlider()
{
  prepareHistogram();
  renderView();
}

void TargetPanel::renderView()
{
  if (!m_stats) {
    m_ui.leftGraph->setPixmap(QPixmap());
    m_ui.leftGraph->setText("No data available");
    m_ui.slider->hide();
    m_ui.progressBar->hide();
    m_ui.sizeText->hide();
    return;
  }

  m_ui.leftGraph->setText("");
  m_ui.slider->show();
  m_ui.progressBar->show();
  m_ui.sizeText->show();

  switch (m_viewMode) {
  case ViewModeHistogram:
    renderHistogram();
    break;
  case ViewModeLineGraph:
    renderLineGraph();
    break;
  case ViewModeBarGraph:
    renderBarGraph();
    break;
  default:
    break;
  }
}

void TargetPanel::renderHistogram()
{
  /* already computed in prepareHistogram() */
  m_ui.leftGraph->setPixmap(m_histogramPixmap.scaled(m_ui.leftGraph->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void TargetPanel::renderLineGraph()
{
  QPixmap pixmap(m_ui.leftGraph->size());
  QPainter painter(&pixmap);

  const int gridDensity = pixmap.width() / 64.0;
  const double heightPercent = 0.95; /* dont use the entire graph height */

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
  for(int i = 0; i < gridDensity; ++i) {
    double fi = i / double(gridDensity - 1);
    double ypos = fi;
    ypos = 1 - pow(1 - ypos, 1 + sliderValue() * 8);
    ypos *= height;
    painter.drawLine(0, ypos, width, ypos);
  }

  /* draw vertical grid */
  double aspectRatio = pixmap.width() / double(pixmap.height());
  int gridDenistyVertical = gridDensity * aspectRatio;
  for(int i = 0; i < gridDenistyVertical; ++i) {
    double fi = i / double(gridDenistyVertical - 1);
    double xpos = fi * width;
    painter.drawLine(xpos, 0, xpos, height);
  }

  /* get graph limits for scaling */
  std::vector<double> data = m_stats->entropy1d();
  double dataMin = std::numeric_limits<double>::max();
  double dataMax = -std::numeric_limits<double>::max();
  for(size_t i = 0; i < data.size(); ++i) {
    dataMin = std::min(dataMin, data[i]);
    dataMax = std::max(dataMax, data[i]);
  }

  /* use this pen for drawing the graph edge */
  QPen graphPen = painter.pen();
  graphPen.setWidthF(1.0);
  graphPen.setColor(QColor(0, 0, 0));
  painter.setPen(graphPen);

  /* filled slightly transparent so you can see the grid underneath */
  painter.setBrush(QColor(0, 0, 0, 64));

  /* setup polyon points */
  std::vector<QPointF> polygon;
  polygon.push_back(QPointF(0, height));
  for(size_t i = 0; i < data.size(); ++i) {
    double fi = i / double(data.size() - 1);
    double norm = data[i] / dataMax;
    double ypos = 1 - norm;
    ypos = 1 - pow(1 - ypos, 1 + sliderValue() * 8);
    ypos = 1 - (1 - ypos) * heightPercent;
    polygon.push_back(QPointF(fi * width, ypos * height));
  }

  /* draw graph as a filled polygon */
  polygon.push_back(QPointF(width, height));
  painter.drawPolygon(&polygon[0], polygon.size());

  m_ui.leftGraph->setPixmap(pixmap);
}

void TargetPanel::renderBarGraph()
{
  QSize pixmapSize = m_ui.leftGraph->size();
  double aspectRatio = pixmapSize.height() / double(pixmapSize.width());
  pixmapSize.setWidth(std::max(pixmapSize.width(), 512));
  pixmapSize.setHeight(pixmapSize.width() * aspectRatio);

  const int gridDensity = pixmapSize.width() / 64.0;
  const double heightPercent = 0.95; /* dont use the entire graph height */

  QPixmap pixmap(pixmapSize);
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
  for(int i = 0; i < gridDensity; ++i) {
    double fi = i / double(gridDensity - 1);
    double ypos = fi;
    //ypos = 1 - pow(1 - ypos, 1 - sliderValue() * heightPercent);
    ypos = ypos - 0.5;
    double sgn = GfxMath::sign(ypos);
    ypos = pow(fabs(ypos), 1 - sliderValue() * heightPercent * 0.5);
    ypos = ypos * sgn + 0.5;
    ypos *= height;
    painter.drawLine(0, ypos, width, ypos);
  }

  /* draw vertical grid */
  int gridDenistyVertical = gridDensity / aspectRatio;
  for(int i = 0; i < gridDenistyVertical; ++i) {
    double fi = i / double(gridDenistyVertical - 1);
    double xpos = fi * width;
    painter.drawLine(xpos, 0, xpos, height);
  }

  /* get graph limits for scaling */
  std::vector<double> data = m_stats->histogram();
  double dataMin = std::numeric_limits<double>::max();
  double dataMax = -std::numeric_limits<double>::max();
  for(size_t i = 0; i < data.size(); ++i) {
    dataMin = std::min(dataMin, data[i]);
    dataMax = std::max(dataMax, data[i]);
  }

  /* use this pen for drawing the graph edge */
  QPen graphPen = painter.pen();
  graphPen.setWidthF(0.0);
  painter.setPen(graphPen);

  /* setup polyon points */
  QRect rect(0, 0, 0, 0);
  for(size_t i = 0; i < data.size(); ++i) {
    rect.setLeft(rect.right());
    double xpos = (i + 1) / double(data.size()) * width;
    double norm = (data[i] - dataMin * 0.5) / (dataMax - dataMin * 0.5);
    norm = pow(norm, 1 - sliderValue() * heightPercent * 0.5);
    double barHeight = norm * height * heightPercent;
    rect.setRight(xpos);
    rect.setTop((height - barHeight) * 0.5);
    rect.setBottom((height + barHeight) * 0.5);
    painter.fillRect(rect, QColor(128, 128, 128));
  }

  m_ui.leftGraph->setPixmap(pixmap.scaled(m_ui.leftGraph->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void TargetPanel::updateInfo()
{
  if (!m_stats) {
    return;
  }

  std::stringstream size;
  size << "" << m_stats->fileSize() << " bytes";
  m_ui.sizeText->setText(size.str().c_str());

  std::stringstream entropy;
  entropy.precision(2);
  entropy << "" << std::fixed << m_stats->totalEntropy() << " bits";

  double bar = m_stats->totalEntropy() / 8 * 100;
  m_ui.progressBar->setValue(bar);
  m_ui.progressBar->setFormat(entropy.str().c_str());
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
  if (!m_stats) {
    return;
  }
  const std::vector<double>& entropy2d = m_stats->entropy2d();
  m_histogramBuffer = std::vector<int>(256 * 256);
  for (int y = 0; y < 256; ++y) {
    for (int x = 0; x < 256; ++x) {
      double h = entropy2d[y*256+x];
      h = 1 - pow(1 - h, 1 + sliderValue() * 8.0);
      m_histogramBuffer[y*256+x] = pack(h, h, h);
    }
  }
  QImage image((uchar*)&m_histogramBuffer[0], 256, 256, QImage::Format_RGB32);
  m_histogramPixmap = QPixmap::fromImage(image);
}

double TargetPanel::sliderValue() const
{
  return m_ui.slider->value() / 100.0;
}

void TargetPanel::resizeEvent(QResizeEvent* event)
{
  renderView();
  QWidget::resizeEvent(event);
}
