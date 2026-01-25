/****************************************************************************
** Meta object code from reading C++ file 'solver.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../src/solver/solver.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'solver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN6SolverE_t {};
} // unnamed namespace

template <> constexpr inline auto Solver::qt_create_metaobjectdata<qt_meta_tag_ZN6SolverE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Solver",
        "newElementStiffMatrixStep",
        "",
        "count",
        "applyBaundaryConditionsStep",
        "solveSystemStep",
        "getOutputStep",
        "calcFinishedStep"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'newElementStiffMatrixStep'
        QtMocHelpers::SignalData<void(unsigned )>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 3 },
        }}),
        // Signal 'applyBaundaryConditionsStep'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'solveSystemStep'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'getOutputStep'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'calcFinishedStep'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Solver, qt_meta_tag_ZN6SolverE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Solver::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6SolverE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6SolverE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6SolverE_t>.metaTypes,
    nullptr
} };

void Solver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Solver *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->newElementStiffMatrixStep((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1]))); break;
        case 1: _t->applyBaundaryConditionsStep(); break;
        case 2: _t->solveSystemStep(); break;
        case 3: _t->getOutputStep(); break;
        case 4: _t->calcFinishedStep(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Solver::*)(unsigned  )>(_a, &Solver::newElementStiffMatrixStep, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Solver::*)()>(_a, &Solver::applyBaundaryConditionsStep, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Solver::*)()>(_a, &Solver::solveSystemStep, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Solver::*)()>(_a, &Solver::getOutputStep, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Solver::*)()>(_a, &Solver::calcFinishedStep, 4))
            return;
    }
}

const QMetaObject *Solver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Solver::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6SolverE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Solver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Solver::newElementStiffMatrixStep(unsigned  _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Solver::applyBaundaryConditionsStep()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Solver::solveSystemStep()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Solver::getOutputStep()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Solver::calcFinishedStep()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
