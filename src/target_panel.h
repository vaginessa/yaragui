#ifndef __TARGET_PANEL_H__
#define __TARGET_PANEL_H__

#include "ui_target_panel.h"
#include "file_stats.h"
#include <vector>

class TargetPanel : public QWidget
{
  Q_OBJECT

public:

  TargetPanel(QWidget* parent);

  void show(const std::string& filename, FileStats::Ref stats);

  std::string filename() const {return m_filename;}

private slots:

  void showHistogram();
  void showLineGraph();
  void showBarGraph();
  void handleSlider();

private:

  enum ViewMode
  {
    ViewModeHistogram,
    ViewModeLineGraph,
    ViewModeBarGraph
  };

  void renderView(); /* main render */

  void renderHistogram();
  void renderLineGraph();
  void renderBarGraph();

  void updateInfo();
  void prepareHistogram();

  double sliderValue() const;

  virtual void resizeEvent(QResizeEvent* event);

  Ui::TargetPanel m_ui;
  std::string m_filename;
  FileStats::Ref m_stats;

  ViewMode m_viewMode;
  QAction* m_histogramButton;
  QAction* m_lineGraphButton;
  QAction* m_barGraphButton;

  std::vector<int> m_histogramBuffer;
  QPixmap m_histogramPixmap;

};

#endif // __TARGET_PANEL_H__
