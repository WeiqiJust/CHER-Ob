/****************************************************************************
** Meta object code from reading C++ file 'specularenhanc.h'
**
** Created: Tue Dec 22 11:15:09 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../rtiviewer/src/specularenhanc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'specularenhanc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpecularEControl[] = {

 // content:
       5,       // revision
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

const QMetaObject SpecularEControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SpecularEControl,
      qt_meta_data_SpecularEControl, 0 }
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
        switch (_id) {
        case 0: kdChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: ksChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: expChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
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
       5,       // revision
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

const QMetaObject SpecularEnhancement::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SpecularEnhancement,
      qt_meta_data_SpecularEnhancement, 0 }
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
        switch (_id) {
        case 0: refreshImage(); break;
        case 1: setKd((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setKs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setExp((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
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
