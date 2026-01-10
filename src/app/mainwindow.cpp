#include "mainwindow.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QDockWidget>
#include <QLabel>
#include <QMdiArea>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <qdockwidget.h>
#include <qnamespace.h>

MainWindow::MainWindow() {
  setWindowTitle("Пример Docking областей");
  resize(1000, 600);
  createMenus();
  createToolBar();
  createLeftDock();
  createToolStrip();
}

void MainWindow::createLeftDock() {
  // Центральный виджет - текстовый редактор
  QMdiArea *m_pma = new QMdiArea(this);
  m_pma->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_pma->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QTextEdit *centralTextEdit = new QTextEdit("Text");
  m_pma->addSubWindow(centralTextEdit);
  centralTextEdit->setWindowTitle("Graphic window");
  m_pma->setViewMode(QMdiArea::TabbedView);
  m_pma->setViewMode(QMdiArea::TabbedView);
  m_pma->setTabPosition(QTabWidget::North);
  m_pma->setTabsClosable(true); // Добавить кнопки закрытия
  m_pma->setTabsMovable(true);  // Возможность перемещать вкладки

  setCentralWidget(m_pma);

  // ========== 4. DOCK WIDGETS (Док-система) ==========

  // Док-виджет слева (Структура документа)
  QDockWidget *leftDock = new QDockWidget("Структура", this);
  leftDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  QTreeWidget *treeWidget = new QTreeWidget();
  treeWidget->setHeaderLabel("Заголовки");
  QTreeWidgetItem *root =
      new QTreeWidgetItem(treeWidget, QStringList("Документ"));
  new QTreeWidgetItem(root, QStringList("Введение"));
  new QTreeWidgetItem(root, QStringList("Глава 1"));
  new QTreeWidgetItem(root, QStringList("Глава 2"));
  new QTreeWidgetItem(root, QStringList("Заключение"));
  treeWidget->expandAll();

  leftDock->setWidget(treeWidget);
  leftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, leftDock);

  // // Док-виджет справа (Свойства/Информация)
  // QDockWidget *rightDock = new QDockWidget("Свойства", this);
  // rightDock->setAllowedAreas(Qt::LeftDockWidgetArea |
  // Qt::RightDockWidgetArea);

  // QTabWidget *rightTabWidget = new QTabWidget();

  // QWidget *propertiesTab = new QWidget();
  // QVBoxLayout *propsLayout = new QVBoxLayout(propertiesTab);

  // QLabel *label1 = new QLabel("Размер файла: 1.2 MB");
  // QLabel *label2 = new QLabel("Количество слов: 156");
  // QLabel *label3 = new QLabel("Дата изменения: сегодня");

  // propsLayout->addWidget(label1);
  // propsLayout->addWidget(label2);
  // propsLayout->addWidget(label3);
  // propsLayout->addStretch();

  // QWidget *notesTab = new QWidget();
  // QVBoxLayout *notesLayout = new QVBoxLayout(notesTab);
  // QTextEdit *notesEdit = new QTextEdit();
  // notesEdit->setPlaceholderText("Ваши заметки...");
  // notesLayout->addWidget(notesEdit);

  // rightTabWidget->addTab(propertiesTab, "Свойства");
  // rightTabWidget->addTab(notesTab, "Заметки");

  // rightDock->setWidget(rightTabWidget);
  // addDockWidget(Qt::RightDockWidgetArea, rightDock);

  // // Док-виджет снизу (Консоль/Логи)
  // QDockWidget *bottomDock = new QDockWidget("Консоль", this);
  // bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea |
  // Qt::TopDockWidgetArea);

  // QTextBrowser *consoleWidget = new QTextBrowser();
  // consoleWidget->setPlainText("[INFO] Приложение запущено\n"
  //                             "[INFO] Загружен документ\n"
  //                             "[INFO] Готов к работе");

  // bottomDock->setWidget(consoleWidget);
  // addDockWidget(Qt::BottomDockWidgetArea, bottomDock);
}

void MainWindow::createMenus() {
  QMenuBar *menuBar = this->menuBar();

  QMenu *fileMenu = menuBar->addMenu("Настройки");
  QMenu *viewMenu = menuBar->addMenu("&Вид");
}

void MainWindow::createToolBar() {
  QToolBar *mainToolBar = new QToolBar("Основная панель");
  addToolBar(Qt::TopToolBarArea, mainToolBar);

  mainToolBar->addAction(QIcon::fromTheme("document-new"), "Новый");
  mainToolBar->addAction(QIcon::fromTheme("document-open"), "Открыть");
  mainToolBar->addAction(QIcon::fromTheme("document-save"), "Сохранить");
  mainToolBar->addSeparator();
  mainToolBar->addAction(QIcon::fromTheme("edit-cut"), "Вырезать");
  mainToolBar->addAction(QIcon::fromTheme("edit-copy"), "Копировать");
  mainToolBar->addAction(QIcon::fromTheme("edit-paste"), "Вставить");
  mainToolBar->addSeparator();
  mainToolBar->addAction(QIcon::fromTheme("edit-undo"), "Отменить");
  mainToolBar->addAction(QIcon::fromTheme("edit-redo"), "Повторить");

  // Панель форматирования
  QToolBar *formatToolBar = new QToolBar("Форматирование");
  addToolBar(Qt::TopToolBarArea, formatToolBar);

  QPushButton *boldBtn = new QPushButton("B");
  boldBtn->setCheckable(true);
  boldBtn->setFixedSize(30, 30);

  QPushButton *italicBtn = new QPushButton("I");
  italicBtn->setCheckable(true);
  italicBtn->setFixedSize(30, 30);

  QPushButton *underlineBtn = new QPushButton("U");
  underlineBtn->setCheckable(true);
  underlineBtn->setFixedSize(30, 30);

  formatToolBar->addWidget(boldBtn);
  formatToolBar->addWidget(italicBtn);
  formatToolBar->addWidget(underlineBtn);
  formatToolBar->addSeparator();

  // Выпадающий список шрифтов
  QComboBox *fontCombo = new QComboBox();
  fontCombo->addItems({"Arial", "Times New Roman", "Courier New", "Verdana"});
  fontCombo->setFixedWidth(120);
  formatToolBar->addWidget(fontCombo);

  // Выпадающий список размера
  QComboBox *sizeCombo = new QComboBox();
  sizeCombo->addItems({"8", "10", "12", "14", "16", "18", "24", "36"});
  sizeCombo->setFixedWidth(60);
  formatToolBar->addWidget(sizeCombo);
}

void MainWindow::createToolStrip() {
  QStatusBar *statusBar = this->statusBar();

  // Статусные метки
  QLabel *statusLabel = new QLabel("Готов");
  statusBar->addWidget(statusLabel);

  QLabel *posLabel = new QLabel("Строка: 1, Колонка: 1");
  statusBar->addPermanentWidget(posLabel);

  QLabel *zoomLabel = new QLabel("100%");
  statusBar->addPermanentWidget(zoomLabel);

  // Прогресс-бар в статус-баре
  QProgressBar *progressBar = new QProgressBar();
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->setFixedWidth(150);
  progressBar->setVisible(false); // По умолчанию скрыт
  statusBar->addPermanentWidget(progressBar);
}