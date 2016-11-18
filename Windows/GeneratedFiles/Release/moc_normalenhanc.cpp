/****************************************************************************
** Meta object code from reading C++ file 'normalenhanc.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/normalenhanc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'normalenhanc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NormalEControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   16,   15,   15, 0x05,
      39,   16,   15,   15, 0x05,
      54,   16,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_NormalEControl[] = {
    "NormalEControl\0\0value\0gainChanged(int)\0"
    "kdChanged(int)\0envIllChanged(int)\0"
};

void NormalEControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NormalEControl *_t = static_cast<NormalEControl *>(_o);
        switch (_id) {
        case 0: _t->gainChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->kdChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->envIllChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NormalEControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NormalEControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NormalEControl,
      qt_meta_data_NormalEControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NormalEControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NormalEControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NormalEControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NormalEControl))
        return static_cast<void*>(const_cast< NormalEControl*>(this));
    if (!strcmp(_clname, "RenderControlUtils"))
        return static_cast< RenderControlUtils*>(const_cast< NormalEControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int NormalEControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void NormalEControl::gainChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NormalEControl::kdChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NormalEControl::envIllChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_NormalEnhancement[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   34,   18,   18, 0x0a,
      53,   34,   18,   18, 0x0a,
      64,   34,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NormalEnhancement[] = {
    "NormalEnhancement\0\0refreshImage()\0"
    "value\0setGain(int)\0setKd(int)\0"
    "setEnvIll(int)\0"
};

void NormalEnhancement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NormalEnhancement *_t = static_cast<NormalEnhancement *>(_o);
        switch (_id) {
        case 0: _t->refreshImage(); break;
        case 1: _t->setGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setKd((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setEnvIll((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NormalEnhancement::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NormalEnhancement::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NormalEnhancement,
      qt_meta_data_NormalEnhancement, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NormalEnhancement::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NormalEnhancement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NormalEnhancement::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NormalEnhancement))
        return static_cast<void*>(const_cast< NormalEnhancement*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< NormalEnhancement*>(this));
    return QObject::qt_metacast(_clname);
}

int NormalEnhancement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NormalEnhancement::refreshImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
