#ifndef __MATCH_PANEL_H__
#define __MATCH_PANEL_H__

#include "ui_match_panel.h"
#include "scanner_rule.h"
#include "ruleset_view.h"

class MatchPanel : public QWidget
{
  Q_OBJECT

public:

  MatchPanel(QWidget* parent);

  void show(const ScannerRule::Ref rule, RulesetView::Ref view);

private:

  Ui::MatchPanel m_ui;

};

#endif // __MATCH_PANEL_H__
