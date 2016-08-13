/****************************************************************************
** Meta object code from reading C++ file 'normalenhanc.h'
**
** Created: Tue Dec 22 11:15:09 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../rtiviewer/src/normalenhanc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'normalenhanc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NormalEControl[] = {

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
      22,   16,   15,   15, 0x05,
      39,   16,   15,   15, 0x05,
      54,   16,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_NormalEControl[] = {
    "NormalEControl\0\0value\0gainChanged(int)\0"
    "kdChanged(int)\0envIllChanged(int)\0"
};

const QMetaObject NormalEControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NormalEControl,
      qt_meta_data_NormalEControl, 0 }
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
        switch (_id) {
        case 0: gainChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: kdChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: envIllChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
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

const QMetaObject NormalEnhancement::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NormalEnhancement,
      qt_meta_data_NormalEnhancement, 0 }
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
        switch (_id) {
        case 0: refreshImage(); break;
        case 1: setGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setKd((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setEnvIll((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
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
