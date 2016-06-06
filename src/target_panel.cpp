#include "target_panel.h"

TargetPanel::TargetPanel(QWidget* parent)
{
  m_ui.setupUi(this);
  hide();
}

void TargetPanel::show(const std::string& target)
{
  m_ui.label->setText(target.c_str());
  QWidget::show();
}
