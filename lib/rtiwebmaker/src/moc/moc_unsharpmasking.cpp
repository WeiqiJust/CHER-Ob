/****************************************************************************
** Meta object code from reading C++ file 'unsharpmasking.h'
**
** Created: Tue Dec 22 11:15:08 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../rtiviewer/src/unsharpmasking.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'unsharpmasking.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UnsharpMControl[] = {

 // content:
       5,       // revision
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

static const char qt_meta_stringdata_UnsharpMControl[] = {
    "UnsharpMControl\0\0value\0gainChanged(int)\0"
};

const QMetaObject UnsharpMControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_UnsharpMControl,
      qt_meta_data_UnsharpMControl, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UnsharpMControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UnsharpMControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UnsharpMControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UnsharpMControl))
        return static_cast<void*>(const_cast< UnsharpMControl*>(this));
    if (!strcmp(_clname, "RenderControlUtils"))
        return static_cast< RenderControlUtils*>(const_cast< UnsharpMControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int UnsharpMControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: gainChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void UnsharpMControl::gainChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_UnsharpMasking[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   31,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UnsharpMasking[] = {
    "UnsharpMasking\0\0refreshImage()\0value\0"
    "setGain(int)\0"
};

const QMetaObject UnsharpMasking::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UnsharpMasking,
      qt_meta_data_UnsharpMasking, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UnsharpMasking::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UnsharpMasking::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UnsharpMasking::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UnsharpMasking))
        return static_cast<void*>(const_cast< UnsharpMasking*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< UnsharpMasking*>(this));
    return QObject::qt_metacast(_clname);
}

int UnsharpMasking::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refreshImage(); break;
        case 1: setGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void UnsharpMasking::refreshImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
