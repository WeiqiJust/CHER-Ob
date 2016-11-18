/****************************************************************************
** Meta object code from reading C++ file 'reportFilter.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/function/reportFilter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'reportFilter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ReportFilter[] = {

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
      14,   13,   13,   13, 0x08,
      25,   13,   13,   13, 0x08,
      34,   13,   13,   13, 0x08,
      58,   46,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ReportFilter[] = {
    "ReportFilter\0\0generate()\0cancel()\0"
    "selectAll()\0item,column\0"
    "itemChanged(QTreeWidgetItem*,int)\0"
};

void ReportFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ReportFilter *_t = static_cast<ReportFilter *>(_o);
        switch (_id) {
        case 0: _t->generate(); break;
        case 1: _t->cancel(); break;
        case 2: _t->selectAll(); break;
        case 3: _t->itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ReportFilter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ReportFilter::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ReportFilter,
      qt_meta_data_ReportFilter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ReportFilter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ReportFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ReportFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ReportFilter))
        return static_cast<void*>(const_cast< ReportFilter*>(this));
    return QWidget::qt_metacast(_clname);
}

int ReportFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
