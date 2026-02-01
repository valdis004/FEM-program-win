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
// #include <stdexcept>

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/femtypes.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/mesh/mesh.h"
#include "context-menu/treeContextMenu.h"
#include "element.h"
#include "femtypes.h"
#include "graphics/qtgl/qtgl.h"
#include "mainwindow.h"
#include "solver/solver.h"

MainWindow::MainWindow() {
  setWindowTitle("Fem test");
  resize(1000, 600);
  createMenus();
  createToolBar();
  createLeftDock();
  createToolStrip();
  setupTreeContextMenu(); // Настройка контекстного меню для работы
}

MainWindow::~MainWindow() {
  delete scene;
  delete solver;
  delete mesh;
}

void MainWindow::createLeftDock() {
  // Центральный виджет - текстовый редактор
  m_pma = new QMdiArea(this);
  m_pma->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_pma->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scene = new Qtgl(m_pma);
  scene->setWindowTitle("Graphic window");
  m_pma->addSubWindow(scene);
  m_pma->setViewMode(QMdiArea::TabbedView);
  m_pma->setTabsClosable(false); // Добавить кнопки закрытия
  m_pma->setTabsMovable(true);   // Возможность перемещать вкладки

  setCentralWidget(m_pma);

  // Док-виджет слева (Дерево)
  QDockWidget *leftDock = new QDockWidget("Tree wiev", this);
  leftDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  treeWidget = new QTreeWidget();
  treeWidget->setHeaderLabel("");
  treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

  QTreeWidgetItem *root =
      new QTreeWidgetItem(treeWidget, QStringList("Plates"));

  connect(treeWidget, &QTreeWidget::customContextMenuRequested, this,
          &MainWindow::onTreeContextMenuRequested);

  leftDock->setWidget(treeWidget);
  leftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, leftDock);
}

void MainWindow::createMenus() {
  QMenuBar *menuBar = this->menuBar();

  QMenu *fileMenu = menuBar->addMenu("Настройки");
  QMenu *viewMenu = menuBar->addMenu("&Вид");
}

