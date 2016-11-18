/****************************************************************************
** Meta object code from reading C++ file 'exportToProjectDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/CHE/exportToProjectDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'exportToProjectDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExportToProjectDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   22,   22,   22, 0x08,
      53,   22,   22,   22, 0x08,
      60,   22,   22,   22, 0x08,
      73,   22,   22,   22, 0x08,
      82,   22,   22,   22, 0x08,
      94,   22,   22,   22, 0x08,
     106,   22,   22,   22, 0x08,
     130,  125,   22,   22, 0x08,
     163,  158,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ExportToProjectDialog[] = {
    "ExportToProjectDialog\0\0infoUpdate()\0"
    "locationBrowse()\0next()\0nextCancel()\0"
    "cancel()\0selectAll()\0nextReady()\0"
    "enableNextButton()\0name\0"
    "projectNameChanged(QString)\0path\0"
    "projectPathChanged(QString)\0"
};

void ExportToProjectDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ExportToProjectDialog *_t = static_cast<ExportToProjectDialog *>(_o);
        switch (_id) {
        case 0: _t->infoUpdate(); break;
        case 1: _t->locationBrowse(); break;
        case 2: _t->next(); break;
        case 3: _t->nextCancel(); break;
        case 4: _t->cancel(); break;
        case 5: _t->selectAll(); break;
        case 6: _t->nextReady(); break;
        case 7: _t->enableNextButton(); break;
        case 8: _t->projectNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->projectPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ExportToProjectDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ExportToProjectDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ExportToProjectDialog,
      qt_meta_data_ExportToProjectDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ExportToProjectDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ExportToProjectDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ExportToProjectDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExportToProjectDialog))
        return static_cast<void*>(const_cast< ExportToProjectDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int ExportToProjectDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ExportToProjectDialog::infoUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
