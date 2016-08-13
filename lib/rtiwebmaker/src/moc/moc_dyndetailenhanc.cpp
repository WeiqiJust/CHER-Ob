/****************************************************************************
** Meta object code from reading C++ file 'dyndetailenhanc.h'
**
** Created: Tue Dec 22 11:15:09 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../rtiviewer/src/dyndetailenhanc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dyndetailenhanc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DynamicDetConfDlg[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      78,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     173,   18,   18,   18, 0x0a,
     194,  188,   18,   18, 0x08,
     212,  188,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DynamicDetConfDlg[] = {
    "DynamicDetConfDlg\0\0"
    "sharpnessOp,sphereSampl,k1,k2,threshold,smoothFilter,nIter\0"
    "configUpdated(SharpnessMeasuresDyn,SphereSamplingDyn,float,float,float"
    ",SmoothingFilterDyn,int)\0"
    "applyPressed()\0value\0k1Changed(double)\0"
    "k2Changed(double)\0"
};

const QMetaObject DynamicDetConfDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DynamicDetConfDlg,
      qt_meta_data_DynamicDetConfDlg, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DynamicDetConfDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DynamicDetConfDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DynamicDetConfDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DynamicDetConfDlg))
        return static_cast<void*>(const_cast< DynamicDetConfDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int DynamicDetConfDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: configUpdated((*reinterpret_cast< SharpnessMeasuresDyn(*)>(_a[1])),(*reinterpret_cast< SphereSamplingDyn(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< SmoothingFilterDyn(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 1: applyPressed(); break;
        case 2: k1Changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: k2Changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DynamicDetConfDlg::configUpdated(SharpnessMeasuresDyn _t1, SphereSamplingDyn _t2, float _t3, float _t4, float _t5, SmoothingFilterDyn _t6, int _t7)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_DynamicDetailEControl[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   23,   22,   22, 0x05,
      56,   49,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   75,   22,   22, 0x0a,
      96,   94,   22,   22, 0x0a,
     111,   75,   22,   22, 0x08,
     131,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DynamicDetailEControl[] = {
    "DynamicDetailEControl\0\0size\0"
    "tileSizeChanged(int)\0offset\0"
    "offsetChanged(int)\0s\0setTileSize(int)\0"
    "o\0setOffset(int)\0changeTileSize(int)\0"
    "showAdvancedSettings()\0"
};

const QMetaObject DynamicDetailEControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DynamicDetailEControl,
      qt_meta_data_DynamicDetailEControl, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DynamicDetailEControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DynamicDetailEControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DynamicDetailEControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DynamicDetailEControl))
        return static_cast<void*>(const_cast< DynamicDetailEControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int DynamicDetailEControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: tileSizeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: offsetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setTileSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setOffset((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: changeTileSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: showAdvancedSettings(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void DynamicDetailEControl::tileSizeChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DynamicDetailEControl::offsetChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_DynamicDetailEnh[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      35,   33,   17,   17, 0x05,
      58,   56,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   33,   17,   17, 0x0a,
      96,   94,   17,   17, 0x0a,
     132,  111,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DynamicDetailEnh[] = {
    "DynamicDetailEnh\0\0refreshImage()\0s\0"
    "tileSizeChanged(int)\0o\0offsetChanged(int)\0"
    "setTileSize(int)\0x\0setOffset(int)\0"
    "m,ss,v1,v2,t,f,nIter\0"
    "updateConfig(SharpnessMeasuresDyn,SphereSamplingDyn,float,float,float,"
    "SmoothingFilterDyn,int)\0"
};

const QMetaObject DynamicDetailEnh::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DynamicDetailEnh,
      qt_meta_data_DynamicDetailEnh, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DynamicDetailEnh::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DynamicDetailEnh::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DynamicDetailEnh::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DynamicDetailEnh))
        return static_cast<void*>(const_cast< DynamicDetailEnh*>(this));
    if (!strcmp(_clname, "RenderingMode"))
        return static_cast< RenderingMode*>(const_cast< DynamicDetailEnh*>(this));
    return QObject::qt_metacast(_clname);
}

int DynamicDetailEnh::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refreshImage(); break;
        case 1: tileSizeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: offsetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setTileSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: setOffset((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: updateConfig((*reinterpret_cast< SharpnessMeasuresDyn(*)>(_a[1])),(*reinterpret_cast< SphereSamplingDyn(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< SmoothingFilterDyn(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void DynamicDetailEnh::refreshImage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void DynamicDetailEnh::tileSizeChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DynamicDetailEnh::offsetChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
