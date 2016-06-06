#ifndef __TARGET_PANEL_H__
#define __TARGET_PANEL_H__

#include "ui_target_panel.h"

class TargetPanel : public QWidget
{
  Q_OBJECT

public:

  TargetPanel(QWidget* parent);

  void show(const std::string& target);

private:

  Ui::TargetPanel m_ui;

};

#endif // __TARGET_PANEL_H__
