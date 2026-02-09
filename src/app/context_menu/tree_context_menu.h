#pragma once

#include <qcombobox.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qwidget.h>

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "mesh/mesh.h"

class Qtgl;

class TreeContextMenu : public QObject {
  Q_OBJECT
 private:
  void setElementTypeComboBox(QComboBox* comboBox);

 public:
  explicit TreeContextMenu(QTreeWidget* treeWidget, QObject* parent = nullptr);
  virtual ~TreeContextMenu();

  // Основной метод для показа меню
  virtual void showMenu(const QPoint& pos);

  // Методы для настройки меню
  void addAction(const QString& text, const QIcon& icon = QIcon());

  // Получить текущий выбранный элемент
  QTreeWidgetItem* getCurrentItem() const { return current_item_; }

  // Получить меню (для кастомизации)
  QMenu* menu() const { return context_menu_; }

  // Создать стандартное меню
  void createDefaultMenu();

  void createDiologDefualtSchemePlate(
      QVector<shared_ptr<AbstractElement>>* elements,
      QWidget* main_window,
      Qtgl* scene,
      Mesh*& mesh);

 signals:
  // Сигналы для различных действий
  void actionTriggered(const QString& action_name, QTreeWidgetItem* item);
  void createDefaultPlateScheme(QTreeWidgetItem* item);

 protected:
  virtual void setupMenu();       // Виртуальный метод для настройки меню
  virtual void connectActions();  // Подключение действий

 protected slots:
  virtual void onActionTriggered();

 protected:
  QTreeWidget* tree_widget_;
  QTreeWidgetItem* current_item_;
  QMenu* context_menu_;
  QHash<QString, QAction*> actions_;  // Для хранения действий по имени
};
