#pragma once

#include <QDialogButtonBox>
#include <QMainWindow>
#include <qglobal.h>
#include <qmdiarea.h>
#include <qstandarditemmodel.h>
#include <qtableview.h>
#include <qtablewidget.h>
#include <qtoolbutton.h>

#include "context_menu/tree_context_menu.h"
// #include "femtypes.h"

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
  QMdiArea *mid_area_;
  Qtgl *scene_{nullptr};
  Mesh *mesh{nullptr};
  Solver *solver_{nullptr};
  QVector<QToolButton *> result_buttons_;
  QLabel *status_label_;
  QToolButton *tables_btn_;

  ~MainWindow();

private:
  // Result table vars
  // QStandardItemModel *model{nullptr};
  QTableWidget *results_view_{nullptr};
  shared_ptr<AbstractElement> selected_eLement_{nullptr};
  QTreeWidget *tree_widget_;
  TreeContextMenu *tree_context_menu_;

  void createLeftDock();
  void createMenus();
  void createToolBar();
  void createToolStrip();
  QToolBar *createToolBarFromWidget(QWidget *widget);
  void setupTreeContextMenu();
  void getResultTable(shared_ptr<AbstractElement> selected_eLement,
                      int selectedId);
  void setSpanResultTable(shared_ptr<AbstractElement> selected_eLement,
                          QTableView *results_view);

private slots:
  void onTreeContextMenuRequested(const QPoint &pos);
  void createDefaultPlateScheme(QTreeWidgetItem *item);
  void calculateButtonClicked();
  void createTableResultsTab();
  void copySelectionToClipboard();

  // Result table
  void updateResultTable();
};
