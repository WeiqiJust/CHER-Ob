/****************************************************************************
** Meta object code from reading C++ file 'vtkView.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/visualization/vtkView.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vtkView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VtkSplitter[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_VtkSplitter[] = {
    "VtkSplitter\0"
};

void VtkSplitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData VtkSplitter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VtkSplitter::staticMetaObject = {
    { &QSplitter::staticMetaObject, qt_meta_stringdata_VtkSplitter,
      qt_meta_data_VtkSplitter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VtkSplitter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VtkSplitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VtkSplitter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VtkSplitter))
        return static_cast<void*>(const_cast< VtkSplitter*>(this));
    return QSplitter::qt_metacast(_clname);
}

int VtkSplitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSplitter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_VtkSplitterHandle[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_VtkSplitterHandle[] = {
    "VtkSplitterHandle\0"
};

void VtkSplitterHandle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData VtkSplitterHandle::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VtkSplitterHandle::staticMetaObject = {
    { &QSplitterHandle::staticMetaObject, qt_meta_stringdata_VtkSplitterHandle,
      qt_meta_data_VtkSplitterHandle, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VtkSplitterHandle::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VtkSplitterHandle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VtkSplitterHandle::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VtkSplitterHandle))
        return static_cast<void*>(const_cast< VtkSplitterHandle*>(this));
    return QSplitterHandle::qt_metacast(_clname);
}

int VtkSplitterHandle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSplitterHandle::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_VtkView[] = {

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
       9,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,   33,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VtkView[] = {
    "VtkView\0\0updateMainWindowMenus()\0"
    "current\0updateCurrent(int)\0"
};

void VtkView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VtkView *_t = static_cast<VtkView *>(_o);
        switch (_id) {
        case 0: _t->updateMainWindowMenus(); break;
        case 1: _t->updateCurrent((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VtkView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VtkView::staticMetaObject = {
    { &VtkSplitter::staticMetaObject, qt_meta_stringdata_VtkView,
      qt_meta_data_VtkView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VtkView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VtkView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VtkView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VtkView))
        return static_cast<void*>(const_cast< VtkView*>(this));
    return VtkSplitter::qt_metacast(_clname);
}

int VtkView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VtkSplitter::qt_metacall(_c, _id, _a);
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
void VtkView::updateMainWindowMenus()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
