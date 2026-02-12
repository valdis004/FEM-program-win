#include "main_window.h"

#include <qapplication.h>
#include <qboxlayout.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qdebug.h>
#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qdockwidget.h>
#include <qglobal.h>
#include <qgridlayout.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qshortcut.h>
#include <qstandarditemmodel.h>
#include <qtablewidget.h>
#include <qtabwidget.h>
#include <qthread.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qwidget.h>

#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QMdiArea>
#include <QMenuBar>
#include <QObject>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QTableView>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <cstddef>
#include <memory>

#include "fem_types.h"
#include "graphics/qtgl/qtgl.h"
#include "solver/solver.h"
#include "structural_element.h"

MainWindow::MainWindow() {
  setWindowTitle("Fem test");
  resize(1000, 600);
  createMenus();
  createToolBar();
  createLeftDock();
  createToolStrip();
  setupTreeContextMenu();
}

MainWindow::~MainWindow() {
  delete scene_;
  delete solver_;
  delete mesh;
}

void MainWindow::createLeftDock() {
  // Центральный виджет - текстовый редактор
  mid_area_ = new QMdiArea(this);
  mid_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mid_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scene_ = new Qtgl(mid_area_);
  scene_->setWindowTitle("Graphic window");
  mid_area_->addSubWindow(scene_);
  mid_area_->setViewMode(QMdiArea::TabbedView);
  mid_area_->setTabsClosable(false);  // Добавить кнопки закрытия
  mid_area_->setTabsMovable(true);    // Возможность перемещать вкладки

  setCentralWidget(mid_area_);

  // Док-виджет слева (Дерево)
  QDockWidget* leftDock = new QDockWidget("Tree wiev", this);
  leftDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  tree_widget_ = new QTreeWidget();
  tree_widget_->setHeaderLabel("");
  tree_widget_->setContextMenuPolicy(Qt::CustomContextMenu);

  QTreeWidgetItem* root =
      new QTreeWidgetItem(tree_widget_, QStringList("Plates"));

  connect(tree_widget_, &QTreeWidget::customContextMenuRequested, this,
          &MainWindow::onTreeContextMenuRequested);

  leftDock->setWidget(tree_widget_);
  leftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, leftDock);
}

void MainWindow::createMenus() {
  QMenuBar* menuBar = this->menuBar();

  QMenu* fileMenu = menuBar->addMenu("Настройки");
  QMenu* viewMenu = menuBar->addMenu("&Вид");
}

