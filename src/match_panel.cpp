#include "match_panel.h"
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <QtWidgets/QToolBar>
#include <QtGui/QClipboard>

#ifdef WIN32
  #undef min
  #undef max
#endif

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
  m_stringsButton->setIcon(QIcon(":/glyphicons-120-table.png"));

  m_metaButton = tb->addAction("Metas");
  connect(m_metaButton, SIGNAL(triggered()), this, SLOT(showMeta()));
  m_metaButton->setIcon(QIcon(":/glyphicons-67-tags.png"));

  m_ui.table->verticalHeader()->hide();
  m_ui.table->setContextMenuPolicy(Qt::ActionsContextMenu);
  m_copyMenuAction = new QAction("&Copy", this);
  m_copyMenuAction->setIcon(QIcon::fromTheme("edit-copy"));
  m_copyMenuAction->setEnabled(false);
  connect(m_copyMenuAction, SIGNAL(triggered()), this, SLOT(handleCopyItemClicked()));
  m_ui.table->addAction(m_copyMenuAction);

  connect(m_ui.table, SIGNAL(itemSelectionChanged()), this, SLOT(handleSelectionChanged()));
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

  m_ui.table->setSortingEnabled(false);
  m_ui.table->setRowCount(0);
  m_ui.table->setColumnCount(3);
  QStringList headerLabels;
  headerLabels << "Offset" << "ID" << "Bytes";
  m_ui.table->setHorizontalHeaderLabels(headerLabels);
  m_ui.table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

  BOOST_FOREACH(ScannerRule::String::Ref string, m_rule->strings) {
    BOOST_FOREACH(ScannerRule::Match::Ref match, string->matches) {
      int row = m_ui.table->rowCount();
      m_ui.table->setRowCount(row + 1);

      std::stringstream offset;
      offset << "0x" << std::hex << std::uppercase << match->offset;
      QTableWidgetItem* offsetItem = new OffsetTableWidgetItem(match->offset);
      offsetItem->setText(offset.str().c_str());
      offsetItem->setFlags(offsetItem->flags() & ~Qt::ItemIsEditable);
      m_ui.table->setItem(row, 0, offsetItem);

      QTableWidgetItem* identifierItem = new QTableWidgetItem(string->identifier.c_str());
      identifierItem->setFlags(identifierItem->flags() & ~Qt::ItemIsEditable);
      m_ui.table->setItem(row, 1, identifierItem);

      std::stringstream bytes;
      size_t maxBytes = std::min(match->data.size(), size_t(32));
      for (size_t i = 0; i < maxBytes; ++i) {
        bytes << (i != 0 ? " " : "") << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << int(match->data[i]);
      }
      if (maxBytes != match->data.size()) {
        bytes << "...";
      }
      QTableWidgetItem* bytesItem = new QTableWidgetItem(bytes.str().c_str());
      bytesItem->setFlags(bytesItem->flags() & ~Qt::ItemIsEditable);
      m_ui.table->setItem(row, 2, bytesItem);
    }
  }

  m_ui.table->setSortingEnabled(true);
  m_ui.table->sortItems(0);
  m_ui.table->setSortingEnabled(false);
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
  headerLabels << "Name" << "Value";
  m_ui.table->setHorizontalHeaderLabels(headerLabels);
  m_ui.table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  BOOST_FOREACH(ScannerRule::Meta::Ref meta, m_rule->metas) {
    int row = m_ui.table->rowCount();
    m_ui.table->setRowCount(row + 1);
    QTableWidgetItem* identifierItem = new QTableWidgetItem(meta->identifier.c_str());
    identifierItem->setFlags(identifierItem->flags() & ~Qt::ItemIsEditable);
    m_ui.table->setItem(row, 0, identifierItem);

    QTableWidgetItem* valueItem = new QTableWidgetItem(meta->value.c_str());
    valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
    m_ui.table->setItem(row, 1, valueItem);
  }
}

void MatchPanel::handleSelectionChanged()
{
  if (m_ui.table->selectedItems().size()) {
    m_copyMenuAction->setEnabled(true);
  } else {
    m_copyMenuAction->setEnabled(false);
  }
}

void MatchPanel::handleCopyItemClicked()
{
  QList<QTableWidgetItem*> items = m_ui.table->selectedItems();
  if (items.size() != 1) {
    return;
  }
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->clear();
  clipboard->setText(items[0]->text());
}
