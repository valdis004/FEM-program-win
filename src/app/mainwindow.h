#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class MainWindow : public QWidget {
public:
  explicit MainWindow(QWidget *parent = 0);
};

#endif // WINDOW_H