void MainWindow::createToolBar() {
  // Заменяем тулбар на виджет с вкладками
  QTabWidget* toolTabs = new QTabWidget();
  toolTabs->setTabPosition(QTabWidget::North);
  toolTabs->setDocumentMode(true);

  // ========== Первая вкладка: Основные операции ==========
  QWidget* mainTab = new QWidget();
  QVBoxLayout* mainLayout = new QVBoxLayout(mainTab);
  mainLayout->setSpacing(2);
  mainLayout->setContentsMargins(2, 2, 2, 2);

  // Первый ряд кнопок
  QHBoxLayout* row1Layout = new QHBoxLayout();
  row1Layout->setSpacing(2);

  QToolButton* newBtn = new QToolButton(mainTab);
  newBtn->setFixedHeight(30);
  newBtn->setAutoRaise(true);
  newBtn->setText("Calculate");
  row1Layout->addWidget(newBtn);
  connect(newBtn, &QToolButton::clicked, this,
          &MainWindow::calculateButtonClicked);

  QToolButton* addElementButton = new QToolButton(mainTab);
  addElementButton->setFixedHeight(30);
  addElementButton->setAutoRaise(true);
  addElementButton->setText("Add element");
  row1Layout->addWidget(addElementButton);
  connect(addElementButton, &QToolButton::clicked, this, [this]() {
    this->tree_context_menu_->createDiologDefualtSchemePlate(
        &this->elements, this, this->scene_, this->mesh);
  });

  row1Layout->addSpacing(10);  // Небольшой разделитель
  row1Layout->addStretch();

  // Второй ряд кнопок
  QHBoxLayout* row2Layout = new QHBoxLayout();
  row2Layout->setSpacing(2);
  row2Layout->addStretch();

  mainLayout->addLayout(row1Layout);
  mainLayout->addLayout(row2Layout);

  // ========== Вторая вкладка: Форматирование ==========
  QWidget* resultWindget = new QWidget();
  QHBoxLayout* hlayout = new QHBoxLayout(resultWindget);
  resultWindget->setLayout(hlayout);

  QGridLayout* structuralForsesLayout = new QGridLayout();
  structuralForsesLayout->setContentsMargins(5, 5, 5, 5);
  structuralForsesLayout->setSpacing(5);
  structuralForsesLayout->setAlignment(Qt::AlignLeft);

  QToolButton* Ux = new QToolButton(resultWindget);
  Ux->setFixedHeight(30);
  Ux->setAutoRaise(true);
  Ux->setText("U_x");

  QToolButton* Uy = new QToolButton(resultWindget);
  Uy->setFixedHeight(30);
  Uy->setAutoRaise(true);
  Uy->setText("U_y");

  QToolButton* Uz = new QToolButton(resultWindget);
  Uz->setFixedHeight(30);
  Uz->setAutoRaise(true);
  Uz->setText("U_z");
  connect(Uz, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Uz];
    scene_->setResulthIndex(this, index);
  });

  QToolButton* Rx = new QToolButton(resultWindget);
  Rx->setFixedHeight(30);
  Rx->setAutoRaise(true);
  Rx->setText("R_x");
  connect(Rx, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Rx];
    scene_->setResulthIndex(this, index);
  });

  QToolButton* Ry = new QToolButton(resultWindget);
  Ry->setFixedHeight(30);
  Ry->setAutoRaise(true);
  Ry->setText("R_y");
  connect(Ry, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Ry];
    scene_->setResulthIndex(this, index);
  });

  QToolButton* Rz = new QToolButton(resultWindget);
  Rz->setFixedHeight(30);
  Rz->setAutoRaise(true);
  Rz->setText("R_z");

  QToolButton* Nx = new QToolButton(resultWindget);
  Nx->setFixedHeight(30);
  Nx->setAutoRaise(true);
  Nx->setText("N_x");

  QToolButton* Ny = new QToolButton(resultWindget);
  Ny->setFixedHeight(30);
  Ny->setAutoRaise(true);
  Ny->setText("N_y");

  QToolButton* Nxy = new QToolButton(resultWindget);
  Nxy->setFixedHeight(30);
  Nxy->setAutoRaise(true);
  Nxy->setText("N_xy");

  QToolButton* Qx = new QToolButton(resultWindget);
  Qx->setFixedHeight(30);
  Qx->setAutoRaise(true);
  Qx->setText("Q_x");
  connect(Qx, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Qx];
    scene_->setResulthIndex(this, index);
  });

  QToolButton* Qy = new QToolButton(resultWindget);
  Qy->setFixedHeight(30);
  Qy->setAutoRaise(true);
  Qy->setText("Q_y");
  connect(Qy, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Qy];
    scene_->setResulthIndex(this, index);
  });

  QToolButton* Mx = new QToolButton(resultWindget);
  Mx->setFixedHeight(30);
  Mx->setAutoRaise(true);
  Mx->setText("M_x");
  connect(Mx, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Mx];
    scene_->setResulthIndex(this, index);
  });

  QToolButton* My = new QToolButton(resultWindget);
  My->setFixedHeight(30);
  My->setAutoRaise(true);
  My->setText("M_y");
  connect(My, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::My];
    scene_->setResulthIndex(this, index);
  });

  QToolButton* Mxy = new QToolButton(resultWindget);
  Mxy->setFixedHeight(30);
  Mxy->setAutoRaise(true);
  Mxy->setText("M_xy");
  connect(Mxy, &QToolButton::clicked, this, [this]() {
    ElementType type = solver_->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Mxy];
    scene_->setResulthIndex(this, index);
  });

  result_buttons_ = QVector<QToolButton*>{
      Ux, Uy, Uz, Rx, Ry, Rz, Nx, Ny, Nxy, Qx, Qy, Mx, My, Mxy,
  };

  for (auto& cutBtn : result_buttons_) {
    cutBtn->setDisabled(true);
  }

  structuralForsesLayout->addWidget(Ux, 0, 0);
  structuralForsesLayout->addWidget(Uy, 1, 0);
  structuralForsesLayout->addWidget(Uz, 0, 1);

  structuralForsesLayout->addWidget(Rx, 0, 2);
  structuralForsesLayout->addWidget(Ry, 1, 2);
  structuralForsesLayout->addWidget(Rz, 0, 3);

  structuralForsesLayout->addWidget(Nx, 0, 4);
  structuralForsesLayout->addWidget(Ny, 1, 4);
  structuralForsesLayout->addWidget(Nxy, 0, 5);
  structuralForsesLayout->addWidget(Qx, 0, 6);
  structuralForsesLayout->addWidget(Qy, 1, 6);
  structuralForsesLayout->addWidget(Mx, 0, 7);
  structuralForsesLayout->addWidget(My, 1, 7);
  structuralForsesLayout->addWidget(Mxy, 0, 8);
  hlayout->addLayout(structuralForsesLayout);

  QGridLayout* tablesLayout = new QGridLayout();

  tables_btn_ = new QToolButton(resultWindget);
  tables_btn_->setDisabled(true);
  tables_btn_->setAutoRaise(true);
  tables_btn_->setText("Tabels");
  tablesLayout->addWidget(tables_btn_, 0, 0);
  connect(tables_btn_, &QToolButton::clicked, this,
          &MainWindow::createTableResultsTab);

  hlayout->addLayout(tablesLayout);

  // Добавляем вкладки
  toolTabs->addTab(mainTab, "Основные");
  toolTabs->addTab(resultWindget, "Результаты");

  // Устанавливаем панель с вкладками в качестве тулбара
  addToolBar(Qt::TopToolBarArea, createToolBarFromWidget(toolTabs));
}

