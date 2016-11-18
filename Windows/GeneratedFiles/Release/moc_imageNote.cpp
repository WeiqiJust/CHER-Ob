/****************************************************************************
** Meta object code from reading C++ file 'imageNote.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/information/imageNote.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'imageNote.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ImageNote[] = {

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
      14,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ImageNote[] = {
    "ImageNote\0\0id\0removeImage(int)\0remove()\0"
};

void ImageNote::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ImageNote *_t = static_cast<ImageNote *>(_o);
        switch (_id) {
        case 0: _t->removeImage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->remove(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ImageNote::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImageNote::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ImageNote,
      qt_meta_data_ImageNote, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImageNote::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImageNote::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImageNote::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageNote))
        return static_cast<void*>(const_cast< ImageNote*>(this));
    return QWidget::qt_metacast(_clname);
}

int ImageNote::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ImageNote::removeImage(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
