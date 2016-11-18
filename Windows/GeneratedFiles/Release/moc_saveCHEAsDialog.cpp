/****************************************************************************
** Meta object code from reading C++ file 'saveCHEAsDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/CHE/saveCHEAsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'saveCHEAsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SaveCHEAsDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   16,   16,   16, 0x08,
      47,   16,   16,   16, 0x08,
      54,   16,   16,   16, 0x08,
      68,   63,   16,   16, 0x08,
     101,   96,   16,   16, 0x08,
     129,   16,   16,   16, 0x08,
     148,   16,   16,   16, 0x08,
     160,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SaveCHEAsDialog[] = {
    "SaveCHEAsDialog\0\0infoUpdate()\0"
    "locationBrowse()\0next()\0cancel()\0name\0"
    "projectNameChanged(QString)\0path\0"
    "projectPathChanged(QString)\0"
    "enableNextButton()\0nextReady()\0"
    "nextCancel()\0"
};

void SaveCHEAsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SaveCHEAsDialog *_t = static_cast<SaveCHEAsDialog *>(_o);
        switch (_id) {
        case 0: _t->infoUpdate(); break;
        case 1: _t->locationBrowse(); break;
        case 2: _t->next(); break;
        case 3: _t->cancel(); break;
        case 4: _t->projectNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->projectPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->enableNextButton(); break;
        case 7: _t->nextReady(); break;
        case 8: _t->nextCancel(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SaveCHEAsDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SaveCHEAsDialog::staticMetaObject = {
    { &CHEInfoDialog::staticMetaObject, qt_meta_stringdata_SaveCHEAsDialog,
      qt_meta_data_SaveCHEAsDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SaveCHEAsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SaveCHEAsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SaveCHEAsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SaveCHEAsDialog))
        return static_cast<void*>(const_cast< SaveCHEAsDialog*>(this));
    return CHEInfoDialog::qt_metacast(_clname);
}

int SaveCHEAsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CHEInfoDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SaveCHEAsDialog::infoUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