// Вспомогательная функция для создания тулбара из виджета
QToolBar* MainWindow::createToolBarFromWidget(QWidget* widget) {
  QToolBar* toolBar = new QToolBar("Панель вкладок");
  toolBar->setMovable(false);  // Можно сделать фиксированной
  toolBar->setAllowedAreas(Qt::TopToolBarArea);
  toolBar->addWidget(widget);
  return toolBar;
}

void MainWindow::createToolStrip() {
  QStatusBar* statusBar = this->statusBar();

  // Статусные метки
  status_label_ = new QLabel();
  statusBar->addWidget(status_label_);

  // Прогресс-бар в статус-баре
  QProgressBar* progressBar = new QProgressBar();
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->setFixedWidth(150);
  progressBar->setVisible(false);  // По умолчанию скрыт
  statusBar->addPermanentWidget(progressBar);
}

void MainWindow::setupTreeContextMenu() {
  // Создаем структурированное меню
  tree_context_menu_ = new TreeContextMenu(tree_widget_, this);

  // Подключаем сигналы к слотам
  connect(tree_context_menu_, &TreeContextMenu::createDefaultPlateScheme, this,
          &MainWindow::createDefaultPlateScheme);
}

void MainWindow::onTreeContextMenuRequested(const QPoint& pos) {
  if (tree_context_menu_) {
    tree_context_menu_->showMenu(pos);
  }
}

// Функция, отрабатываемая при нажатии кнопки "Create default scheme" У treewiev
// для plate
void MainWindow::createDefaultPlateScheme(QTreeWidgetItem* item) {
  tree_context_menu_->createDiologDefualtSchemePlate(&elements, this, scene_,
                                                     mesh);
}

