/****************************************************************************
** Meta object code from reading C++ file 'specularenhanc.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/specularenhanc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'specularenhanc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpecularEControl[] = {

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
      24,   18,   17,   17, 0x05,
      39,   18,   17,   17, 0x05,
      54,   18,   17,   17, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_SpecularEControl[] = {
    "SpecularEControl\0\0value\0kdChanged(int)\0"
    "ksChanged(int)\0expChanged(int)\0"
};

void SpecularEControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpecularEControl *_t = static_cast<SpecularEControl *>(_o);
        switch (_id) {
        case 0: _t->kdChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->ksChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->expChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpecularEControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpecularEControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SpecularEControl,
      qt_meta_data_SpecularEControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpecularEControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpecularEControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpecularEControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpecularEControl))
        return static_cast<void*>(const_cast< SpecularEControl*>(this));
    if (!strcmp(_clname, "RenderControlUtils"))
        return static_cast< RenderControlUtils*>(const_cast< SpecularEControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int SpecularEControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SpecularEControl::kdChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SpecularEControl::ksChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SpecularEControl::expChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_SpecularEnhancement[] = {

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
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   36,   20,   20, 0x0a,
      53,   36,   20,   20, 0x0a,
      64,   36,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpecularEnhancement[] = {
    "SpecularEnhancement\0\0refreshImage()\0"
    "value\0setKd(int)\0setKs(int)\0setExp(int)\0"
};

void SpecularEnhancement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpecularEnhancement *_t = static_cast<SpecularEnhancement *>(_o);
        switch (_id) {
        case 0: _t->refreshImage(); break;
        case 1: _t->setKd((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setKs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setExp((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpecularEnhancement::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpecularEnhancement::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SpecularEnhancement,
      qt_meta_data_SpecularEnhancement, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpecularEnhancement::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpecularEnhancement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpecularEnhancement::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpecularEnhancement))
        return static_cast<void*>(const_cast< SpecularEnhancement*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< SpecularEnhancement*>(this));
    return QObject::qt_metacast(_clname);
}

int SpecularEnhancement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void SpecularEnhancement::refreshImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
