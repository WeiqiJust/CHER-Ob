/****************************************************************************
** Meta object code from reading C++ file 'mapView.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/mapKit/mapView.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mapView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_mapView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      21,    8,    8,    8, 0x05,
      44,   37,    8,    8, 0x05,
      86,   79,    8,    8, 0x05,
     126,  115,    8,    8, 0x05,
     154,  115,    8,    8, 0x05,
     188,  115,    8,    8, 0x05,
     221,    8,    8,    8, 0x05,
     236,    8,    8,    8, 0x05,
     255,    8,    8,    8, 0x05,
     272,  115,    8,    8, 0x05,
     299,  115,    8,    8, 0x05,
     328,  115,    8,    8, 0x05,
     356,    8,    8,    8, 0x05,
     378,  373,    8,    8, 0x05,
     402,    8,    8,    8, 0x05,
     416,    8,    8,    8, 0x05,
     437,  431,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     460,    8,    8,    8, 0x09,
     476,    8,    8,    8, 0x09,
     519,  497,    8,    8, 0x09,
     581,  562,    8,    8, 0x09,
     623,  612,    8,    8, 0x09,
     651,  562,    8,    8, 0x09,
     680,  562,    8,    8, 0x09,
     715,  562,    8,    8, 0x09,
     749,  562,    8,    8, 0x09,
     778,  562,    8,    8, 0x09,
     808,  562,    8,    8, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_mapView[] = {
    "mapView\0\0mapLoaded()\0mapBecameIdle()\0"
    "region\0regionChanged(mapCoordinateRegion)\0"
    "center\0centerChanged(mapCoordinate)\0"
    "coordinate\0mouseClicked(mapCoordinate)\0"
    "mouseDoubleClicked(mapCoordinate)\0"
    "mouseRightClicked(mapCoordinate)\0"
    "mouseDragged()\0mouseDragStarted()\0"
    "mouseDragEnded()\0cursorMoved(mapCoordinate)\0"
    "cursorEntered(mapCoordinate)\0"
    "cursorLeaved(mapCoordinate)\0"
    "headingChanged()\0type\0mapTypeChanged(MapType)\0"
    "tilesLoaded()\0tilesChanged()\0level\0"
    "zoomLevelChanged(uint)\0initializeMap()\0"
    "insertNativeObject()\0north,south,east,west\0"
    "regionDidChangeTo(qreal,qreal,qreal,qreal)\0"
    "latitude,longitude\0centerDidChangeTo(qreal,qreal)\0"
    "typeString\0mapTypeDidChangeTo(QString)\0"
    "mouseDidClickAt(qreal,qreal)\0"
    "mouseDidDoubleClickAt(qreal,qreal)\0"
    "mouseDidRightClickAt(qreal,qreal)\0"
    "cursorDidMoveTo(qreal,qreal)\0"
    "cursorDidEnterTo(qreal,qreal)\0"
    "cursorDidLeaveFrom(qreal,qreal)\0"
};

void mapView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        mapView *_t = static_cast<mapView *>(_o);
        switch (_id) {
        case 0: _t->mapLoaded(); break;
        case 1: _t->mapBecameIdle(); break;
        case 2: _t->regionChanged((*reinterpret_cast< mapCoordinateRegion(*)>(_a[1]))); break;
        case 3: _t->centerChanged((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 4: _t->mouseClicked((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 5: _t->mouseDoubleClicked((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 6: _t->mouseRightClicked((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 7: _t->mouseDragged(); break;
        case 8: _t->mouseDragStarted(); break;
        case 9: _t->mouseDragEnded(); break;
        case 10: _t->cursorMoved((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 11: _t->cursorEntered((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 12: _t->cursorLeaved((*reinterpret_cast< mapCoordinate(*)>(_a[1]))); break;
        case 13: _t->headingChanged(); break;
        case 14: _t->mapTypeChanged((*reinterpret_cast< MapType(*)>(_a[1]))); break;
        case 15: _t->tilesLoaded(); break;
        case 16: _t->tilesChanged(); break;
        case 17: _t->zoomLevelChanged((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 18: _t->initializeMap(); break;
        case 19: _t->insertNativeObject(); break;
        case 20: _t->regionDidChangeTo((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4]))); break;
        case 21: _t->centerDidChangeTo((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 22: _t->mapTypeDidChangeTo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 23: _t->mouseDidClickAt((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 24: _t->mouseDidDoubleClickAt((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 25: _t->mouseDidRightClickAt((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 26: _t->cursorDidMoveTo((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 27: _t->cursorDidEnterTo((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 28: _t->cursorDidLeaveFrom((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData mapView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject mapView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_mapView,
      qt_meta_data_mapView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &mapView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *mapView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *mapView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mapView))
        return static_cast<void*>(const_cast< mapView*>(this));
    return QWidget::qt_metacast(_clname);
}

int mapView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void mapView::mapLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void mapView::mapBecameIdle()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void mapView::regionChanged(mapCoordinateRegion _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void mapView::centerChanged(mapCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void mapView::mouseClicked(mapCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void mapView::mouseDoubleClicked(mapCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void mapView::mouseRightClicked(mapCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void mapView::mouseDragged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void mapView::mouseDragStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void mapView::mouseDragEnded()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void mapView::cursorMoved(mapCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void mapView::cursorEntered(mapCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void mapView::cursorLeaved(mapCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void mapView::headingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void mapView::mapTypeChanged(MapType _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void mapView::tilesLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void mapView::tilesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void mapView::zoomLevelChanged(uint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}
QT_END_MOC_NAMESPACE
