#include <QApplication>
#include <QStyleFactory>

// #include "elements/elementprovider.h"
#include "main_window.h"

int main(int argc, char *argv[]) {
  // ElementProvider::init();

  QApplication app(argc, argv);
  app.setStyle(QStyleFactory::create("Fusion"));
  MainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}
