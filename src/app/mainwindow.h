#pragma once

#include <QDialogButtonBox>
#include <QMainWindow>
#include <qglobal.h>
#include <qmdiarea.h>
#include <qstandarditemmodel.h>
#include <qtableview.h>
#include <qtablewidget.h>
#include <qtoolbutton.h>

#include "context-menu/treeContextMenu.h"
#include "femtypes.h"

class mesh;
class Qtgl;
class Solver;
class QTableWidget;
class QStandardItemModel;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  QVector<shared_ptr<AbstractElement>> elements;

  MainWindow();
  QMdiArea *m_pma;
  Qtgl *scene{nullptr};
  Mesh *mesh{nullptr};
  Solver *solver{nullptr};
  QVector<QToolButton *> resultButtons;
  QLabel *statusLabel;
  QToolButton *tablesBtn;

  ~MainWindow();

private:
  // Result table vars
  // QStandardItemModel *model{nullptr};
  QTableWidget *resultsView{nullptr};
  shared_ptr<AbstractElement> selectedELement{nullptr};

  void createLeftDock();
  void createMenus();
  void createToolBar();
  void createToolStrip();
  QToolBar *createToolBarFromWidget(QWidget *widget);
  QTreeWidget *treeWidget;
  TreeContextMenu *treeContextMenu;
  void setupTreeContextMenu();
  void getResultTable(shared_ptr<AbstractElement> selectedELement,
                      int selectedId);
  void setSpanResultTable(shared_ptr<AbstractElement> selectedELement,
                          QTableView *resultsView);

private slots:
  void onTreeContextMenuRequested(const QPoint &pos);
  void createDefaultPlateScheme(QTreeWidgetItem *item);
  void calculateButtonClicked();
  void createTableResultsTab();
  void copySelectionToClipboard();

  // Result table
  void updateResultTable();
};
