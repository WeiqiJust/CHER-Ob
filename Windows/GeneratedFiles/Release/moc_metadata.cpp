/****************************************************************************
** Meta object code from reading C++ file 'metadata.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/information/metadata.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'metadata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DMetadataTreeWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_DMetadataTreeWidget[] = {
    "DMetadataTreeWidget\0"
};

void DMetadataTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DMetadataTreeWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DMetadataTreeWidget::staticMetaObject = {
    { &BookmarkTreeWidget::staticMetaObject, qt_meta_stringdata_DMetadataTreeWidget,
      qt_meta_data_DMetadataTreeWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DMetadataTreeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DMetadataTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DMetadataTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DMetadataTreeWidget))
        return static_cast<void*>(const_cast< DMetadataTreeWidget*>(this));
    return BookmarkTreeWidget::qt_metacast(_clname);
}

int DMetadataTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BookmarkTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Metadata[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      26,    9,    9,    9, 0x08,
      40,    9,    9,    9, 0x08,
      60,   53,    9,    9, 0x08,
      91,    9,    9,    9, 0x08,
     115,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Metadata[] = {
    "Metadata\0\0updateButtons()\0addCategory()\0"
    "deleteItem()\0item,i\0loadText(QTreeWidgetItem*,int)\0"
    "showContextMenu(QPoint)\0toggleEdit()\0"
};

void Metadata::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Metadata *_t = static_cast<Metadata *>(_o);
        switch (_id) {
        case 0: _t->updateButtons(); break;
        case 1: _t->addCategory(); break;
        case 2: _t->deleteItem(); break;
        case 3: _t->loadText((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 5: _t->toggleEdit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Metadata::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Metadata::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Metadata,
      qt_meta_data_Metadata, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Metadata::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Metadata::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Metadata::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Metadata))
        return static_cast<void*>(const_cast< Metadata*>(this));
    return QDialog::qt_metacast(_clname);
}

int Metadata::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
