/****************************************************************************
** Meta object code from reading C++ file 'mergeBackToCHEDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/CHE/mergeBackToCHEDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mergeBackToCHEDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MergeBackToCHEDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,
      30,   21,   21,   21, 0x08,
      39,   21,   21,   21, 0x08,
      63,   51,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MergeBackToCHEDialog[] = {
    "MergeBackToCHEDialog\0\0merge()\0cancel()\0"
    "selectAll()\0item,column\0"
    "itemChanged(QTreeWidgetItem*,int)\0"
};

void MergeBackToCHEDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MergeBackToCHEDialog *_t = static_cast<MergeBackToCHEDialog *>(_o);
        switch (_id) {
        case 0: _t->merge(); break;
        case 1: _t->cancel(); break;
        case 2: _t->selectAll(); break;
        case 3: _t->itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MergeBackToCHEDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MergeBackToCHEDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MergeBackToCHEDialog,
      qt_meta_data_MergeBackToCHEDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MergeBackToCHEDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MergeBackToCHEDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MergeBackToCHEDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MergeBackToCHEDialog))
        return static_cast<void*>(const_cast< MergeBackToCHEDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int MergeBackToCHEDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
