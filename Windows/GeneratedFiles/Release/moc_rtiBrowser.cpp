/****************************************************************************
** Meta object code from reading C++ file 'rtiBrowser.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/rtiBrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rtiBrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RtiBrowser[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x05,
      44,   12,   11,   11, 0x05,
     115,   66,   11,   11, 0x05,
     188,  172,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     249,  239,   11,   11, 0x0a,
     279,  277,   11,   11, 0x2a,
     307,  302,   11,   11, 0x0a,
     329,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RtiBrowser[] = {
    "RtiBrowser\0\0value\0setInteractiveLight(bool)\0"
    "setEnabledLight(bool)\0"
    "data,textureWidth,textureHeight,FIRST_RTI_UPDATE\0"
    "rtiImageChanged(std::vector<unsigned char>,int,int,bool)\0"
    "l,currRendering\0"
    "rtiImageLoadingDone(QMap<int,RenderingMode*>*,int)\0"
    "l,refresh\0setLight(vcg::Point3f,bool)\0"
    "l\0setLight(vcg::Point3f)\0mode\0"
    "setRenderingMode(int)\0updateImage()\0"
};

void RtiBrowser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RtiBrowser *_t = static_cast<RtiBrowser *>(_o);
        switch (_id) {
        case 0: _t->setInteractiveLight((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setEnabledLight((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->rtiImageChanged((*reinterpret_cast< std::vector<unsigned char>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 3: _t->rtiImageLoadingDone((*reinterpret_cast< QMap<int,RenderingMode*>*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->setLight((*reinterpret_cast< vcg::Point3f(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->setLight((*reinterpret_cast< vcg::Point3f(*)>(_a[1]))); break;
        case 6: _t->setRenderingMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->updateImage(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RtiBrowser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RtiBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RtiBrowser,
      qt_meta_data_RtiBrowser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RtiBrowser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RtiBrowser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RtiBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RtiBrowser))
        return static_cast<void*>(const_cast< RtiBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int RtiBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void RtiBrowser::setInteractiveLight(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RtiBrowser::setEnabledLight(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RtiBrowser::rtiImageChanged(std::vector<unsigned char> _t1, int _t2, int _t3, bool _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RtiBrowser::rtiImageLoadingDone(QMap<int,RenderingMode*> * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
