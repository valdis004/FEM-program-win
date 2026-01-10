#ifndef WINDOW_H
#define WINDOW_H

// #include <QAbstractButton>
// #include <QComboBox>
// #include <QFontComboBox>
#include <QMainWindow>
// #include <QToolBar>
// #include <QToolBox>
// #include <QToolButton>
// #include <QWidget>
// #include <qdockwidget.h>
// #include <qwidget.h>
class QMdiArea;

class MainWindow : public QMainWindow {

public:
  MainWindow();

private:
  void createLeftDock();
  void createMenus();
  void createToolBar();
  void createToolStrip();
};

#endif // WINDOW_H