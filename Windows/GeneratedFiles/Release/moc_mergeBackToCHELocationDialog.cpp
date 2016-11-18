/****************************************************************************
** Meta object code from reading C++ file 'mergeBackToCHELocationDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/CHE/mergeBackToCHELocationDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mergeBackToCHELocationDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MergeBackToCHELocationDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x08,
      35,   29,   29,   29, 0x08,
      56,   44,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MergeBackToCHELocationDialog[] = {
    "MergeBackToCHELocationDialog\0\0ok()\0"
    "cancel()\0item,column\0"
    "itemChanged(QTreeWidgetItem*,int)\0"
};

void MergeBackToCHELocationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MergeBackToCHELocationDialog *_t = static_cast<MergeBackToCHELocationDialog *>(_o);
        switch (_id) {
        case 0: _t->ok(); break;
        case 1: _t->cancel(); break;
        case 2: _t->itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MergeBackToCHELocationDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MergeBackToCHELocationDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MergeBackToCHELocationDialog,
      qt_meta_data_MergeBackToCHELocationDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MergeBackToCHELocationDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MergeBackToCHELocationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MergeBackToCHELocationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MergeBackToCHELocationDialog))
        return static_cast<void*>(const_cast< MergeBackToCHELocationDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int MergeBackToCHELocationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
