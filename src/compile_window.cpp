#include "compile_window.h"
#include <sstream>
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

void CompileWindow::setEnabled(bool state)
{
  m_ui.compileButton->setEnabled(state);
}

RulesetView::Ref CompileWindow::rule()
{
  return m_rule;
}

void CompileWindow::setRule(RulesetView::Ref rule)
{
  m_rule = rule;
  m_ui.rulePath->setText(rule->file().c_str());
  std::stringstream ss;
  ss << rule->compilerMessages();
  if (rule->isCompiled()) {
    ss << "Rule compiled successfully." << std::endl;
  }
  m_ui.output->setText(ss.str().c_str());
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
