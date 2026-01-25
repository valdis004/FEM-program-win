#include <QContextMenuEvent>
#include <QDialog>
#include <QMessageBox>
#include <QThread>
#include <memory>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialogbuttonbox.h>
#include <qformlayout.h>
#include <qglobal.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qtimer.h>
#include <qwidget.h>

#include "femtypes.h"
#include "generalElement/plates/plate.h"
#include "graphics/qtgl/qtgl.h"
#include "load.h"
#include "mesh/mesh.h"
#include "treeContextMenu.h"

#define TEXT_PLATE_STANDART_SCHEME "Create default scheme"

using std::make_shared;
using std::shared_ptr;

TreeContextMenu::TreeContextMenu(QTreeWidget *treeWidget, QObject *parent)
    : QObject(parent), treeWidget(treeWidget), currentItem(nullptr) {

  // Создаем меню
  contextMenu = new QMenu();

  // Настраиваем стандартное меню
  setupMenu();
  connectActions();
}

TreeContextMenu::~TreeContextMenu() { delete contextMenu; }

void TreeContextMenu::setupMenu() {
  // Стандартные действия
  addAction(TEXT_PLATE_STANDART_SCHEME);
}

void TreeContextMenu::showMenu(const QPoint &pos) {
  // Определяем элемент под курсором
  currentItem = treeWidget->itemAt(pos);

  // в зависимости от выбранного элемента
  if (actions.contains("delete")) {
    bool canDelete = currentItem && currentItem->parent();
    actions["delete"]->setEnabled(canDelete);
  }

  if (actions.contains("rename")) {
    actions["rename"]->setEnabled(currentItem != nullptr);
  }

  // Показываем меню
  if (contextMenu) {
    contextMenu->exec(treeWidget->viewport()->mapToGlobal(pos));
  }
}

void TreeContextMenu::connectActions() {
  // Подключаем все действия к одному слоту
  for (QAction *action : contextMenu->actions()) {
    connect(action, &QAction::triggered, this,
            &TreeContextMenu::onActionTriggered);
  }
}

void TreeContextMenu::addAction(const QString &text, const QIcon &icon) {
  QAction *action = contextMenu->addAction(icon, text);
  action->setData(text); // Сохраняем имя действия
  actions[text] = action;
}

void TreeContextMenu::onActionTriggered() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action)
    return;

  QString actionName = action->data().toString();

  // Испускаем соответствующие сигналы
  if (actionName == TEXT_PLATE_STANDART_SCHEME) {
    emit createDefaultPlateScheme(currentItem);
  }

  // Общий сигнал
  emit actionTriggered(actionName, currentItem);
}

void TreeContextMenu::createDiologDefualtSchemePlate(
    QVector<shared_ptr<AbstractElement>> *elements, QWidget *mainWindow,
    Qtgl *scene, Mesh *&mesh) {
  QDialog *d = new QDialog(mainWindow);
  d->setFixedSize({500, 260});
  d->setWindowTitle("Settings of scheme");
  // d->setModal(true);
  // d->setWindowFlag(Qt);
  // d->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
  // Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout(d);
  QFormLayout *formLayout = new QFormLayout();
  QComboBox *comboBox = new QComboBox(d);
  comboBox->addItem("MITC4");
  comboBox->addItem("MITC9");
  comboBox->addItem("MITC16");
  formLayout->addRow("Тип элемента:", comboBox);

  // Выравнивание
  formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
  formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
  formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

  d->setSizeGripEnabled(true); // Добавляет маркер изменения размера в углу

  // 2. Создаем панель с кнопками
  QDialogButtonBox *buttonBox = new QDialogButtonBox(d);
  // Добавляем стандартные кнопки
  buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                QDialogButtonBox::Cancel);
  // Подключаем сигналы: нажатие OK/Отмена закроет диалог с соответствующим
  // результатом
  connect(buttonBox, &QDialogButtonBox::accepted, d, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, d, &QDialog::reject);

  // 3. Задаем основную компоновку
  mainLayout->addLayout(formLayout);
  mainLayout->addWidget(buttonBox); // Панель кнопок — внизу
  // d->setLayout(mainLayout);
  d->show();

  ElementType type = (ElementType)comboBox->currentIndex();
  double loadv[] = {-100, 0, 0};
  shared_ptr<AbstractLoad> load = make_shared<AreaLoadFzMxMy>(loadv, 3);

  shared_ptr<AbstractElement> plate = std::make_shared<Plate>(load, type, 2000);

  elements->push_back(plate);

  if (d->exec() == QDialog::Accepted) {
    QProgressDialog *progressDilog = new QProgressDialog(mainWindow);
    progressDilog->setWindowTitle("Generating default mesh...");
    progressDilog->setLabelText("Initializing calculation...");
    progressDilog->setModal(true);
    progressDilog->setRange(0, 0);
    progressDilog->setMinimumDuration(0);
    progressDilog->show();

    mesh = new Mesh();

    connect(mesh, &Mesh::progressChanged, this, [progressDilog](int count) {
      progressDilog->setLabelText(QString("Creating element: %1").arg(count));
    });
    connect(mesh, &Mesh::meshFinished, this, [progressDilog](int count) {
      progressDilog->setLabelText(
          QString("Sucsesfully Created %1 elements").arg(count));
      progressDilog->close();
    });

    // Создаем отдельный поток для mesh_
    QThread *workerThread = new QThread();
    mesh->moveToThread(workerThread); // mesh_ теперь принадлежит workerThread

    // Когда поток запустится, выполним создание меша
    connect(workerThread, &QThread::started, this,
            [mesh, elements]() { mesh->meshCreateManager(elements, true); });

    connect(mesh, &Mesh::meshFinished, this,
            [mesh, scene, workerThread, elements](int count) {
              scene->setMeshData(elements);

              // Завершаем поток
              workerThread->quit();
            });

    // Удаляем поток и mesh_ при завершении
    connect(workerThread, &QThread::finished, workerThread,
            &QThread::deleteLater);
    // connect(workerThread, &QThread::finished, mesh_, &Mesh::deleteLater);

    workerThread->start();
    // delete mesh;
  }
}
