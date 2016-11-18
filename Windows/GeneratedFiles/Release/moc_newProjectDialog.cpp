/****************************************************************************
** Meta object code from reading C++ file 'newProjectDialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/information/newProjectDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newProjectDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NewProjectDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      40,   31,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     126,   17,   17,   17, 0x08,
     143,   17,   17,   17, 0x08,
     158,   17,   17,   17, 0x08,
     169,   17,   17,   17, 0x08,
     176,   17,   17,   17, 0x08,
     190,  185,   17,   17, 0x08,
     223,  218,   17,   17, 0x08,
     251,  185,   17,   17, 0x08,
     276,   17,   17,   17, 0x08,
     295,   17,   17,   17, 0x08,
     307,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NewProjectDialog[] = {
    "NewProjectDialog\0\0infoUpdate()\0,,,,,,,,\0"
    "nextPressed(QString,QString,USERMODE,QString,QString,QString,QString,Q"
    "String,QString)\0"
    "locationBrowse()\0objectBrowse()\0"
    "ctBrowse()\0next()\0cancel()\0name\0"
    "projectNameChanged(QString)\0path\0"
    "projectPathChanged(QString)\0"
    "userNameChanged(QString)\0enableNextButton()\0"
    "nextReady()\0nextCancel()\0"
};

void NewProjectDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NewProjectDialog *_t = static_cast<NewProjectDialog *>(_o);
        switch (_id) {
        case 0: _t->infoUpdate(); break;
        case 1: _t->nextPressed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const USERMODE(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QString(*)>(_a[6])),(*reinterpret_cast< const QString(*)>(_a[7])),(*reinterpret_cast< const QString(*)>(_a[8])),(*reinterpret_cast< const QString(*)>(_a[9]))); break;
        case 2: _t->locationBrowse(); break;
        case 3: _t->objectBrowse(); break;
        case 4: _t->ctBrowse(); break;
        case 5: _t->next(); break;
        case 6: _t->cancel(); break;
        case 7: _t->projectNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->projectPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->userNameChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->enableNextButton(); break;
        case 11: _t->nextReady(); break;
        case 12: _t->nextCancel(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NewProjectDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NewProjectDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NewProjectDialog,
      qt_meta_data_NewProjectDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NewProjectDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NewProjectDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NewProjectDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NewProjectDialog))
        return static_cast<void*>(const_cast< NewProjectDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int NewProjectDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void NewProjectDialog::infoUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void NewProjectDialog::nextPressed(const QString _t1, const QString _t2, const USERMODE _t3, const QString _t4, const QString _t5, const QString _t6, const QString _t7, const QString _t8, const QString _t9)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)), const_cast<void*>(reinterpret_cast<const void*>(&_t9)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
