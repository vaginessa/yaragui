#include "compile_window.h"
#include <QtGui/QKeyEvent>

CompileWindow::CompileWindow(RulesetView::Ref rule)
{
  m_ui.setupUi(this);
  setWindowIcon(QIcon(":/yaragui.png"));
  connect(m_ui.compileButton, SIGNAL(clicked()), this, SLOT(handleCompileClicked()));
  connect(m_ui.closeButton, SIGNAL(clicked()), this, SLOT(close()));
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

void CompileWindow::handleCompileClicked()
{
  onRecompileRule(m_rule);
}

void CompileWindow::keyPressEvent(QKeyEvent *event)
{
  switch(event->key())
  {
  case Qt::Key_Escape:
    close();
    break;
  default:
    QMainWindow::keyPressEvent(event);
  }
}
