#include <QApplication>
#include <QDockWidget>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QStyleFactory>
#include <QTextEdit>
#include <QTreeWidget>
#include <qstylefactory.h>
#include <qtreewidget.h>
#include <qwidget.h>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setStyle(QStyleFactory::create("Basic"));

  // Создаем главное окно
  QMainWindow mainWindow;
  mainWindow.setWindowTitle("Пример Docking областей");
  mainWindow.resize(1000, 600);

  // Создаем центральный виджет
  QTextEdit *centralTextEdit = new QTextEdit(&mainWindow);
  centralTextEdit->setPlaceholderText("Центральная область");
  mainWindow.setCentralWidget(centralTextEdit);

  // 1. Создаем первую docking область слева
  QDockWidget *leftDock = new QDockWidget("Левая панель", &mainWindow);

  QTreeWidget *tree = new QTreeWidget(leftDock);
  tree->setWindowTitle("Tree wiev");
  QTreeWidgetItem *item1 = new QTreeWidgetItem(tree, {"Ветка 1"});
  QTreeWidgetItem *item2 = new QTreeWidgetItem(tree, {"Ветка 2"});
  new QTreeWidgetItem(item1, {"Подэлемент 1"});
  new QTreeWidgetItem(item1, {"Подэлемент 2"});
  leftDock->setWidget(tree);

  // Добавляем в главное окно слева
  mainWindow.addDockWidget(Qt::LeftDockWidgetArea, leftDock);

  mainWindow.show();
  return app.exec();
}
