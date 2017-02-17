/****************************************************************************
** Meta object code from reading C++ file 'QtMapWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/QtMapKit/QtMapWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtMapWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtMapWidget[] = {

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
      28,   13,   12,   12, 0x0a,
      54,   49,   12,   12, 0x2a,
      67,   13,   12,   12, 0x0a,
      92,   49,   12,   12, 0x2a,
     109,   12,   12,   12, 0x08,
     122,   12,   12,   12, 0x08,
     144,  136,   12,   12, 0x08,
     173,   12,   12,   12, 0x08,
     198,  191,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtMapWidget[] = {
    "QtMapWidget\0\0text,delimiter\0"
    "log(QString,QString)\0text\0log(QString)\0"
    "log(const char*,QString)\0log(const char*)\0"
    "createMark()\0onMapLoaded()\0latlong\0"
    "onMouseClicked(QMCoordinate)\0"
    "onMapBecameIdle()\0region\0"
    "onRegionChanged(QMCoordinateRegion)\0"
};

void QtMapWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QtMapWidget *_t = static_cast<QtMapWidget *>(_o);
        switch (_id) {
        case 0: _t->log((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->log((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->log((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->log((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 4: _t->createMark(); break;
        case 5: _t->onMapLoaded(); break;
        case 6: _t->onMouseClicked((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
        case 7: _t->onMapBecameIdle(); break;
        case 8: _t->onRegionChanged((*reinterpret_cast< QMCoordinateRegion(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QtMapWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QtMapWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtMapWidget,
      qt_meta_data_QtMapWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QtMapWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QtMapWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QtMapWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtMapWidget))
        return static_cast<void*>(const_cast< QtMapWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtMapWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