void MainWindow::calculateButtonClicked() {
  if (!this->mesh) {
    return;
  }

  QProgressDialog* progressBar = new QProgressDialog(this);
  progressBar->setWindowTitle("Calculating...");
  progressBar->setLabelText("Initializing calculation...");
  progressBar->setModal(true);
  progressBar->setRange(0, 0);
  progressBar->setMinimumDuration(0);
  progressBar->show();

  Solver* solver = new Solver();
  this->solver_ = solver;

  // Создаем отдельный поток
  QThread* workerThread = new QThread();

  // Перемещаем solver в рабочий поток
  solver->moveToThread(workerThread);

  connect(
      solver, &Solver::newElementStiffMatrixStep, this,
      [progressBar](unsigned count) {
        progressBar->setLabelText(
            QString("Creating local stiff matrix for element %1").arg(count));
      });

  connect(solver, &Solver::applyBaundaryConditionsStep, this, [progressBar]() {
    progressBar->setLabelText("Applying boundary conditions...");
  });
  connect(solver, &Solver::solveSystemStep, this,
          [progressBar]() { progressBar->setLabelText("Solving system..."); });
  connect(solver, &Solver::getOutputStep, this, [progressBar]() {
    progressBar->setLabelText("Getting output values...");
  });
  connect(solver, &Solver::calcFinishedStep, this, [progressBar]() {
    progressBar->setLabelText("Calculated successfully");
    progressBar->close();
  });
  connect(workerThread, &QThread::started, solver,
          [solver, this]() { solver->calculate(this->elements); });

  connect(solver, &Solver::calcFinishedStep, this,
          [this, solver, workerThread, progressBar]() {
            // Этот код выполнится в главном потоке
            this->scene_->setResulthData(solver->maxAbsValues,
                                         solver->maxValues, solver->minValues);

            const auto* elems = solver->elements;

            for (const auto& element : *elems) {
              auto data = ElementProvider.at(element->getType());
              for (size_t i = 0; i < data.STR_OUTPUT_VALUES.size(); i++) {
                for (size_t j = 0; j < this->result_buttons_.size(); j++) {
                  if (data.STR_OUTPUT_VALUES[i] ==
                      this->result_buttons_[j]->text()) {
                    this->result_buttons_[j]->setEnabled(true);
                    break;
                  }
                }
              }
            }

            // Завершаем поток
            workerThread->quit();
          });
  connect(solver, &Solver::calcFinishedStep, tables_btn_,
          [this]() { tables_btn_->setEnabled(true); });

  // Убираем объекты при завершении
  connect(workerThread, &QThread::finished, workerThread,
          &QThread::deleteLater);

  // Запускаем поток
  workerThread->start();
  progressBar->show();
}

// Creating the result tab
void MainWindow::createTableResultsTab() {
  QWidget* tableWindow = new QWidget();
  QVBoxLayout* layout =
      new QVBoxLayout(tableWindow);  // Создаем layout для виджета

  QHBoxLayout* hLayout =
      new QHBoxLayout(tableWindow);  // Создаем layout для виджета

  // model = new QTableWidgetItemModel();

  QComboBox* comboBoxForResultType = new QComboBox(tableWindow);

  QComboBox* comboBoxForSelectedElement = new QComboBox(tableWindow);
  for (const auto& element : elements) {
    auto type = element->getType();
    auto DATA = ElementProvider.at(type);
    comboBoxForSelectedElement->addItem(element->name_);
  }

  selected_eLement_ = elements.first();
  // for (const auto &element : elements) {
  //   if (element->name ==
  //   comboBoxForSelectedElement->currentData().toString()) {
  //     selectedELement = element;

  //     auto type = element->getType();
  //     auto DATA = ElementProvider::elementData[type];
  //     comboBoxForResultType->addItem("All results");
  //     for (const auto &resultName : DATA.STR_OUTPUT_VALUES) {
  //       comboBoxForResultType->addItem(resultName);
  //     }
  //   }
  // }

  auto element = elements.first();
  auto type = element->getType();
  auto DATA = ElementProvider.at(type);

  for (const auto& resultName : DATA.STR_OUTPUT_VALUES) {
    comboBoxForResultType->addItem(resultName);
  }
  comboBoxForResultType->addItem("All results");
  comboBoxForResultType->setCurrentIndex(DATA.OUTPUT_VALUES_COUNT);

  comboBoxForResultType->setFixedWidth(100);
  comboBoxForSelectedElement->setFixedWidth(100);
  layout->addLayout(hLayout);
  hLayout->addWidget(comboBoxForResultType);
  hLayout->addWidget(comboBoxForSelectedElement);

  results_view_ = new QTableWidget();
  results_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
  results_view_->setSelectionBehavior(QAbstractItemView::SelectItems);
  results_view_->setSelectionMode(QAbstractItemView::MultiSelection);
  results_view_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+C"), results_view_);
  connect(shortcut, &QShortcut::activated, this,
          &MainWindow::copySelectionToClipboard);

  getResultTable(selected_eLement_, DATA.OUTPUT_VALUES_COUNT);

  setSpanResultTable(selected_eLement_, results_view_);

  layout->addWidget(results_view_);  // Добавляем таблицу в layout
  tableWindow->setLayout(layout);    // Устанавливаем layout для виджета

  // Устанавливаем заголовок для подокна (вкладки)
  tableWindow->setWindowTitle(
      "Tables " + QString::number(mid_area_->subWindowList().count() + 1));

  // Добавляем подокно в MDI-область
  mid_area_->addSubWindow(tableWindow);

  connect(comboBoxForResultType, &QComboBox::currentIndexChanged, this,
          &MainWindow::updateResultTable);

  tableWindow->show();
}

