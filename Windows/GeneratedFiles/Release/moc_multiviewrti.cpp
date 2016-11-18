/****************************************************************************
** Meta object code from reading C++ file 'multiviewrti.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/io/multiviewrti.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multiviewrti.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewpointControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   18,   17,   17, 0x05,
      46,   18,   17,   17, 0x05,
      67,   18,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      83,   18,   17,   17, 0x08,
     101,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ViewpointControl[] = {
    "ViewpointControl\0\0value\0viewpointChanged(int)\0"
    "snapModeChanged(int)\0rowChanged(int)\0"
    "updateSlider(int)\0sliderReleased()\0"
};

void ViewpointControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ViewpointControl *_t = static_cast<ViewpointControl *>(_o);
        switch (_id) {
        case 0: _t->viewpointChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->snapModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->rowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->updateSlider((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->sliderReleased(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ViewpointControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ViewpointControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ViewpointControl,
      qt_meta_data_ViewpointControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewpointControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewpointControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewpointControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewpointControl))
        return static_cast<void*>(const_cast< ViewpointControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int ViewpointControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ViewpointControl::viewpointChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ViewpointControl::snapModeChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ViewpointControl::rowChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_DefaultMRti[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   28,   12,   12, 0x0a,
      47,   28,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DefaultMRti[] = {
    "DefaultMRti\0\0refreshImage()\0value\0"
    "setPosX(int)\0setPosY(int)\0"
};

void DefaultMRti::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DefaultMRti *_t = static_cast<DefaultMRti *>(_o);
        switch (_id) {
        case 0: _t->refreshImage(); break;
        case 1: _t->setPosX((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setPosY((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DefaultMRti::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DefaultMRti::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DefaultMRti,
      qt_meta_data_DefaultMRti, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DefaultMRti::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DefaultMRti::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DefaultMRti::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DefaultMRti))
        return static_cast<void*>(const_cast< DefaultMRti*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< DefaultMRti*>(this));
    return QObject::qt_metacast(_clname);
}

int DefaultMRti::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DefaultMRti::refreshImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
