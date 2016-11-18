/****************************************************************************
** Meta object code from reading C++ file 'lightControl.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/lightControl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lightControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LightControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   14,   13,   13, 0x05,
      83,   62,   13,   13, 0x05,
     154,  132,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     188,   13,   13,   13, 0x0a,
     200,  196,   13,   13, 0x08,
     250,   13,   13,   13, 0x08,
     284,  273,   13,   13, 0x08,
     325,  315,   13,   13, 0x08,
     355,  315,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LightControl[] = {
    "LightControl\0\0transform\0"
    "signalLitCamPosChanged(vtkTransform*)\0"
    "lightX,lightY,lightZ\0"
    "signalLightPositionChanged(double,double,double)\0"
    "intensity1,intensity2\0"
    "signalIntensityL12(double,double)\0"
    "reset()\0,,,\0"
    "getLitVectorControl(vtkObject*,ulong,void*,void*)\0"
    "updateLightingVector()\0widgetmode\0"
    "updateLightControl(WidgetMode)\0intensity\0"
    "changeBrightnessLocal(double)\0"
    "changeContrastLocal(double)\0"
};

void LightControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LightControl *_t = static_cast<LightControl *>(_o);
        switch (_id) {
        case 0: _t->signalLitCamPosChanged((*reinterpret_cast< vtkTransform*(*)>(_a[1]))); break;
        case 1: _t->signalLightPositionChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 2: _t->signalIntensityL12((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 3: _t->reset(); break;
        case 4: _t->getLitVectorControl((*reinterpret_cast< vtkObject*(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2])),(*reinterpret_cast< void*(*)>(_a[3])),(*reinterpret_cast< void*(*)>(_a[4]))); break;
        case 5: _t->updateLightingVector(); break;
        case 6: _t->updateLightControl((*reinterpret_cast< WidgetMode(*)>(_a[1]))); break;
        case 7: _t->changeBrightnessLocal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->changeContrastLocal((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LightControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LightControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LightControl,
      qt_meta_data_LightControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LightControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LightControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LightControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LightControl))
        return static_cast<void*>(const_cast< LightControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int LightControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void LightControl::signalLitCamPosChanged(vtkTransform * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LightControl::signalLightPositionChanged(double _t1, double _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LightControl::signalIntensityL12(double _t1, double _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
