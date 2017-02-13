/****************************************************************************
** Meta object code from reading C++ file 'QMMapView.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../QMMapView.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMMapView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMMapView[] = {

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
      11,   10,   10,   10, 0x05,
      23,   10,   10,   10, 0x05,
      46,   39,   10,   10, 0x05,
      87,   80,   10,   10, 0x05,
     126,  115,   10,   10, 0x05,
     153,  115,   10,   10, 0x05,
     186,  115,   10,   10, 0x05,
     218,   10,   10,   10, 0x05,
     233,   10,   10,   10, 0x05,
     252,   10,   10,   10, 0x05,
     269,  115,   10,   10, 0x05,
     295,  115,   10,   10, 0x05,
     323,  115,   10,   10, 0x05,
     350,   10,   10,   10, 0x05,
     372,  367,   10,   10, 0x05,
     396,   10,   10,   10, 0x05,
     410,   10,   10,   10, 0x05,
     431,  425,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     454,   10,   10,   10, 0x09,
     470,   10,   10,   10, 0x09,
     513,  491,   10,   10, 0x09,
     575,  556,   10,   10, 0x09,
     617,  606,   10,   10, 0x09,
     645,  556,   10,   10, 0x09,
     674,  556,   10,   10, 0x09,
     709,  556,   10,   10, 0x09,
     743,  556,   10,   10, 0x09,
     772,  556,   10,   10, 0x09,
     802,  556,   10,   10, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_QMMapView[] = {
    "QMMapView\0\0mapLoaded()\0mapBecameIdle()\0"
    "region\0regionChanged(QMCoordinateRegion)\0"
    "center\0centerChanged(QMCoordinate)\0"
    "coordinate\0mouseClicked(QMCoordinate)\0"
    "mouseDoubleClicked(QMCoordinate)\0"
    "mouseRightClicked(QMCoordinate)\0"
    "mouseDragged()\0mouseDragStarted()\0"
    "mouseDragEnded()\0cursorMoved(QMCoordinate)\0"
    "cursorEntered(QMCoordinate)\0"
    "cursorLeaved(QMCoordinate)\0headingChanged()\0"
    "type\0mapTypeChanged(MapType)\0tilesLoaded()\0"
    "tilesChanged()\0level\0zoomLevelChanged(uint)\0"
    "initializeMap()\0insertNativeObject()\0"
    "north,south,east,west\0"
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

void QMMapView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMMapView *_t = static_cast<QMMapView *>(_o);
        switch (_id) {
        case 0: _t->mapLoaded(); break;
        case 1: _t->mapBecameIdle(); break;
        case 2: _t->regionChanged((*reinterpret_cast< QMCoordinateRegion(*)>(_a[1]))); break;
        case 3: _t->centerChanged((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
        case 4: _t->mouseClicked((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
        case 5: _t->mouseDoubleClicked((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
        case 6: _t->mouseRightClicked((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
        case 7: _t->mouseDragged(); break;
        case 8: _t->mouseDragStarted(); break;
        case 9: _t->mouseDragEnded(); break;
        case 10: _t->cursorMoved((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
        case 11: _t->cursorEntered((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
        case 12: _t->cursorLeaved((*reinterpret_cast< QMCoordinate(*)>(_a[1]))); break;
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

const QMetaObjectExtraData QMMapView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMMapView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMMapView,
      qt_meta_data_QMMapView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMMapView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMMapView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMMapView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMMapView))
        return static_cast<void*>(const_cast< QMMapView*>(this));
    return QWidget::qt_metacast(_clname);
}

int QMMapView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void QMMapView::mapLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QMMapView::mapBecameIdle()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QMMapView::regionChanged(QMCoordinateRegion _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QMMapView::centerChanged(QMCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QMMapView::mouseClicked(QMCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QMMapView::mouseDoubleClicked(QMCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QMMapView::mouseRightClicked(QMCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QMMapView::mouseDragged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void QMMapView::mouseDragStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void QMMapView::mouseDragEnded()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void QMMapView::cursorMoved(QMCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void QMMapView::cursorEntered(QMCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void QMMapView::cursorLeaved(QMCoordinate _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void QMMapView::headingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void QMMapView::mapTypeChanged(MapType _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void QMMapView::tilesLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void QMMapView::tilesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void QMMapView::zoomLevelChanged(uint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}
QT_END_MOC_NAMESPACE
