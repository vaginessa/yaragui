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

private slots:

  void showStrings();
  void showMeta();

  void handleSelectionChanged();
  void handleCopyItemClicked();

private:

  /* necessary to sort the items in the table */
  class OffsetTableWidgetItem : public QTableWidgetItem
  {
  public:
    OffsetTableWidgetItem(uint64_t offset) : m_offset(offset)
    {
    }
    virtual bool operator <(const QTableWidgetItem &rhs) const
    {
      OffsetTableWidgetItem const *item = dynamic_cast<OffsetTableWidgetItem const*>(&rhs);
      if(item) {
        return m_offset < item->m_offset;
      } else {
          return QTableWidgetItem::operator<(rhs);
      }
    }
    uint64_t m_offset;
  };

  enum Mode
  {
    ModeStrings,
    ModeMeta,
  };

  Ui::MatchPanel m_ui;
  Mode m_mode;
  ScannerRule::Ref m_rule;
  RulesetView::Ref m_view;

  QAction* m_copyMenuAction;
  QAction* m_stringsButton;
  QAction* m_metaButton;

};

#endif // __MATCH_PANEL_H__
