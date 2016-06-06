#include "compile_window.h"

CompileWindow::CompileWindow(RulesetView::Ref rule)
{
  m_ui.setupUi(this);
  setRule(rule);
  show();
}

RulesetView::Ref CompileWindow::rule()
{
  return m_rule;
}

void CompileWindow::setRule(RulesetView::Ref rule)
{
  m_rule = rule;
  m_ui.rulePath->setText(rule->file().c_str());
  m_ui.output->setText(rule->compilerMessages().c_str());
}
