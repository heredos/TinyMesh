/****************************************************************************
** Meta object code from reading C++ file 'realtime.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "AppTinyMesh/Include/realtime.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'realtime.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
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
struct qt_meta_tag_ZN10MeshWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto MeshWidget::qt_create_metaobjectdata<qt_meta_tag_ZN10MeshWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MeshWidget",
        "_signalUpdate",
        "",
        "_signalCameraChanged",
        "_signalMouseMoveEdit",
        "QMouseEvent*",
        "_signalMouseMove",
        "_signalMouseRelease",
        "_signalEditSceneLeft",
        "Ray",
        "_signalEditSceneRight",
        "mousePressEvent",
        "mouseReleaseEvent",
        "mouseDoubleClickEvent",
        "mouseMoveEvent",
        "wheelEvent",
        "QWheelEvent*",
        "keyPressEvent",
        "QKeyEvent*",
        "keyReleaseEvent"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal '_signalUpdate'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal '_signalCameraChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal '_signalMouseMoveEdit'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Signal '_signalMouseMove'
        QtMocHelpers::SignalData<void(QMouseEvent *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Signal '_signalMouseRelease'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal '_signalEditSceneLeft'
        QtMocHelpers::SignalData<void(const Ray &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 2 },
        }}),
        // Signal '_signalEditSceneRight'
        QtMocHelpers::SignalData<void(const Ray &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 2 },
        }}),
        // Slot 'mousePressEvent'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Slot 'mouseReleaseEvent'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Slot 'mouseDoubleClickEvent'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Slot 'mouseMoveEvent'
        QtMocHelpers::SlotData<void(QMouseEvent *)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Slot 'wheelEvent'
        QtMocHelpers::SlotData<void(QWheelEvent *)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 16, 2 },
        }}),
        // Slot 'keyPressEvent'
        QtMocHelpers::SlotData<void(QKeyEvent *)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 2 },
        }}),
        // Slot 'keyReleaseEvent'
        QtMocHelpers::SlotData<void(QKeyEvent *)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MeshWidget, qt_meta_tag_ZN10MeshWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MeshWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MeshWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MeshWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MeshWidgetE_t>.metaTypes,
    nullptr
} };

void MeshWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MeshWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->_signalUpdate(); break;
        case 1: _t->_signalCameraChanged(); break;
        case 2: _t->_signalMouseMoveEdit((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 3: _t->_signalMouseMove((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 4: _t->_signalMouseRelease(); break;
        case 5: _t->_signalEditSceneLeft((*reinterpret_cast<std::add_pointer_t<Ray>>(_a[1]))); break;
        case 6: _t->_signalEditSceneRight((*reinterpret_cast<std::add_pointer_t<Ray>>(_a[1]))); break;
        case 7: _t->mousePressEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 8: _t->mouseReleaseEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 9: _t->mouseDoubleClickEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 10: _t->mouseMoveEvent((*reinterpret_cast<std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 11: _t->wheelEvent((*reinterpret_cast<std::add_pointer_t<QWheelEvent*>>(_a[1]))); break;
        case 12: _t->keyPressEvent((*reinterpret_cast<std::add_pointer_t<QKeyEvent*>>(_a[1]))); break;
        case 13: _t->keyReleaseEvent((*reinterpret_cast<std::add_pointer_t<QKeyEvent*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MeshWidget::*)()>(_a, &MeshWidget::_signalUpdate, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshWidget::*)()>(_a, &MeshWidget::_signalCameraChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshWidget::*)(QMouseEvent * )>(_a, &MeshWidget::_signalMouseMoveEdit, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshWidget::*)(QMouseEvent * )>(_a, &MeshWidget::_signalMouseMove, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshWidget::*)()>(_a, &MeshWidget::_signalMouseRelease, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshWidget::*)(const Ray & )>(_a, &MeshWidget::_signalEditSceneLeft, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshWidget::*)(const Ray & )>(_a, &MeshWidget::_signalEditSceneRight, 6))
            return;
    }
}

const QMetaObject *MeshWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MeshWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MeshWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int MeshWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void MeshWidget::_signalUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MeshWidget::_signalCameraChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MeshWidget::_signalMouseMoveEdit(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void MeshWidget::_signalMouseMove(QMouseEvent * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void MeshWidget::_signalMouseRelease()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MeshWidget::_signalEditSceneLeft(const Ray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void MeshWidget::_signalEditSceneRight(const Ray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