// Initialize s default result table with all results type
void MainWindow::getResultTable(shared_ptr<AStructuralElement> selected_eLement,
                                int selected_id) {
  auto& mesh = selected_eLement->meshData_;
  auto DATA = ElementProvider.at(selected_eLement->getType());
  auto nodes = mesh->nodes_;
  results_view_->setColumnCount(5 + DATA.OUTPUT_VALUES_COUNT);
  results_view_->setRowCount(nodes.size());
  short outputValuesCount = DATA.OUTPUT_VALUES_COUNT;

  results_view_->setHorizontalHeaderItem(0, new QTableWidgetItem("Elements"));
  results_view_->setHorizontalHeaderItem(1, new QTableWidgetItem("Nodes"));
  results_view_->setHorizontalHeaderItem(2, new QTableWidgetItem("x"));
  results_view_->setHorizontalHeaderItem(3, new QTableWidgetItem("y"));
  results_view_->setHorizontalHeaderItem(4, new QTableWidgetItem("z"));

  if (selected_id == outputValuesCount) {
    for (size_t i = 0; i < DATA.STR_OUTPUT_VALUES.size(); i++) {
      results_view_->setHorizontalHeaderItem(
          i + 5, new QTableWidgetItem(DATA.STR_OUTPUT_VALUES[i]));
    }

    for (size_t i = 0; i < nodes.size(); i++) {
      int elementId = i / DATA.NODES_COUNT;
      results_view_->setItem(
          i, 0, new QTableWidgetItem("Element " + QString::number(elementId)));
      results_view_->setItem(
          i, 1, new QTableWidgetItem("Node " + QString::number(nodes[i]->id)));
      results_view_->setItem(
          i, 2, new QTableWidgetItem(QString::number(nodes[i]->point.x)));
      results_view_->setItem(
          i, 3, new QTableWidgetItem(QString::number(nodes[i]->point.y)));
      results_view_->setItem(
          i, 4, new QTableWidgetItem(QString::number(nodes[i]->point.z)));

      for (size_t j = 0; j < DATA.OUTPUT_VALUES_COUNT; j++) {
        results_view_->setItem(
            i, j + 5,
            new QTableWidgetItem(QString::number(nodes[i]->outputValues[j])));
      }
    }

    return;
  }

  results_view_->setHorizontalHeaderItem(
      4, new QTableWidgetItem(DATA.STR_OUTPUT_VALUES[selected_id]));

  for (size_t i = 0; i < nodes.size(); i++) {
    int elementId = i / DATA.NODES_COUNT;
    results_view_->setItem(
        i, 0, new QTableWidgetItem("Element " + QString::number(elementId)));
    results_view_->setItem(
        i, 1, new QTableWidgetItem(QString::number(nodes[i]->point.x)));
    results_view_->setItem(
        i, 2, new QTableWidgetItem(QString::number(nodes[i]->point.y)));
    results_view_->setItem(
        i, 3, new QTableWidgetItem(QString::number(nodes[i]->point.z)));
    results_view_->setItem(i, 4,
                           new QTableWidgetItem(QString::number(
                               nodes[i]->outputValues[selected_id])));
  }
}

