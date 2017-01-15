/****************************************************************************
** Meta object code from reading C++ file 'videoFilter.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/videoFilter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videoFilter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VideoFilter[] = {

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
      13,   12,   12,   12, 0x08,
      24,   12,   12,   12, 0x08,
      33,   12,   12,   12, 0x08,
      57,   45,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VideoFilter[] = {
    "VideoFilter\0\0generate()\0cancel()\0"
    "selectAll()\0item,column\0"
    "itemChanged(QTreeWidgetItem*,int)\0"
};

void VideoFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VideoFilter *_t = static_cast<VideoFilter *>(_o);
        switch (_id) {
        case 0: _t->generate(); break;
        case 1: _t->cancel(); break;
        case 2: _t->selectAll(); break;
        case 3: _t->itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VideoFilter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VideoFilter::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VideoFilter,
      qt_meta_data_VideoFilter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VideoFilter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VideoFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VideoFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VideoFilter))
        return static_cast<void*>(const_cast< VideoFilter*>(this));
    return QWidget::qt_metacast(_clname);
}

int VideoFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
