/****************************************************************************
** Meta object code from reading C++ file 'plotView.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/plotView.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plotView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PlotView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      48,   10,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     143,   10,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PlotView[] = {
    "PlotView\0\0wavelengths,hyperPixels,icoords,fname\0"
    "currentHyperPixelsChanged(std::vector<float>,std::vector<float>,const "
    "int*,const std::string*)\0"
    "updateSpectralPlot(std::vector<float>,std::vector<float>,const int*,co"
    "nst std::string*)\0"
};

void PlotView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PlotView *_t = static_cast<PlotView *>(_o);
        switch (_id) {
        case 0: _t->currentHyperPixelsChanged((*reinterpret_cast< std::vector<float>(*)>(_a[1])),(*reinterpret_cast< std::vector<float>(*)>(_a[2])),(*reinterpret_cast< const int*(*)>(_a[3])),(*reinterpret_cast< const std::string*(*)>(_a[4]))); break;
        case 1: _t->updateSpectralPlot((*reinterpret_cast< std::vector<float>(*)>(_a[1])),(*reinterpret_cast< std::vector<float>(*)>(_a[2])),(*reinterpret_cast< const int*(*)>(_a[3])),(*reinterpret_cast< const std::string*(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PlotView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PlotView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PlotView,
      qt_meta_data_PlotView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PlotView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PlotView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PlotView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PlotView))
        return static_cast<void*>(const_cast< PlotView*>(this));
    return QWidget::qt_metacast(_clname);
}

int PlotView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PlotView::currentHyperPixelsChanged(std::vector<float> _t1, std::vector<float> _t2, const int * _t3, const std::string * _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