// Span element rows
void MainWindow::setSpanResultTable(
    shared_ptr<AStructuralElement> selected_eLement, QTableView* results_view) {
  auto& mesh = selected_eLement->meshData_;
  auto DATA = ElementProvider.at(selected_eLement->getType());
  auto nodes = mesh->nodes_;

  for (size_t i = 0; i < nodes.size(); i++) {
    // int elementId = i / DATA.NODES_COUNT;
    results_view->setSpan(i * DATA.NODES_COUNT, 0, DATA.NODES_COUNT, 1);
  }
}

// Slot that activates when comboBox selection changes after initializing the
// default result table
void MainWindow::updateResultTable() {
  QComboBox* comboBox = (QComboBox*)sender();
  int selectedIndex = comboBox->currentIndex();
  auto& mesh = selected_eLement_->meshData_;
  auto DATA = ElementProvider.at(selected_eLement_->getType());
  short outputValuesCount = DATA.OUTPUT_VALUES_COUNT;
  const auto& nodes = mesh->nodes_;

  // If selected result type == All results (that have last index)
  if (selectedIndex == outputValuesCount) {
    for (size_t i = 0; i < outputValuesCount; i++) {
      results_view_->setColumnHidden(i + 5, false);
      QString strOutput = DATA.STR_OUTPUT_VALUES[i];
      results_view_->setHorizontalHeaderItem(
          i + 5, new QTableWidgetItem(DATA.STR_OUTPUT_VALUES[i]));
    }

    for (size_t i = 0; i < nodes.size(); i++) {
      for (size_t j = 0; j < DATA.OUTPUT_VALUES_COUNT; j++) {
        results_view_->setItem(
            i, j + 5,
            new QTableWidgetItem(QString::number(nodes[i]->outputValues[j])));
      }
    }
  }
  // If selected result type is not all results
  else {
    QString strOutput = DATA.STR_OUTPUT_VALUES[selectedIndex];
    results_view_->setHorizontalHeaderItem(5, new QTableWidgetItem(strOutput));

    for (size_t i = 0; i < nodes.size(); i++) {
      for (size_t j = 0; j < DATA.OUTPUT_VALUES_COUNT; j++) {
        results_view_->setItem(i, 5,
                               new QTableWidgetItem(QString::number(
                                   nodes[i]->outputValues[selectedIndex])));
      }
    }

    for (size_t i = 1; i < outputValuesCount; i++) {
      results_view_->setColumnHidden(i + 5, true);
    }
  }
}

void MainWindow::copySelectionToClipboard() {
  // Получаем список всех выделенных элементов
  QList<QTableWidgetItem*> selectedItems = results_view_->selectedItems();

  if (selectedItems.isEmpty()) return;  // Ничего не выделено

  // Определяем границы выделенного диапазона
  int minRow = INT_MAX, maxRow = INT_MIN;
  int minCol = INT_MAX, maxCol = INT_MIN;

  foreach (QTableWidgetItem* item, selectedItems) {
    minRow = qMin(minRow, item->row());
    maxRow = qMax(maxRow, item->row());
    minCol = qMin(minCol, item->column());
    maxCol = qMax(maxCol, item->column());
  }

  // Создаем карту для группировки элементов по строкам
  QMap<int, QMap<int, QString>> cellTexts;
  foreach (QTableWidgetItem* item, selectedItems) {
    cellTexts[item->row()][item->column()] = item->text();
  }

  // Формируем строку для буфера обмена
  QString clipboardText;
  for (int row = minRow; row <= maxRow; ++row) {
    if (row != minRow) clipboardText += "\n";  // Новая строка

    for (int col = minCol; col <= maxCol; ++col) {
      if (col != minCol) clipboardText += "\t";  // Табуляция между столбцами

      // Добавляем текст ячейки или пустую строку
      if (cellTexts.contains(row) && cellTexts[row].contains(col)) {
        clipboardText += cellTexts[row][col];
      } else {
        clipboardText += "";  // Пустая ячейка в выделенном диапазоне
      }
    }
  }

  // Копируем в буфер обмена
  QApplication::clipboard()->setText(clipboardText);

  // Для отладки
  qDebug() << "Скопировано:" << selectedItems.size() << "ячеек";
  qDebug() << "Диапазон: строки" << minRow << "-" << maxRow << ", столбцы"
           << minCol << "-" << maxCol;
}