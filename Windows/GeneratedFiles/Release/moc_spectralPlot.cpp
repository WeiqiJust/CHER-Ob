/****************************************************************************
** Meta object code from reading C++ file 'spectralPlot.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/spectralPlot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectralPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpectralPlot[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      52,   14,   13,   13, 0x0a,
     140,   13,   13,   13, 0x0a,
     159,  153,   13,   13, 0x0a,
     192,  184,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpectralPlot[] = {
    "SpectralPlot\0\0wavelengths,hyperPixels,icoords,fname\0"
    "updateSpectralPlot(std::vector<float>,std::vector<float>,const int*,co"
    "nst std::string*)\0"
    "exportPlot()\0state\0set_normalized_plot(int)\0"
    "item,on\0showCurve(QwtPlotItem*,bool)\0"
};

void SpectralPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpectralPlot *_t = static_cast<SpectralPlot *>(_o);
        switch (_id) {
        case 0: _t->updateSpectralPlot((*reinterpret_cast< std::vector<float>(*)>(_a[1])),(*reinterpret_cast< std::vector<float>(*)>(_a[2])),(*reinterpret_cast< const int*(*)>(_a[3])),(*reinterpret_cast< const std::string*(*)>(_a[4]))); break;
        case 1: _t->exportPlot(); break;
        case 2: _t->set_normalized_plot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showCurve((*reinterpret_cast< QwtPlotItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpectralPlot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpectralPlot::staticMetaObject = {
    { &QwtPlot::staticMetaObject, qt_meta_stringdata_SpectralPlot,
      qt_meta_data_SpectralPlot, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpectralPlot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpectralPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpectralPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpectralPlot))
        return static_cast<void*>(const_cast< SpectralPlot*>(this));
    return QwtPlot::qt_metacast(_clname);
}

int SpectralPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
