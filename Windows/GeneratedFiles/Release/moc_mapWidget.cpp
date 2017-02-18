/****************************************************************************
** Meta object code from reading C++ file 'mapWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/mapKit/mapWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mapWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MapWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   11,   10,   10, 0x0a,
      52,   47,   10,   10, 0x2a,
      65,   11,   10,   10, 0x0a,
      90,   47,   10,   10, 0x2a,
     107,   10,   10,   10, 0x08,
     120,   10,   10,   10, 0x08,
     142,  134,   10,   10, 0x08,
     172,   10,   10,   10, 0x08,
     197,  190,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MapWidget[] = {
    "MapWidget\0\0text,delimiter\0"
    "log(QString,QString)\0text\0log(QString)\0"
    "log(const char*,QString)\0log(const char*)\0"
    "createMark()\0onMapLoaded()\0latlong\0"
    "onMouseClicked(mapCoordinate)\0"
    "onMapBecameIdle()\0region\0"
    "onRegionChanged(mapCoordinateRegion)\0"
};

void MapWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MapWidget *_t = static_cast<MapWidget *>(_o);
        switch (_id) {
        case 0: _t->log((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->log((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->log((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->log((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 4: _t->createMark(); break;
        case 5: _t->onMapLoaded(); break;
        case 6: _t->onMouseClicked((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 7: _t->onMapBecameIdle(); break;
        case 8: _t->onRegionChanged((*reinterpret_cast< mapCoordinateRegion(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MapWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MapWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MapWidget,
      qt_meta_data_MapWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MapWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MapWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MapWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MapWidget))
        return static_cast<void*>(const_cast< MapWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int MapWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
