#include "tree_context_menu.h"

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

#include <QDialog>
#include <QMessageBox>
#include <QThread>
#include <QcontextmenuEvent>
#include <memory>

#include "fem_types.h"
#include "graphics/qtgl/qtgl.h"
#include "load.h"
#include "mesh/mesh.h"
#include "plate_material.h"
#include "point.h"
#include "structural_element/structural_plates/plate.h"

#define TEXT_PLATE_STANDART_SCHEME "Create default scheme"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
// using std::unique_ptr;

TreeContextMenu::TreeContextMenu(QTreeWidget* tree_widget, QObject* parent)
    : QObject(parent), tree_widget_(tree_widget), current_item_(nullptr) {
  // Создаем меню
  context_menu_ = new QMenu();

  // Настраиваем стандартное меню
  setupMenu();
  connectActions();
}

TreeContextMenu::~TreeContextMenu() { delete context_menu_; }

void TreeContextMenu::setupMenu() {
  // Стандартные действия
  addAction(TEXT_PLATE_STANDART_SCHEME);
}

void TreeContextMenu::showMenu(const QPoint& pos) {
  // Определяем элемент под курсором
  current_item_ = tree_widget_->itemAt(pos);

  // в зависимости от выбранного элемента
  if (actions_.contains("delete")) {
    bool canDelete = current_item_ && current_item_->parent();
    actions_["delete"]->setEnabled(canDelete);
  }

  if (actions_.contains("rename")) {
    actions_["rename"]->setEnabled(current_item_ != nullptr);
  }

  // Показываем меню
  if (context_menu_) {
    context_menu_->exec(tree_widget_->viewport()->mapToGlobal(pos));
  }
}

void TreeContextMenu::connectActions() {
  // Подключаем все действия к одному слоту
  for (QAction* action : context_menu_->actions()) {
    connect(action, &QAction::triggered, this,
            &TreeContextMenu::onActionTriggered);
  }
}

void TreeContextMenu::addAction(const QString& text, const QIcon& icon) {
  QAction* action = context_menu_->addAction(icon, text);
  action->setData(text);  // Сохраняем имя действия
  actions_[text] = action;
}

void TreeContextMenu::onActionTriggered() {
  QAction* action = qobject_cast<QAction*>(sender());
  if (!action) return;

  QString actionName = action->data().toString();

  // Испускаем соответствующие сигналы
  if (actionName == TEXT_PLATE_STANDART_SCHEME) {
    emit createDefaultPlateScheme(current_item_);
  }

  // Общий сигнал
  emit actionTriggered(actionName, current_item_);
}

void TreeContextMenu::createDiologDefualtSchemePlate(
    QVector<shared_ptr<AStructuralElement>>* elements,
    QWidget* mainWindow,
    Qtgl* scene,
    Mesh*& mesh) {
  QDialog* diolog = new QDialog(mainWindow);
  diolog->setFixedSize({500, 260});
  diolog->setWindowTitle("Settings of scheme");
  diolog->setModal(true);
  diolog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
                         Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

  QVBoxLayout* main_layout = new QVBoxLayout(diolog);
  QFormLayout* form_layout = new QFormLayout();
  QComboBox* combo_box = new QComboBox(diolog);
  setElementTypeComboBox(combo_box);
  form_layout->addRow("Element type:", combo_box);

  // Выравнивание
  form_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
  form_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
  form_layout->setRowWrapPolicy(QFormLayout::DontWrapRows);

  diolog->setSizeGripEnabled(
      true);  // Добавляет маркер изменения размера в углу

  // 2. Создаем панель с кнопками
  QDialogButtonBox* button_box = new QDialogButtonBox(diolog);
  // Добавляем стандартные кнопки
  button_box->setStandardButtons(QDialogButtonBox::Ok |
                                 QDialogButtonBox::Cancel);
  // Подключаем сигналы: нажатие OK/Отмена закроет диалог с соответствующим
  // результатом
  connect(button_box, &QDialogButtonBox::accepted, diolog, &QDialog::accept);
  connect(button_box, &QDialogButtonBox::rejected, diolog, &QDialog::reject);

  // 3. Задаем основную компоновку
  main_layout->addLayout(form_layout);
  main_layout->addWidget(button_box);  // Панель кнопок — внизу
  // d->setLayout(mainLayout);
  diolog->show();

  ElementType selected_type{ElementType::MITC4MY};
  connect(combo_box, &QComboBox::currentIndexChanged, this,
          [&selected_type, combo_box]() {
            selected_type = combo_box->currentData().value<ElementType>();
          });

  // When dialog closed
  if (diolog->exec() == QDialog::Accepted) {
    // Creating a general element (Plate)
    Point3 start_point{0, 0, 0};
    // Find the last point of existed elements
    if (elements->size() > 0) {
      auto element = elements->at(elements->size() - 1);
      start_point = element->getStartPoint();
      start_point.x += 1000 + element->getLenght();
    }

    // Creating general element
    double load_values[] = {-100, 0, 0};
    shared_ptr<AStructuralLoad> load = make_shared<AreaLoadFzMxMy>(load_values);

    shared_ptr<AStructuralElement> plate =
        make_shared<Plate>(selected_type, 4000.0, start_point);
    plate->addMaterial(make_unique<PlateMaterial>(2e11, 5.0 / 6.0, 0.2, 0.02));
    plate->addLoad(load);

    elements->push_back(plate);

    QProgressDialog* progress_dialog = new QProgressDialog(diolog);
    progress_dialog->setWindowTitle("Generating default mesh...");
    progress_dialog->setLabelText("Initializing calculation...");
    progress_dialog->setModal(true);
    progress_dialog->setRange(0, 0);
    progress_dialog->setMinimumDuration(0);
    progress_dialog->show();

    if (mesh) {
      mesh->disconnect();
    } else {
      mesh = new Mesh();
    }

    // Создаем отдельный поток для mesh_
    QThread* workerThread = new QThread();
    mesh->moveToThread(workerThread);  // mesh_ теперь принадлежит workerThread

    connect(workerThread, &QThread::started, this,
            [mesh, elements]() { mesh->meshCreateManager(elements, true); });

    connect(mesh, &Mesh::progressChanged, this, [progress_dialog](int count) {
      progress_dialog->setLabelText(QString("Creating element: %1").arg(count));
    });
    connect(mesh, &Mesh::meshFinished, this, [progress_dialog](int count) {
      progress_dialog->setLabelText(
          QString("Sucsesfully Created %1 elements").arg(count));
      progress_dialog->close();
    });

    connect(mesh, &Mesh::meshFinished, this,
            [mesh, scene, workerThread, elements](int count) {
              scene->setMeshData(elements);
              workerThread->quit();
            });

    connect(workerThread, &QThread::finished, workerThread,
            &QThread::deleteLater);

    workerThread->start();
    progress_dialog->show();
  }
}

void TreeContextMenu::setElementTypeComboBox(QComboBox* combo_box) {
  QVariant var = ElementType::MITC4MY;
  combo_box->addItem("my MITC4", ElementType::MITC4MY);
  combo_box->addItem("my MITC9", ElementType::MITC9MY);
  combo_box->addItem("DKMQ", ElementType::DKMQ);
  // comboBox->addItem("MITC16");
}