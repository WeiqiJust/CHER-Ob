/****************************************************************************
** Meta object code from reading C++ file 'CHETab.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/CHE/CHETab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CHETab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CHETab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      15,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,    7,    7,    7, 0x0a,
      45,    7,    7,    7, 0x08,
      61,    7,    7,    7, 0x08,
      80,   75,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CHETab[] = {
    "CHETab\0\0save()\0exportProject()\0"
    "savePressed()\0exportPressed()\0"
    "editPressed()\0mode\0setFilterMode(int)\0"
};

void CHETab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CHETab *_t = static_cast<CHETab *>(_o);
        switch (_id) {
        case 0: _t->save(); break;
        case 1: _t->exportProject(); break;
        case 2: _t->savePressed(); break;
        case 3: _t->exportPressed(); break;
        case 4: _t->editPressed(); break;
        case 5: _t->setFilterMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CHETab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CHETab::staticMetaObject = {
    { &CHEInfoDialog::staticMetaObject, qt_meta_stringdata_CHETab,
      qt_meta_data_CHETab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CHETab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CHETab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CHETab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CHETab))
        return static_cast<void*>(const_cast< CHETab*>(this));
    return CHEInfoDialog::qt_metacast(_clname);
}

int CHETab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CHEInfoDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CHETab::save()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CHETab::exportProject()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
