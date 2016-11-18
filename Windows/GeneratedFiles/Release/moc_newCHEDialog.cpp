/****************************************************************************
** Meta object code from reading C++ file 'newCHEDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/CHE/newCHEDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newCHEDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NewCHEDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      34,   27,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     114,   13,   13,   13, 0x08,
     131,   13,   13,   13, 0x08,
     146,   13,   13,   13, 0x08,
     157,   13,   13,   13, 0x08,
     164,   13,   13,   13, 0x08,
     178,  173,   13,   13, 0x08,
     207,  202,   13,   13, 0x08,
     231,  173,   13,   13, 0x08,
     256,   13,   13,   13, 0x08,
     275,   13,   13,   13, 0x08,
     287,   13,   13,   13, 0x08,
     305,  300,   13,   13, 0x08,
     336,   13,   13,   13, 0x08,
     350,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NewCHEDialog[] = {
    "NewCHEDialog\0\0infoUpdate()\0,,,,,,\0"
    "createCHE(QString,QString,USERMODE,const CHEInfoBasic*,QString,QString"
    ",QString)\0"
    "locationBrowse()\0objectBrowse()\0"
    "ctBrowse()\0next()\0cancel()\0name\0"
    "CHENameChanged(QString)\0path\0"
    "CHEPathChanged(QString)\0"
    "userNameChanged(QString)\0enableNextButton()\0"
    "nextReady()\0nextCancel()\0info\0"
    "CHEInfoOK(const CHEInfoBasic*)\0"
    "CHEInfoBack()\0CHEInfoCancel()\0"
};

void NewCHEDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NewCHEDialog *_t = static_cast<NewCHEDialog *>(_o);
        switch (_id) {
        case 0: _t->infoUpdate(); break;
        case 1: _t->createCHE((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const USERMODE(*)>(_a[3])),(*reinterpret_cast< const CHEInfoBasic*(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QString(*)>(_a[6])),(*reinterpret_cast< const QString(*)>(_a[7]))); break;
        case 2: _t->locationBrowse(); break;
        case 3: _t->objectBrowse(); break;
        case 4: _t->ctBrowse(); break;
        case 5: _t->next(); break;
        case 6: _t->cancel(); break;
        case 7: _t->CHENameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->CHEPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->userNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->enableNextButton(); break;
        case 11: _t->nextReady(); break;
        case 12: _t->nextCancel(); break;
        case 13: _t->CHEInfoOK((*reinterpret_cast< const CHEInfoBasic*(*)>(_a[1]))); break;
        case 14: _t->CHEInfoBack(); break;
        case 15: _t->CHEInfoCancel(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NewCHEDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NewCHEDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NewCHEDialog,
      qt_meta_data_NewCHEDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NewCHEDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NewCHEDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NewCHEDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NewCHEDialog))
        return static_cast<void*>(const_cast< NewCHEDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int NewCHEDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void NewCHEDialog::infoUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void NewCHEDialog::createCHE(const QString _t1, const QString _t2, const USERMODE _t3, const CHEInfoBasic * _t4, const QString _t5, const QString _t6, const QString _t7)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
