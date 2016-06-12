#include "match_panel.h"
#include <boost/foreach.hpp>
#include <sstream>
#include <iomanip>
#include <QtWidgets/QToolBar>

MatchPanel::MatchPanel(QWidget* parent) : QWidget(parent), m_mode(ModeStrings)
{
  m_ui.setupUi(this);
  hide();

  m_ui.layout->setContentsMargins(0,0,0,0);
  QToolBar* tb = new QToolBar("Match Results", this);
  tb->setOrientation(Qt::Vertical);
  tb->setIconSize(QSize(16, 16));
  m_ui.layout->insertWidget(0, tb);

  m_stringsButton = tb->addAction("Strings");
  connect(m_stringsButton, SIGNAL(triggered()), this, SLOT(showStrings()));
  m_stringsButton->setIcon(QIcon("res/glyphicons-120-table.png"));

  m_metaButton = tb->addAction("Metas");
  connect(m_metaButton, SIGNAL(triggered()), this, SLOT(showMeta()));
  m_metaButton->setIcon(QIcon("res/glyphicons-67-tags.png"));

  m_ui.table->verticalHeader()->hide();
}

void MatchPanel::show(const ScannerRule::Ref rule, RulesetView::Ref view)
{
  m_rule = rule;
  m_view = view;

  switch (m_mode) {
  case ModeStrings:
    showStrings();
    break;
  case ModeMeta:
    showMeta();
    break;
  default:
    break;
  }

  QWidget::show();
}

void MatchPanel::showStrings()
{
  m_mode = ModeStrings;
  m_stringsButton->setEnabled(false);
  m_metaButton->setEnabled(true);

  m_ui.table->setSortingEnabled(true);
  m_ui.table->setRowCount(0);
  m_ui.table->setColumnCount(3);
  QStringList headerLabels;
  headerLabels << "ID" << "Offset" << "Bytes";
  m_ui.table->setHorizontalHeaderLabels(headerLabels);
  m_ui.table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

  BOOST_FOREACH(ScannerRule::String::Ref string, m_rule->strings) {
    BOOST_FOREACH(ScannerRule::Match::Ref match, string->matches) {
      int row = m_ui.table->rowCount();
      m_ui.table->setRowCount(row + 1);

      m_ui.table->setItem(row, 0, new QTableWidgetItem(string->identifier.c_str()));

      std::stringstream offset;
      offset << "0x" << std::hex << std::setfill('0') << std::setw(8) << match->offset;
      m_ui.table->setItem(row, 1, new QTableWidgetItem(offset.str().c_str()));

      std::stringstream bytes;
      for (size_t i = 0; i < match->data.size(); ++i) {
        bytes << (i != 0 ? " " : "") << std::hex << std::setfill('0') << std::setw(2) << int(match->data[i]);
      }
      m_ui.table->setItem(row, 2, new QTableWidgetItem(bytes.str().c_str()));
    }
  }

  m_ui.table->sortItems(1);
}

void MatchPanel::showMeta()
{
  m_mode = ModeMeta;
  m_stringsButton->setEnabled(true);
  m_metaButton->setEnabled(false);

  m_ui.table->setSortingEnabled(false);
  m_ui.table->setRowCount(0);
  m_ui.table->setColumnCount(2);
  QStringList headerLabels;
  headerLabels << "ID" << "Value";
  m_ui.table->setHorizontalHeaderLabels(headerLabels);
  m_ui.table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  BOOST_FOREACH(ScannerRule::Meta::Ref meta, m_rule->metas) {
    int row = m_ui.table->rowCount();
    m_ui.table->setRowCount(row + 1);
    m_ui.table->setItem(row, 0, new QTableWidgetItem(meta->identifier.c_str()));
    m_ui.table->setItem(row, 1, new QTableWidgetItem(meta->value.c_str()));
  }
}
