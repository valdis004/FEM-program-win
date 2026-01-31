#pragma once

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QTreeWidget>

#include <QTreeWidgetItem>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qwidget.h>

#include "mesh/mesh.h"

class Qtgl;

class TreeContextMenu : public QObject {
  Q_OBJECT
private:
  void setElementTypeComboBox(QComboBox *comboBox);
  void createAddELementDiolog(QDialog *MainDiolog,
                              QVector<shared_ptr<AbstractElement>> *elements);

public:
  explicit TreeContextMenu(QTreeWidget *treeWidget, QObject *parent = nullptr);
  virtual ~TreeContextMenu();

  // Основной метод для показа меню
  virtual void showMenu(const QPoint &pos);

  // Методы для настройки меню
  void addAction(const QString &text, const QIcon &icon = QIcon());

  // Получить текущий выбранный элемент
  QTreeWidgetItem *getCurrentItem() const { return currentItem; }

  // Получить меню (для кастомизации)
  QMenu *menu() const { return contextMenu; }

  // Создать стандартное меню
  void createDefaultMenu();

  void
  createDiologDefualtSchemePlate(QVector<shared_ptr<AbstractElement>> *elements,
                                 QWidget *mainWindow, Qtgl *scene, Mesh *&mesh);

signals:
  // Сигналы для различных действий
  void actionTriggered(const QString &actionName, QTreeWidgetItem *item);
  void createDefaultPlateScheme(QTreeWidgetItem *item);

protected:
  virtual void setupMenu();      // Виртуальный метод для настройки меню
  virtual void connectActions(); // Подключение действий

protected slots:
  virtual void onActionTriggered();

protected:
  QTreeWidget *treeWidget;
  QTreeWidgetItem *currentItem;
  QMenu *contextMenu;
  QHash<QString, QAction *> actions; // Для хранения действий по имени
};
