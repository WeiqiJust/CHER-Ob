/****************************************************************************
** Meta object code from reading C++ file 'CTControl.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/CTControl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CTControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CTControl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      49,   36,   10,   10, 0x05,
      85,   71,   10,   10, 0x05,
     123,   10,   10,   10, 0x05,
     156,  140,   10,   10, 0x05,
     200,  190,   10,   10, 0x05,
     247,  226,   10,   10, 0x05,
     313,  289,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     370,  361,   10,   10, 0x0a,
     403,  390,   10,   10, 0x0a,
     444,  427,   10,   10, 0x0a,
     498,  482,   10,   10, 0x0a,
     559,  540,   10,   10, 0x0a,
     602,  140,   10,   10, 0x0a,
     642,  631,   10,   10, 0x0a,
     680,   10,   10,   10, 0x0a,
     697,   10,   10,   10, 0x0a,
     714,   36,   10,   10, 0x08,
     742,   10,   10,   10, 0x08,
     761,   10,   10,   10, 0x08,
     778,   10,   10,   10, 0x08,
     796,   10,   10,   10, 0x08,
     816,   11,   10,   10, 0x08,
     843,  831,   10,   10, 0x08,
     879,  868,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CTControl[] = {
    "CTControl\0\0value\0sendaddonelcd(int)\0"
    "currentSlide\0sendSlideCurrent(int)\0"
    "currentOrient\0sendOrientationChanged(CTOrientation)\0"
    "sendUpDownView()\0reductionFactor\0"
    "sendReductionFactorChanged(float)\0"
    "blendType\0sendBlendTypeChanged(int)\0"
    "currentVisualization\0"
    "sendVisualizationChanged(CTVisualization)\0"
    "currentVolumeRenderMode\0"
    "sendVolumeRenderModeChanged(CTVolumeRenderMode)\0"
    "sliceMax\0updateSliceMax(int)\0sliceCurrent\0"
    "updateSliceCurrent(int)\0sliceOrientation\0"
    "updateSliceOrientation(CTOrientation)\0"
    "ctVisualization\0"
    "updateSliceVisualization(CTVisualization)\0"
    "ctVolumeRenderMode\0"
    "updateVolumeRenderMode(CTVolumeRenderMode)\0"
    "updateReductionFactor(float)\0widgetmode\0"
    "updateCtControlWidgetMode(WidgetMode)\0"
    "setCTStackView()\0setVTvolumeGPU()\0"
    "sendUpdateSlideCurrent(int)\0"
    "clickFrontButton()\0clickTopButton()\0"
    "clickSideButton()\0clickUpDownButton()\0"
    "addonelcd(int)\0sliderInput\0"
    "sendReductionFactor(int)\0comboInput\0"
    "sendVolumeRenderMode(int)\0"
};

void CTControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CTControl *_t = static_cast<CTControl *>(_o);
        switch (_id) {
        case 0: _t->sendaddonelcd((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->sendSlideCurrent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->sendOrientationChanged((*reinterpret_cast< CTOrientation(*)>(_a[1]))); break;
        case 3: _t->sendUpDownView(); break;
        case 4: _t->sendReductionFactorChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: _t->sendBlendTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->sendVisualizationChanged((*reinterpret_cast< CTVisualization(*)>(_a[1]))); break;
        case 7: _t->sendVolumeRenderModeChanged((*reinterpret_cast< CTVolumeRenderMode(*)>(_a[1]))); break;
        case 8: _t->updateSliceMax((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->updateSliceCurrent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->updateSliceOrientation((*reinterpret_cast< CTOrientation(*)>(_a[1]))); break;
        case 11: _t->updateSliceVisualization((*reinterpret_cast< CTVisualization(*)>(_a[1]))); break;
        case 12: _t->updateVolumeRenderMode((*reinterpret_cast< CTVolumeRenderMode(*)>(_a[1]))); break;
        case 13: _t->updateReductionFactor((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 14: _t->updateCtControlWidgetMode((*reinterpret_cast< WidgetMode(*)>(_a[1]))); break;
        case 15: _t->setCTStackView(); break;
        case 16: _t->setVTvolumeGPU(); break;
        case 17: _t->sendUpdateSlideCurrent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->clickFrontButton(); break;
        case 19: _t->clickTopButton(); break;
        case 20: _t->clickSideButton(); break;
        case 21: _t->clickUpDownButton(); break;
        case 22: _t->addonelcd((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->sendReductionFactor((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->sendVolumeRenderMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CTControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CTControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CTControl,
      qt_meta_data_CTControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CTControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CTControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CTControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CTControl))
        return static_cast<void*>(const_cast< CTControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int CTControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void CTControl::sendaddonelcd(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CTControl::sendSlideCurrent(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CTControl::sendOrientationChanged(CTOrientation _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CTControl::sendUpDownView()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void CTControl::sendReductionFactorChanged(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CTControl::sendBlendTypeChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CTControl::sendVisualizationChanged(CTVisualization _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CTControl::sendVolumeRenderModeChanged(CTVolumeRenderMode _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
