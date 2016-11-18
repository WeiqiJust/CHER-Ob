/****************************************************************************
** Meta object code from reading C++ file 'lightControlRTI.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/lightControlRTI.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lightControlRTI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LightControlRTI[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   17,   16,   16, 0x05,
      61,   59,   16,   16, 0x25,

 // slots: signature, parameters, type, tag, flags
      88,   16,   16,   16, 0x08,
     102,   96,   16,   16, 0x0a,
     123,   96,   16,   16, 0x0a,
     150,  140,   16,   16, 0x0a,
     181,   17,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LightControlRTI[] = {
    "LightControlRTI\0\0l,refresh\0"
    "lightChanged(vcg::Point3f,bool)\0l\0"
    "lightChanged(vcg::Point3f)\0fired()\0"
    "value\0setInteractive(bool)\0setEnabled(bool)\0"
    "offx,offy\0moveLightPosition(float,float)\0"
    "setLight(vcg::Point3f,bool)\0"
};

void LightControlRTI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LightControlRTI *_t = static_cast<LightControlRTI *>(_o);
        switch (_id) {
        case 0: _t->lightChanged((*reinterpret_cast< vcg::Point3f(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->lightChanged((*reinterpret_cast< vcg::Point3f(*)>(_a[1]))); break;
        case 2: _t->fired(); break;
        case 3: _t->setInteractive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->moveLightPosition((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 6: _t->setLight((*reinterpret_cast< const vcg::Point3f(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LightControlRTI::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LightControlRTI::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LightControlRTI,
      qt_meta_data_LightControlRTI, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LightControlRTI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LightControlRTI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LightControlRTI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LightControlRTI))
        return static_cast<void*>(const_cast< LightControlRTI*>(this));
    return QWidget::qt_metacast(_clname);
}

int LightControlRTI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void LightControlRTI::lightChanged(vcg::Point3f _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
