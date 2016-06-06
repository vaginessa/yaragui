#include "match_panel.h"

MatchPanel::MatchPanel(QWidget* parent) : QWidget(parent)
{
  m_ui.setupUi(this);
  hide();
}

void MatchPanel::show(const ScannerRule::Ref rule, RulesetView::Ref view)
{
  m_ui.label->setText(rule->identifier.c_str());
  QWidget::show();
}
