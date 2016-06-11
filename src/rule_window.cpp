#include "rule_window.h"
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <QtWidgets/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtCore/QMimeData>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

RuleWindow::RuleWindow()
{
  m_ui.setupUi(this);
  setAcceptDrops(true); /* enable drag and drop */
  setWindowIcon(QIcon(":/yaragui.png"));

  connect(m_ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClicked(QAbstractButton*)));

  m_ui.table->setColumnCount(3);
  m_ui.table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_ui.table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

  connect(m_ui.table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleItemEdit(QTableWidgetItem*)));
  connect(m_ui.table, SIGNAL(itemSelectionChanged()), this, SLOT(handleSelectionChanged()));

  m_ui.table->verticalHeader()->hide();
  m_ui.table->setSelectionBehavior(QTableWidget::SelectRows);
  m_ui.table->setContextMenuPolicy(Qt::ActionsContextMenu);

  m_compileButton = new QAction("&Compile", this);
  m_compileButton->setEnabled(false);
  m_compileButton->setIcon(QIcon(":/glyphicons-82-refresh.png"));
  connect(m_compileButton, SIGNAL(triggered()), this, SLOT(handleCompileClicked()));
  m_ui.table->addAction(m_compileButton);

  m_moveUpButton = new QAction("Move &Up", this);
  m_moveUpButton->setEnabled(false);
  m_moveUpButton->setIcon(QIcon(":/glyphicons-214-arrow-up.png"));
  connect(m_moveUpButton, SIGNAL(triggered()), this, SLOT(handleMoveUpClicked()));
  m_ui.table->addAction(m_moveUpButton);

  m_moveDownButton = new QAction("Move &Down", this);
  m_moveDownButton->setEnabled(false);
  m_moveDownButton->setIcon(QIcon(":/glyphicons-213-arrow-down.png"));
  connect(m_moveDownButton, SIGNAL(triggered()), this, SLOT(handleMoveDownClicked()));
  m_ui.table->addAction(m_moveDownButton);

  m_removeButton = new QAction("&Remove", this);
  m_removeButton->setEnabled(false);
  m_removeButton->setIcon(QIcon(":/glyphicons-200-ban-circle.png"));
  connect(m_removeButton, SIGNAL(triggered()), this, SLOT(handleRemoveClicked()));
  m_ui.table->addAction(m_removeButton);

  m_iconYes = QPixmap(":/glyphicons-207-ok.png");
  m_iconYes = m_iconYes.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  m_iconNo = QPixmap(":/glyphicons-208-remove.png");
  m_iconNo = m_iconNo.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  show();
}

void RuleWindow::setRules(const std::vector<RulesetView::Ref>& rules)
{
  rulesToView(rules);
}

void RuleWindow::handleButtonClicked(QAbstractButton* button)
{
  QDialogButtonBox::StandardButton choice = m_ui.buttonBox->standardButton(button);
  switch (choice) {
  case QDialogButtonBox::Ok:
    onSaveRules(m_rules);
    close();
    break;
  case QDialogButtonBox::Apply:
    onSaveRules(m_rules);
    break;
  case QDialogButtonBox::Cancel:
    close();
    break;
  default:
    break;
  }
}

void RuleWindow::handleItemEdit(QTableWidgetItem* item)
{
  if (m_names.find(item) == m_names.end()) {
    return; /* not a name cell */
  }
  m_names[item]->setName(item->text().toStdString());
}

void RuleWindow::handleSelectionChanged()
{
  boost::optional<int> index = selectedItemIndex();
  if (!index) {
    m_moveUpButton->setEnabled(false);
    m_moveDownButton->setEnabled(false);
  } else {
    if (*index == 0) {
      m_moveUpButton->setEnabled(false);
    } else {
      m_moveUpButton->setEnabled(true);
    }
    if (*index == m_rules.size() - 1) {
      m_moveDownButton->setEnabled(false);
    } else {
      m_moveDownButton->setEnabled(true);
    }
  }

  std::vector<RulesetView::Ref> items = selectedItems();
  m_removeButton->setEnabled(items.size() != 0);
  m_compileButton->setEnabled(items.size() == 1);
}

void RuleWindow::handleCompileClicked()
{
  std::vector<RulesetView::Ref> rules = selectedItems();

  if (rules.size() != 1) {
    return; /* can only compile one rule at a time */
  }

  onCompileRule(rules[0]);
}

void RuleWindow::handleMoveUpClicked()
{
  boost::optional<int> index = selectedItemIndex();
  if (!index) {
    return; /* not a single selection */
  }
  std::vector<RulesetView::Ref> items = selectedItems();
  if (*index == 0) {
    return; /* can't move */
  }
  const int newIndex = *index - 1;
  std::swap(m_rules[*index], m_rules[newIndex]);
  rulesToView(m_rules);
  m_ui.table->selectRow(newIndex);
}