void MainWindow::createToolBar() {
  // Заменяем тулбар на виджет с вкладками
  QTabWidget *toolTabs = new QTabWidget();
  toolTabs->setTabPosition(QTabWidget::North);
  toolTabs->setDocumentMode(true);

  // ========== Первая вкладка: Основные операции ==========
  QWidget *mainTab = new QWidget();
  QVBoxLayout *mainLayout = new QVBoxLayout(mainTab);
  mainLayout->setSpacing(2);
  mainLayout->setContentsMargins(2, 2, 2, 2);

  // Первый ряд кнопок
  QHBoxLayout *row1Layout = new QHBoxLayout();
  row1Layout->setSpacing(2);

  QToolButton *newBtn = new QToolButton(mainTab);
  newBtn->setFixedHeight(30);
  newBtn->setAutoRaise(true);
  newBtn->setText("Calculate");
  row1Layout->addWidget(newBtn);
  connect(newBtn, &QToolButton::clicked, this,
          &MainWindow::calculateButtonClicked);

  QToolButton *addElementButton = new QToolButton(mainTab);
  addElementButton->setFixedHeight(30);
  addElementButton->setAutoRaise(true);
  addElementButton->setText("Add element");
  row1Layout->addWidget(addElementButton);
  connect(addElementButton, &QToolButton::clicked, this, [this]() {
    this->treeContextMenu->createDiologDefualtSchemePlate(
        &this->elements, this, this->scene, this->mesh);
  });

  row1Layout->addSpacing(10); // Небольшой разделитель
  row1Layout->addStretch();

  // Второй ряд кнопок
  QHBoxLayout *row2Layout = new QHBoxLayout();
  row2Layout->setSpacing(2);
  row2Layout->addStretch();

  mainLayout->addLayout(row1Layout);
  mainLayout->addLayout(row2Layout);

  // ========== Вторая вкладка: Форматирование ==========
  QWidget *resultWindget = new QWidget();
  QHBoxLayout *hlayout = new QHBoxLayout(resultWindget);
  resultWindget->setLayout(hlayout);

  QGridLayout *structuralForsesLayout = new QGridLayout();
  structuralForsesLayout->setContentsMargins(5, 5, 5, 5);
  structuralForsesLayout->setSpacing(5);
  structuralForsesLayout->setAlignment(Qt::AlignLeft);

  QToolButton *Ux = new QToolButton(resultWindget);
  Ux->setFixedHeight(30);
  Ux->setAutoRaise(true);
  Ux->setText("U_x");

  QToolButton *Uy = new QToolButton(resultWindget);
  Uy->setFixedHeight(30);
  Uy->setAutoRaise(true);
  Uy->setText("U_y");

  QToolButton *Uz = new QToolButton(resultWindget);
  Uz->setFixedHeight(30);
  Uz->setAutoRaise(true);
  Uz->setText("U_z");
  connect(Uz, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Uz];
    scene->setResulthIndex(this, index);
  });

  QToolButton *Rx = new QToolButton(resultWindget);
  Rx->setFixedHeight(30);
  Rx->setAutoRaise(true);
  Rx->setText("R_x");
  connect(Rx, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Rx];
    scene->setResulthIndex(this, index);
  });

  QToolButton *Ry = new QToolButton(resultWindget);
  Ry->setFixedHeight(30);
  Ry->setAutoRaise(true);
  Ry->setText("R_y");
  connect(Ry, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Ry];
    scene->setResulthIndex(this, index);
  });

  QToolButton *Rz = new QToolButton(resultWindget);
  Rz->setFixedHeight(30);
  Rz->setAutoRaise(true);
  Rz->setText("R_z");

  QToolButton *Nx = new QToolButton(resultWindget);
  Nx->setFixedHeight(30);
  Nx->setAutoRaise(true);
  Nx->setText("N_x");

  QToolButton *Ny = new QToolButton(resultWindget);
  Ny->setFixedHeight(30);
  Ny->setAutoRaise(true);
  Ny->setText("N_y");

  QToolButton *Nxy = new QToolButton(resultWindget);
  Nxy->setFixedHeight(30);
  Nxy->setAutoRaise(true);
  Nxy->setText("N_xy");

  QToolButton *Qx = new QToolButton(resultWindget);
  Qx->setFixedHeight(30);
  Qx->setAutoRaise(true);
  Qx->setText("Q_x");
  connect(Qx, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Qx];
    scene->setResulthIndex(this, index);
  });

  QToolButton *Qy = new QToolButton(resultWindget);
  Qy->setFixedHeight(30);
  Qy->setAutoRaise(true);
  Qy->setText("Q_y");
  connect(Qy, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Qy];
    scene->setResulthIndex(this, index);
  });

  QToolButton *Mx = new QToolButton(resultWindget);
  Mx->setFixedHeight(30);
  Mx->setAutoRaise(true);
  Mx->setText("M_x");
  connect(Mx, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Mx];
    scene->setResulthIndex(this, index);
  });

  QToolButton *My = new QToolButton(resultWindget);
  My->setFixedHeight(30);
  My->setAutoRaise(true);
  My->setText("M_y");
  connect(My, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::My];
    scene->setResulthIndex(this, index);
  });

  QToolButton *Mxy = new QToolButton(resultWindget);
  Mxy->setFixedHeight(30);
  Mxy->setAutoRaise(true);
  Mxy->setText("M_xy");
  connect(Mxy, &QToolButton::clicked, this, [this]() {
    ElementType type = solver->elements->first()->getType();
    short index =
        ElementProvider.at(type).OUTPUT_INDEX_MAP[(int)OutputType::Mxy];
    scene->setResulthIndex(this, index);
  });

  resultButtons = QVector<QToolButton *>{
      Ux, Uy, Uz, Rx, Ry, Rz, Nx, Ny, Nxy, Qx, Qy, Mx, My, Mxy,
  };

  for (auto &cutBtn : resultButtons) {
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

  QGridLayout *tablesLayout = new QGridLayout();

  tablesBtn = new QToolButton(resultWindget);
  tablesBtn->setDisabled(true);
  tablesBtn->setAutoRaise(true);
  tablesBtn->setText("Tabels");
  tablesLayout->addWidget(tablesBtn, 0, 0);
  connect(tablesBtn, &QToolButton::clicked, this,
          &MainWindow::createTableResultsTab);

  hlayout->addLayout(tablesLayout);

  // Добавляем вкладки
  toolTabs->addTab(mainTab, "Основные");
  toolTabs->addTab(resultWindget, "Результаты");

  // Устанавливаем панель с вкладками в качестве тулбара
  addToolBar(Qt::TopToolBarArea, createToolBarFromWidget(toolTabs));
}

