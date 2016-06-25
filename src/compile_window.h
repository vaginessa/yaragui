#ifndef __COMPILE_WINDOW_H__
#define __COMPILE_WINDOW_H__

#include "ui_compile_window.h"
#include "ruleset_view.h"
#include <boost/signals2.hpp>

class CompileWindow : public QMainWindow
{
  Q_OBJECT

public:

  typedef boost::shared_ptr<CompileWindow> Ref;

  CompileWindow(RulesetView::Ref rule);

  boost::signals2::signal<void (RulesetView::Ref rule)> onRecompileRule;

  void setCompilerBusy(bool state);

  RulesetView::Ref rule();
  void setRule(RulesetView::Ref rule);

public slots:

  void handleCompileClicked();

private:

  void keyPressEvent(QKeyEvent *event);

  Ui::CompileWindow m_ui;
  RulesetView::Ref m_rule;

};

#endif // __COMPILE_WINDOW_H__
