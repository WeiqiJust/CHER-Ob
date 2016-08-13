/****************************************************************************
** Meta object code from reading C++ file 'defaultrendering.h'
**
** Created: Tue Dec 22 11:15:10 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../rtiviewer/src/defaultrendering.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'defaultrendering.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LoadRemoteWidget[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_LoadRemoteWidget[] = {
    "LoadRemoteWidget\0"
};

const QMetaObject LoadRemoteWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LoadRemoteWidget,
      qt_meta_data_LoadRemoteWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LoadRemoteWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LoadRemoteWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LoadRemoteWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LoadRemoteWidget))
        return static_cast<void*>(const_cast< LoadRemoteWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int LoadRemoteWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_DefaultRendering[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DefaultRendering[] = {
    "DefaultRendering\0\0resetRemote()\0"
};

const QMetaObject DefaultRendering::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DefaultRendering,
      qt_meta_data_DefaultRendering, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DefaultRendering::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DefaultRendering::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DefaultRendering::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DefaultRendering))
        return static_cast<void*>(const_cast< DefaultRendering*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< DefaultRendering*>(this));
    return QObject::qt_metacast(_clname);
}

int DefaultRendering::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resetRemote(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