// Вспомогательная функция для создания тулбара из виджета
QToolBar *MainWindow::createToolBarFromWidget(QWidget *widget) {
  QToolBar *toolBar = new QToolBar("Панель вкладок");
  toolBar->setMovable(false); // Можно сделать фиксированной
  toolBar->setAllowedAreas(Qt::TopToolBarArea);
  toolBar->addWidget(widget);
  return toolBar;
}

void MainWindow::createToolStrip() {
  QStatusBar *statusBar = this->statusBar();

  // Статусные метки
  statusLabel = new QLabel();
  statusBar->addWidget(statusLabel);

  // Прогресс-бар в статус-баре
  QProgressBar *progressBar = new QProgressBar();
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->setFixedWidth(150);
  progressBar->setVisible(false); // По умолчанию скрыт
  statusBar->addPermanentWidget(progressBar);
}

void MainWindow::setupTreeContextMenu() {
  // Создаем структурированное меню
  treeContextMenu = new TreeContextMenu(treeWidget, this);

  // Подключаем сигналы к слотам
  connect(treeContextMenu, &TreeContextMenu::createDefaultPlateScheme, this,
          &MainWindow::createDefaultPlateScheme);
}

void MainWindow::onTreeContextMenuRequested(const QPoint &pos) {
  if (treeContextMenu) {
    treeContextMenu->showMenu(pos);
  }
}

// Функция, отрабатываемая при нажатии кнопки "Create default scheme" У treewiev
// для plate
void MainWindow::createDefaultPlateScheme(QTreeWidgetItem *item) {
  treeContextMenu->createDiologDefualtSchemePlate(&elements, this, scene, mesh);
}

void MainWindow::calculateButtonClicked() {
  if (!this->mesh) {
    return;
  }

  QProgressDialog *progressBar = new QProgressDialog(this);
  progressBar->setWindowTitle("Calculating...");
  progressBar->setLabelText("Initializing calculation...");
  progressBar->setModal(true);
  progressBar->setRange(0, 0);
  progressBar->setMinimumDuration(0);
  progressBar->show();

  Solver *solver = new Solver();
  this->solver = solver;

  // Создаем отдельный поток
  QThread *workerThread = new QThread();

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
            this->scene->setResulthData(solver->maxAbsValues, solver->maxValues,
                                        solver->minValues);

            const auto *elems = solver->elements;

            for (const auto &element : *elems) {
              auto data = ElementProvider.at(element->getType());
              for (size_t i = 0; i < data.STR_OUTPUT_VALUES.size(); i++) {
                for (size_t j = 0; j < this->resultButtons.size(); j++) {
                  if (data.STR_OUTPUT_VALUES[i] ==
                      this->resultButtons[j]->text()) {
                    this->resultButtons[j]->setEnabled(true);
                    break;
                  }
                }
              }
            }

            // Завершаем поток
            workerThread->quit();
          });
  connect(solver, &Solver::calcFinishedStep, tablesBtn,
          [this]() { tablesBtn->setEnabled(true); });

  // Убираем объекты при завершении
  connect(workerThread, &QThread::finished, workerThread,
          &QThread::deleteLater);

  // Запускаем поток
  workerThread->start();
  progressBar->show();
}

