/****************************************************************************
** Meta object code from reading C++ file 'diffusegain.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/diffusegain.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'diffusegain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DiffuseGControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   17,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DiffuseGControl[] = {
    "DiffuseGControl\0\0value\0gainChanged(int)\0"
};

void DiffuseGControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DiffuseGControl *_t = static_cast<DiffuseGControl *>(_o);
        switch (_id) {
        case 0: _t->gainChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DiffuseGControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DiffuseGControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DiffuseGControl,
      qt_meta_data_DiffuseGControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DiffuseGControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DiffuseGControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DiffuseGControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DiffuseGControl))
        return static_cast<void*>(const_cast< DiffuseGControl*>(this));
    if (!strcmp(_clname, "RenderControlUtils"))
        return static_cast< RenderControlUtils*>(const_cast< DiffuseGControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int DiffuseGControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DiffuseGControl::gainChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_DiffuseGain[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   28,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DiffuseGain[] = {
    "DiffuseGain\0\0refreshImage()\0value\0"
    "setGain(int)\0"
};

void DiffuseGain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DiffuseGain *_t = static_cast<DiffuseGain *>(_o);
        switch (_id) {
        case 0: _t->refreshImage(); break;
        case 1: _t->setGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DiffuseGain::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DiffuseGain::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DiffuseGain,
      qt_meta_data_DiffuseGain, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DiffuseGain::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DiffuseGain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DiffuseGain::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DiffuseGain))
        return static_cast<void*>(const_cast< DiffuseGain*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< DiffuseGain*>(this));
    return QObject::qt_metacast(_clname);
}

int DiffuseGain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DiffuseGain::refreshImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
