/****************************************************************************
** Meta object code from reading C++ file 'CHEInfoDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/CHE/CHEInfoDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CHEInfoDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CHEInfoBasic[] = {

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

static const char qt_meta_stringdata_CHEInfoBasic[] = {
    "CHEInfoBasic\0"
};

void CHEInfoBasic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CHEInfoBasic::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CHEInfoBasic::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CHEInfoBasic,
      qt_meta_data_CHEInfoBasic, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CHEInfoBasic::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CHEInfoBasic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CHEInfoBasic::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CHEInfoBasic))
        return static_cast<void*>(const_cast< CHEInfoBasic*>(this));
    return QWidget::qt_metacast(_clname);
}

int CHEInfoBasic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_CHEInfoDialog[] = {

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

static const char qt_meta_stringdata_CHEInfoDialog[] = {
    "CHEInfoDialog\0"
};

void CHEInfoDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CHEInfoDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CHEInfoDialog::staticMetaObject = {
    { &CHEInfoBasic::staticMetaObject, qt_meta_stringdata_CHEInfoDialog,
      qt_meta_data_CHEInfoDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CHEInfoDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CHEInfoDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CHEInfoDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CHEInfoDialog))
        return static_cast<void*>(const_cast< CHEInfoDialog*>(this));
    return CHEInfoBasic::qt_metacast(_clname);
}

int CHEInfoDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CHEInfoBasic::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
