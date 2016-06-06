#ifndef __COMPILE_WINDOW_H__
#define __COMPILE_WINDOW_H__

#include "ui_compile_window.h"
#include "ruleset_view.h"

class CompileWindow : public QMainWindow
{
  Q_OBJECT

public:

  typedef boost::shared_ptr<CompileWindow> Ref;

  CompileWindow(RulesetView::Ref rule);

  RulesetView::Ref rule();
  void setRule(RulesetView::Ref rule);

private:

  Ui::CompileWindow m_ui;

  RulesetView::Ref m_rule;

};

#endif // __COMPILE_WINDOW_H__
