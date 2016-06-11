#include "match_panel.h"
#include <boost/foreach.hpp>
#include <sstream>
#include <QtWidgets/QToolBar>

MatchPanel::MatchPanel(QWidget* parent) : QWidget(parent)
{
  m_ui.setupUi(this);
  hide();

  m_ui.layout->setContentsMargins(0,0,0,0);
  QToolBar* tb = new QToolBar("Match Results", this);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16, 16));
  m_ui.layout->insertWidget(0, tb);
  QAction* a = tb->addAction("Test");
  a->setIcon(QIcon("res/glyphicons-320-sort.png"));
  QAction* b = tb->addAction("Other");
  b->setIcon(QIcon("res/glyphicons-137-cogwheel.png"));
}

void MatchPanel::show(const ScannerRule::Ref rule, RulesetView::Ref view)
{
  std::stringstream ss;

  ss << "Matched identifier: " << rule->identifier << std::endl;
  ss << "Namespace: " << rule->ns << std::endl;

  ss << std::endl;
  ss << "Tags (" << rule->tags.size() << "):" << std::endl;
  BOOST_FOREACH(std::string tag, rule->tags) {
    ss << "  " << tag << std::endl;
  }
  ss << std::endl;

  ss << "Strings (" << std::dec << rule->strings.size() << "):" << std::endl;
  BOOST_FOREACH(ScannerRule::String::Ref s, rule->strings) {
    ss << "  Identifier: " << s->identifier << std::endl;
    ss << "  Value:";
    BOOST_FOREACH(uint8_t v, s->value) {
      ss << " " << std::hex << int(v);
    }
    ss << std::endl;
    ss << "  Matches (" << std::dec << s->matches.size() << "):" << std::endl;
    BOOST_FOREACH(ScannerRule::Match::Ref m, s->matches) {
      ss << "    Base:" << std::hex << m->base << std::endl;
      ss << "    Offset:" << std::hex << m->offset << std::endl;
      ss << "    Value:";
      BOOST_FOREACH(uint8_t v, m->data) {
        ss << " " << std::hex << int(v);
      }
      ss << std::endl;
    }
  }
  ss << std::endl;

  ss << "Metas (" << std::dec << rule->metas.size() << std::endl;
  BOOST_FOREACH(ScannerRule::Meta::Ref m, rule->metas) {
    ss << "  Identifier: " << m->identifier << std::endl;
    ss << "  Value: " << m->value << std::endl;
  }

  m_ui.info->setPlainText(ss.str().c_str());

  QWidget::show();
}
