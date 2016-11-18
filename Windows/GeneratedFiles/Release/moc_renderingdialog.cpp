/****************************************************************************
** Meta object code from reading C++ file 'renderingdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/renderingdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'renderingdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RenderingDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   17,   16,   16, 0x05,
      48,   16,   16,   16, 0x05,
      62,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      82,   76,   16,   16, 0x08,
     128,  107,   16,   16, 0x0a,
     172,  158,   16,   16, 0x0a,
     210,  194,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RenderingDialog[] = {
    "RenderingDialog\0\0mode\0renderingModeChanged(int)\0"
    "updateImage()\0resetRemote()\0index\0"
    "renderingModeUpdate(int)\0currRendering,remote\0"
    "updateRenderingList(int,bool)\0"
    "currRendering\0setRenderingMode(int)\0"
    "l,currRendering\0"
    "updateRTIRendDlg(QMap<int,RenderingMode*>*,int)\0"
};

void RenderingDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RenderingDialog *_t = static_cast<RenderingDialog *>(_o);
        switch (_id) {
        case 0: _t->renderingModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateImage(); break;
        case 2: _t->resetRemote(); break;
        case 3: _t->renderingModeUpdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->updateRenderingList((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->setRenderingMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->updateRTIRendDlg((*reinterpret_cast< QMap<int,RenderingMode*>*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RenderingDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RenderingDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RenderingDialog,
      qt_meta_data_RenderingDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RenderingDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RenderingDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RenderingDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RenderingDialog))
        return static_cast<void*>(const_cast< RenderingDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int RenderingDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void RenderingDialog::renderingModeChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RenderingDialog::updateImage()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void RenderingDialog::resetRemote()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