// Creating the result tab
void MainWindow::createTableResultsTab() {
  QWidget *tableWindow = new QWidget();
  QVBoxLayout *layout =
      new QVBoxLayout(tableWindow); // Создаем layout для виджета

  QHBoxLayout *hLayout =
      new QHBoxLayout(tableWindow); // Создаем layout для виджета

  // model = new QTableWidgetItemModel();

  QComboBox *comboBoxForResultType = new QComboBox(tableWindow);

  QComboBox *comboBoxForSelectedElement = new QComboBox(tableWindow);
  for (const auto &element : elements) {
    auto type = element->getType();
    auto DATA = ElementProvider.at(type);
    comboBoxForSelectedElement->addItem(element->name_);
  }

  selectedELement = elements.first();
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

  for (const auto &resultName : DATA.STR_OUTPUT_VALUES) {
    comboBoxForResultType->addItem(resultName);
  }
  comboBoxForResultType->addItem("All results");
  comboBoxForResultType->setCurrentIndex(DATA.OUTPUT_VALUES_COUNT);

  comboBoxForResultType->setFixedWidth(100);
  comboBoxForSelectedElement->setFixedWidth(100);
  layout->addLayout(hLayout);
  hLayout->addWidget(comboBoxForResultType);
  hLayout->addWidget(comboBoxForSelectedElement);

  resultsView = new QTableWidget();
  resultsView->setSelectionBehavior(QAbstractItemView::SelectRows);
  resultsView->setSelectionBehavior(QAbstractItemView::SelectItems);
  resultsView->setSelectionMode(QAbstractItemView::MultiSelection);
  resultsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+C"), resultsView);
  connect(shortcut, &QShortcut::activated, this,
          &MainWindow::copySelectionToClipboard);

  getResultTable(selectedELement, DATA.OUTPUT_VALUES_COUNT);

  setSpanResultTable(selectedELement, resultsView);

  layout->addWidget(resultsView); // Добавляем таблицу в layout
  tableWindow->setLayout(layout); // Устанавливаем layout для виджета

  // Устанавливаем заголовок для подокна (вкладки)
  tableWindow->setWindowTitle(
      "Tables " + QString::number(m_pma->subWindowList().count() + 1));

  // Добавляем подокно в MDI-область
  m_pma->addSubWindow(tableWindow);

  connect(comboBoxForResultType, &QComboBox::currentIndexChanged, this,
          &MainWindow::updateResultTable);

  tableWindow->show();
}

// Initialize s default result table with all results type
void MainWindow::getResultTable(shared_ptr<AbstractElement> selectedELement,
                                int selectedId) {
  auto mesh = selectedELement->meshData_;
  auto DATA = ElementProvider.at(selectedELement->getType());
  auto nodes = mesh->nodes;
  resultsView->setColumnCount(5 + DATA.OUTPUT_VALUES_COUNT);
  resultsView->setRowCount(nodes.size());
  short outputValuesCount = DATA.OUTPUT_VALUES_COUNT;

  resultsView->setHorizontalHeaderItem(0, new QTableWidgetItem("Elements"));
  resultsView->setHorizontalHeaderItem(1, new QTableWidgetItem("Nodes"));
  resultsView->setHorizontalHeaderItem(2, new QTableWidgetItem("x"));
  resultsView->setHorizontalHeaderItem(3, new QTableWidgetItem("y"));
  resultsView->setHorizontalHeaderItem(4, new QTableWidgetItem("z"));

  if (selectedId == outputValuesCount) {
    for (size_t i = 0; i < DATA.STR_OUTPUT_VALUES.size(); i++) {
      resultsView->setHorizontalHeaderItem(
          i + 5, new QTableWidgetItem(DATA.STR_OUTPUT_VALUES[i]));
    }

    for (size_t i = 0; i < nodes.size(); i++) {
      int elementId = i / DATA.NODES_COUNT;
      resultsView->setItem(
          i, 0, new QTableWidgetItem("Element " + QString::number(elementId)));
      resultsView->setItem(
          i, 1, new QTableWidgetItem("Node " + QString::number(nodes[i]->id)));
      resultsView->setItem(
          i, 2, new QTableWidgetItem(QString::number(nodes[i]->point.x)));
      resultsView->setItem(
          i, 3, new QTableWidgetItem(QString::number(nodes[i]->point.y)));
      resultsView->setItem(
          i, 4, new QTableWidgetItem(QString::number(nodes[i]->point.z)));

      for (size_t j = 0; j < DATA.OUTPUT_VALUES_COUNT; j++) {
        resultsView->setItem(
            i, j + 5,
            new QTableWidgetItem(QString::number(nodes[i]->outputValues[j])));
      }
    }

    return;
  }

  resultsView->setHorizontalHeaderItem(
      4, new QTableWidgetItem(DATA.STR_OUTPUT_VALUES[selectedId]));

  for (size_t i = 0; i < nodes.size(); i++) {
    int elementId = i / DATA.NODES_COUNT;
    resultsView->setItem(
        i, 0, new QTableWidgetItem("Element " + QString::number(elementId)));
    resultsView->setItem(
        i, 1, new QTableWidgetItem(QString::number(nodes[i]->point.x)));
    resultsView->setItem(
        i, 2, new QTableWidgetItem(QString::number(nodes[i]->point.y)));
    resultsView->setItem(
        i, 3, new QTableWidgetItem(QString::number(nodes[i]->point.z)));
    resultsView->setItem(i, 4,
                         new QTableWidgetItem(QString::number(
                             nodes[i]->outputValues[selectedId])));
  }
}