void RuleWindow::handleMoveDownClicked()
{
  boost::optional<int> index = selectedItemIndex();
  if (!index) {
    return; /* not a single selection */
  }
  std::vector<RulesetView::Ref> items = selectedItems();
  if (*index == m_rules.size() - 1) {
    return; /* can't move */
  }
  const int newIndex = *index + 1;
  std::swap(m_rules[*index], m_rules[newIndex]);
  rulesToView(m_rules);
  m_ui.table->selectRow(newIndex);
}

void RuleWindow::handleRemoveClicked()
{
  std::vector<RulesetView::Ref> rules = selectedItems();
  std::vector<RulesetView::Ref> newRules;
  BOOST_FOREACH(RulesetView::Ref old, m_rules) {
    bool found = false;
    BOOST_FOREACH(RulesetView::Ref erase, rules) {
      if (old->file() == erase->file()) {
        found = true;
        break;
      }
    }
    if (!found) {
      newRules.push_back(old);
    }
  }
  rulesToView(newRules);
}

void RuleWindow::dragEnterEvent(QDragEnterEvent* event)
{
  /* filter out non-files, like images dragged from the web browser */
  const QMimeData* mimeData = event->mimeData();
  if (!mimeData->hasUrls()) {
    event->ignore();
    return;
  }

  QList<QUrl> urls = mimeData->urls();
  for (int i = 0; i < urls.size(); ++i) {
    if (!urls[i].isLocalFile()) {
      event->ignore();
      return;
    }
  }

  event->acceptProposedAction();
}

void RuleWindow::dropEvent(QDropEvent* event)
{
  const QMimeData* mimeData = event->mimeData();
  QList<QUrl> urls = mimeData->urls();

  for (int i = 0; i < urls.size(); ++i) {
    QFileInfo fileInfo(urls[i].toLocalFile());
    QString file = QDir::toNativeSeparators(fileInfo.absoluteFilePath());
    if (fileInfo.suffix() == "yar" || fileInfo.suffix() == "yara") {
      bool duplicate = false;
      BOOST_FOREACH(RulesetView::Ref rule, m_rules) {
        if (rule->file() == file.toStdString()) {
          duplicate = true;
          break;
        }
      }
      if (!duplicate) {
        m_rules.push_back(boost::make_shared<RulesetView>(file.toStdString()));
      }
    }
  }

  rulesToView(m_rules);
  event->acceptProposedAction();
}

void RuleWindow::rulesToView(const std::vector<RulesetView::Ref>& rules)
{
  m_rules = rules;
  m_names.clear();
  m_ui.table->clear();
  m_ui.table->setRowCount(rules.size());

  QStringList headerLabels;
  headerLabels << "File" << "Name" << "Compiled";
  m_ui.table->setHorizontalHeaderLabels(headerLabels);

  for (size_t i = 0; i < rules.size(); ++i) {
    QTableWidgetItem* itemFile = new QTableWidgetItem(rules[i]->file().c_str());
    itemFile->setFlags(itemFile->flags() & ~Qt::ItemIsEditable);
    m_ui.table->setItem(int(i), 0, itemFile);

    QTableWidgetItem* itemName = new QTableWidgetItem(rules[i]->name().c_str());
    m_ui.table->setItem(int(i), 1, itemName);
    m_names[itemName] = rules[i];

    QLabel* itemCompiled = new QLabel(m_ui.table);
    itemCompiled->setAlignment(Qt::AlignCenter);
    if (rules[i]->isCompiled()) {
      itemCompiled->setPixmap(m_iconYes);
    } else {
      itemCompiled->setPixmap(m_iconNo);
    }
    m_ui.table->setCellWidget(int(i), 2, itemCompiled);
  }
}

std::vector<RulesetView::Ref> RuleWindow::selectedItems()
{
  std::vector<RulesetView::Ref> rules;
  QList<QTableWidgetItem*> items = m_ui.table->selectedItems();
  for (int i = 0; i < items.size(); ++i) {
    if (m_names.find(items[i]) != m_names.end()) {
      rules.push_back(m_names[items[i]]);
    }
  }
  return rules;
}

boost::optional<int> RuleWindow::selectedItemIndex()
{
  std::vector<RulesetView::Ref> rules = selectedItems();
  if (rules.size() != 1) {
    return boost::optional<int>();
  }
  for (size_t i = 0; i < m_rules.size(); ++i) {
    if (m_rules[i]->file() == rules[0]->file()) {
      return boost::optional<int>(i);
    }
  }
  return boost::optional<int>();
}

void RuleWindow::keyPressEvent(QKeyEvent *event)
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
