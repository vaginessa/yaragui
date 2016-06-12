#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "ui_main_window.h"
#include "target_panel.h"
#include "match_panel.h"
#include "ruleset_view.h"
#include "scanner_rule.h"
#include <boost/signals2.hpp>
#include <boost/asio/io_service.hpp>
#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QFileIconProvider>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  MainWindow(boost::asio::io_service& io);

  boost::signals2::signal<void (const std::vector<std::string>& files)> onChangeTargets;
  boost::signals2::signal<void (RulesetView::Ref ruleset)> onChangeRuleset;
  boost::signals2::signal<void ()> onScanAbort;
  boost::signals2::signal<void ()> onRequestRuleWindowOpen;
  boost::signals2::signal<void ()> onRequestAboutWindowOpen;

  void scanBegin();
  void scanEnd();
  void setRules(const std::vector<RulesetView::Ref>& rules);
  void addScanResult(const std::string& target, ScannerRule::Ref rule, RulesetView::Ref view);

private slots:

  void handleSelectRuleAllFromMenu();
  void handleSelectRuleFromMenu(int rule);
  void handleTargetFileBrowse();
  void handleTargetDirectoryBrowse();
  void handleRuleFileBrowse();
  void handleEditRulesMenu();
  void handleAboutMenu();
  void treeItemSelectionChanged();
  void handleScanTimer();
  void handleScanAbortButton();

private:

  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);

  boost::asio::io_service& m_io;

  Ui::MainWindow m_ui;
  QLabel* m_status;
  QToolButton* m_stopButton;
  TargetPanel* m_targetPanel;
  MatchPanel* m_matchPanel;
  QSignalMapper* m_signalMapper;

  QTimer* m_scanTimer;
  int m_scanPhase;
  bool m_scanAborted;

  QFileIconProvider m_iconProvider;

  std::vector<RulesetView::Ref> m_rules;

  std::map<std::string, QTreeWidgetItem*> m_treeItems;
  std::map<QTreeWidgetItem*, std::string> m_targetMap;
  std::map<QTreeWidgetItem*, ScannerRule::Ref> m_scannerRuleMap;
  std::map<QTreeWidgetItem*, RulesetView::Ref> m_rulesetViewMap;

};

#endif // __MAIN_WINDOW_H__