// Span element rows
void MainWindow::setSpanResultTable(shared_ptr<AbstractElement> selectedELement,
                                    QTableView *resultsView) {
  auto mesh = selectedELement->meshData_;
  auto DATA = ElementProvider.at(selectedELement->getType());
  auto nodes = mesh->nodes;

  for (size_t i = 0; i < nodes.size(); i++) {
    // int elementId = i / DATA.NODES_COUNT;
    resultsView->setSpan(i * DATA.NODES_COUNT, 0, DATA.NODES_COUNT, 1);
  }
}

// Slot that activates when comboBox selection changes after initializing the
// default result table
void MainWindow::updateResultTable() {
  QComboBox *comboBox = (QComboBox *)sender();
  int selectedIndex = comboBox->currentIndex();
  auto mesh = selectedELement->meshData_;
  auto DATA = ElementProvider.at(selectedELement->getType());
  short outputValuesCount = DATA.OUTPUT_VALUES_COUNT;
  const auto &nodes = mesh->nodes;

  // If selected result type == All results (that have last index)
  if (selectedIndex == outputValuesCount) {
    for (size_t i = 0; i < outputValuesCount; i++) {
      resultsView->setColumnHidden(i + 5, false);
      QString strOutput = DATA.STR_OUTPUT_VALUES[i];
      resultsView->setHorizontalHeaderItem(
          i + 5, new QTableWidgetItem(DATA.STR_OUTPUT_VALUES[i]));
    }

    for (size_t i = 0; i < nodes.size(); i++) {
      for (size_t j = 0; j < DATA.OUTPUT_VALUES_COUNT; j++) {
        resultsView->setItem(
            i, j + 5,
            new QTableWidgetItem(QString::number(nodes[i]->outputValues[j])));
      }
    }
  }
  // If selected result type is not all results
  else {
    QString strOutput = DATA.STR_OUTPUT_VALUES[selectedIndex];
    resultsView->setHorizontalHeaderItem(5, new QTableWidgetItem(strOutput));

    for (size_t i = 0; i < nodes.size(); i++) {
      for (size_t j = 0; j < DATA.OUTPUT_VALUES_COUNT; j++) {
        resultsView->setItem(i, 5,
                             new QTableWidgetItem(QString::number(
                                 nodes[i]->outputValues[selectedIndex])));
      }
    }

    for (size_t i = 1; i < outputValuesCount; i++) {
      resultsView->setColumnHidden(i + 5, true);
    }
  }
}

void MainWindow::copySelectionToClipboard() {
  // Получаем список всех выделенных элементов
  QList<QTableWidgetItem *> selectedItems = resultsView->selectedItems();

  if (selectedItems.isEmpty())
    return; // Ничего не выделено

  // Определяем границы выделенного диапазона
  int minRow = INT_MAX, maxRow = INT_MIN;
  int minCol = INT_MAX, maxCol = INT_MIN;

  foreach (QTableWidgetItem *item, selectedItems) {
    minRow = qMin(minRow, item->row());
    maxRow = qMax(maxRow, item->row());
    minCol = qMin(minCol, item->column());
    maxCol = qMax(maxCol, item->column());
  }

  // Создаем карту для группировки элементов по строкам
  QMap<int, QMap<int, QString>> cellTexts;
  foreach (QTableWidgetItem *item, selectedItems) {
    cellTexts[item->row()][item->column()] = item->text();
  }

  // Формируем строку для буфера обмена
  QString clipboardText;
  for (int row = minRow; row <= maxRow; ++row) {
    if (row != minRow)
      clipboardText += "\n"; // Новая строка

    for (int col = minCol; col <= maxCol; ++col) {
      if (col != minCol)
        clipboardText += "\t"; // Табуляция между столбцами

      // Добавляем текст ячейки или пустую строку
      if (cellTexts.contains(row) && cellTexts[row].contains(col)) {
        clipboardText += cellTexts[row][col];
      } else {
        clipboardText += ""; // Пустая ячейка в выделенном диапазоне
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