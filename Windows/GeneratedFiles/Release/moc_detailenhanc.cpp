/****************************************************************************
** Meta object code from reading C++ file 'detailenhanc.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/detailenhanc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'detailenhanc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DetailConfDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      49,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     158,   14,   14,   14, 0x0a,
     172,  170,   14,   14, 0x08,
     190,  170,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DetailConfDlg[] = {
    "DetailConfDlg\0\0o,size,level,m,ss,v1,v2,t,f,nIter\0"
    "configUpdated(OffsetNum,TileSize,int,SharpnessMeasures,SphereSampling,"
    "float,float,float,SmoothingFilter,int)\0"
    "okPressed()\0k\0k1Changed(double)\0"
    "k2Changed(double)\0"
};

void DetailConfDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DetailConfDlg *_t = static_cast<DetailConfDlg *>(_o);
        switch (_id) {
        case 0: _t->configUpdated((*reinterpret_cast< OffsetNum(*)>(_a[1])),(*reinterpret_cast< TileSize(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< SharpnessMeasures(*)>(_a[4])),(*reinterpret_cast< SphereSampling(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< float(*)>(_a[7])),(*reinterpret_cast< float(*)>(_a[8])),(*reinterpret_cast< SmoothingFilter(*)>(_a[9])),(*reinterpret_cast< int(*)>(_a[10]))); break;
        case 1: _t->okPressed(); break;
        case 2: _t->k1Changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->k2Changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DetailConfDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DetailConfDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DetailConfDlg,
      qt_meta_data_DetailConfDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DetailConfDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DetailConfDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DetailConfDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DetailConfDlg))
        return static_cast<void*>(const_cast< DetailConfDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int DetailConfDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DetailConfDlg::configUpdated(OffsetNum _t1, TileSize _t2, int _t3, SharpnessMeasures _t4, SphereSampling _t5, float _t6, float _t7, float _t8, SmoothingFilter _t9, int _t10)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)), const_cast<void*>(reinterpret_cast<const void*>(&_t9)), const_cast<void*>(reinterpret_cast<const void*>(&_t10)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_DetailEnhDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DetailEnhDlg[] = {
    "DetailEnhDlg\0\0showAdvancedSettings()\0"
};

void DetailEnhDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DetailEnhDlg *_t = static_cast<DetailEnhDlg *>(_o);
        switch (_id) {
        case 0: _t->showAdvancedSettings(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DetailEnhDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DetailEnhDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DetailEnhDlg,
      qt_meta_data_DetailEnhDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DetailEnhDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DetailEnhDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DetailEnhDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DetailEnhDlg))
        return static_cast<void*>(const_cast< DetailEnhDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int DetailEnhDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_DetailEnhancement[] = {

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
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      68,   34,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DetailEnhancement[] = {
    "DetailEnhancement\0\0refreshImage()\0"
    "o,size,level,m,ss,v1,v2,t,f,nIter\0"
    "updateConfig(OffsetNum,TileSize,int,SharpnessMeasures,SphereSampling,f"
    "loat,float,float,SmoothingFilter,int)\0"
};

void DetailEnhancement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DetailEnhancement *_t = static_cast<DetailEnhancement *>(_o);
        switch (_id) {
        case 0: _t->refreshImage(); break;
        case 1: _t->updateConfig((*reinterpret_cast< OffsetNum(*)>(_a[1])),(*reinterpret_cast< TileSize(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< SharpnessMeasures(*)>(_a[4])),(*reinterpret_cast< SphereSampling(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< float(*)>(_a[7])),(*reinterpret_cast< float(*)>(_a[8])),(*reinterpret_cast< SmoothingFilter(*)>(_a[9])),(*reinterpret_cast< int(*)>(_a[10]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DetailEnhancement::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DetailEnhancement::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DetailEnhancement,
      qt_meta_data_DetailEnhancement, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DetailEnhancement::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DetailEnhancement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DetailEnhancement::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DetailEnhancement))
        return static_cast<void*>(const_cast< DetailEnhancement*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< DetailEnhancement*>(this));
    return QObject::qt_metacast(_clname);
}

int DetailEnhancement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void DetailEnhancement::refreshImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
