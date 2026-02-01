#include <QContextMenuEvent>
#include <QDialog>
#include <QMessageBox>
#include <QThread>
#include <memory>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qformlayout.h>
#include <qglobal.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qvariant.h>
#include <qwidget.h>

#include "PlateMaterial.h"
#include "femtypes.h"
#include "generalElement/plates/plate.h"
#include "graphics/qtgl/qtgl.h"
#include "load.h"
#include "material.h"
#include "mesh/mesh.h"
#include "point.h"
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

  QDialog *diolog = new QDialog(mainWindow);
  diolog->setFixedSize({500, 260});
  diolog->setWindowTitle("Settings of scheme");
  diolog->setModal(true);
  diolog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
                         Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout(diolog);
  QFormLayout *formLayout = new QFormLayout();
  QComboBox *comboBox = new QComboBox(diolog);
  setElementTypeComboBox(comboBox);
  formLayout->addRow("Element type:", comboBox);

  // Выравнивание
  formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
  formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
  formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

  diolog->setSizeGripEnabled(true); // Добавляет маркер изменения размера в углу

  // 2. Создаем панель с кнопками
  QDialogButtonBox *buttonBox = new QDialogButtonBox(diolog);
  // Добавляем стандартные кнопки
  buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                QDialogButtonBox::Cancel);
  // Подключаем сигналы: нажатие OK/Отмена закроет диалог с соответствующим
  // результатом
  connect(buttonBox, &QDialogButtonBox::accepted, diolog, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, diolog, &QDialog::reject);

  // 3. Задаем основную компоновку
  mainLayout->addLayout(formLayout);
  mainLayout->addWidget(buttonBox); // Панель кнопок — внизу
  // d->setLayout(mainLayout);
  diolog->show();

  ElementType selectedType{ElementType::MITC4MY};
  connect(comboBox, &QComboBox::currentIndexChanged, this,
          [&selectedType, comboBox]() {
            selectedType = comboBox->currentData().value<ElementType>();
          });

  // When dialog closed
  if (diolog->exec() == QDialog::Accepted) {

    // Creating a general element (Plate)
    Point3 startPoint{0, 0, 0};
    // Find the last point of existed elements
    if (elements->size() > 0) {
      auto element = elements->at(elements->size() - 1);
      startPoint = element->getStartPoint();
      startPoint.x += 1000 + element->getLenght();
    }

    double loadv[] = {-100, 0, 0};
    shared_ptr<Material> material = PlateMaterial::getDefaultMaterial();
    shared_ptr<AbstractLoad> load = make_shared<AreaLoadFzMxMy>(loadv, 3);

    shared_ptr<AbstractElement> plate =
        make_shared<Plate>(load, selectedType, 16000, startPoint, material);

    elements->push_back(plate);

    QProgressDialog *progressDilog = new QProgressDialog(diolog);
    progressDilog->setWindowTitle("Generating default mesh...");
    progressDilog->setLabelText("Initializing calculation...");
    progressDilog->setModal(true);
    progressDilog->setRange(0, 0);
    progressDilog->setMinimumDuration(0);
    progressDilog->show();

    if (mesh) {
      mesh->disconnect();
    } else {
      mesh = new Mesh();
    }

    // Создаем отдельный поток для mesh_
    QThread *workerThread = new QThread();
    mesh->moveToThread(workerThread); // mesh_ теперь принадлежит workerThread

    connect(workerThread, &QThread::started, this,
            [mesh, elements]() { mesh->meshCreateManager(elements, true); });

    connect(mesh, &Mesh::progressChanged, this, [progressDilog](int count) {
      progressDilog->setLabelText(QString("Creating element: %1").arg(count));
    });
    connect(mesh, &Mesh::meshFinished, this, [progressDilog](int count) {
      progressDilog->setLabelText(
          QString("Sucsesfully Created %1 elements").arg(count));
      progressDilog->close();
    });

    connect(mesh, &Mesh::meshFinished, this,
            [mesh, scene, workerThread, elements](int count) {
              scene->setMeshData(elements);
              workerThread->quit();
            });

    connect(workerThread, &QThread::finished, workerThread,
            &QThread::deleteLater);

    workerThread->start();
    progressDilog->show();
  }
}

void TreeContextMenu::setElementTypeComboBox(QComboBox *comboBox) {
  QVariant var = ElementType::MITC4MY;
  comboBox->addItem("my MITC4", ElementType::MITC4MY);
  comboBox->addItem("DKMQ", ElementType::DKMQ);
  // comboBox->addItem("MITC16");
}

void TreeContextMenu::createAddELementDiolog(
    QDialog *MainDiolog, QVector<shared_ptr<AbstractElement>> *elements) {

  QDialog *diolog = new QDialog(MainDiolog);
  diolog->setFixedSize({500, 260});
  diolog->setWindowTitle("Add one element to shceme");
  diolog->setModal(true);
  diolog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
                         Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout(diolog);
  QFormLayout *formLayout = new QFormLayout();
  QComboBox *comboBox = new QComboBox(diolog);
  setElementTypeComboBox(comboBox);
  formLayout->addRow("Element type:", comboBox);

  // Выравнивание
  formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
  formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
  formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(diolog);
  // Добавляем стандартные кнопки
  buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                QDialogButtonBox::Cancel);

  ElementType type{ElementType::MITC4MY};
  connect(comboBox, &QComboBox::currentIndexChanged, diolog,
          [&type](int id) { type = (ElementType)id; });
  mainLayout->addLayout(formLayout);
  mainLayout->addWidget(buttonBox); // Панель кнопок — внизу

  connect(buttonBox, &QDialogButtonBox::accepted, diolog, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, diolog, &QDialog::reject);

  diolog->show();

  double loadv[] = {-100, 0, 0};
  Point3 startPoint = {0, 0, 0};
  if (elements->size() > 0) {
    auto element = elements->at(elements->size() - 1);
    startPoint = element->getStartPoint();
    startPoint.x += 1000 + element->getLenght();
  }
  shared_ptr<AbstractLoad> load = make_shared<AreaLoadFzMxMy>(loadv, 3);
  shared_ptr<AbstractElement> plate =
      std::make_shared<Plate>(load, type, 3000, startPoint);

  elements->push_back(plate);
}
