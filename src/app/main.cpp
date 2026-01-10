#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setStyle(QStyleFactory::create("Universal"));
  MainWindow mainWindow;

  mainWindow.show();
  return app.exec();
}