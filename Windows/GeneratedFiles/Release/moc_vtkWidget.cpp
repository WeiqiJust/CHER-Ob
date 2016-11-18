/****************************************************************************
** Meta object code from reading C++ file 'vtkWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/visualization/vtkWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vtkWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VtkWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      50,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   11,   10,   10, 0x05,
      47,   10,   10,   10, 0x05,
      76,   67,   10,   10, 0x05,
     117,  104,   10,   10, 0x05,
     166,  149,   10,   10, 0x05,
     228,  212,   10,   10, 0x05,
     292,  278,   10,   10, 0x05,
     359,  343,   10,   10, 0x05,
     407,  396,   10,   10, 0x05,
     444,   10,   10,   10, 0x05,
     468,   10,   10,   10, 0x05,
     528,  490,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     623,   10,   10,   10, 0x0a,
     650,  644,   10,   10, 0x0a,
     678,  674,   10,   10, 0x0a,
     739,  718,   10,   10, 0x0a,
     783,   10,   10,   10, 0x0a,
     812,  343,   10,   10, 0x0a,
     858,  848,   10,   10, 0x0a,
     886,  278,   10,   10, 0x0a,
     946,  936,   10,   10, 0x0a,
     997,  981,   10,   10, 0x0a,
    1027, 1020,   10,   10, 0x0a,
    1084, 1052,   10,   10, 0x0a,
    1178, 1163,   10,   10, 0x0a,
    1258, 1020,   10,   10, 0x0a,
    1281, 1052,   10,   10, 0x0a,
    1358, 1163,   10,   10, 0x0a,
    1468, 1436,   10,   10, 0x0a,
    1542, 1520,   10,   10, 0x2a,
    1620, 1589,   10,   10, 0x0a,
    1728, 1707,   10,   10, 0x2a,
    1841, 1810,   10,   10, 0x0a,
    1955, 1934,   10,   10, 0x2a,
    2049, 2043,   10,   10, 0x0a,
    2082, 2043,   10,   10, 0x0a,
    2135, 2113,   10,   10, 0x0a,
    2193, 2181,   10,   10, 0x2a,
    2234, 2043,   10,   10, 0x0a,
    2265, 2043,   10,   10, 0x0a,
    2294, 2113,   10,   10, 0x0a,
    2338, 2181,   10,   10, 0x2a,
    2385, 2377,   10,   10, 0x0a,
    2450, 2377,   10,   10, 0x0a,
    2537, 2513,   10,   10, 0x0a,
    2629, 2615,   10,   10, 0x2a,
    2724, 2702,   10,   10, 0x09,
    2762, 2758,   10,   10, 0x09,
    2822, 2814,   10,   10, 0x09,
    2904, 2845,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VtkWidget[] = {
    "VtkWidget\0\0currentId\0currentViewerChanged(int)\0"
    "resetLightControl()\0sliceMax\0"
    "currentSliceMaxChanged(int)\0sliceCurrent\0"
    "currentSliceCurrentChanged(int)\0"
    "sliceOrientation\0"
    "currentSliceOrientationChanged(CTOrientation)\0"
    "ctVisualization\0"
    "currentSliceVisualizationChanged(CTVisualization)\0"
    "volrendermode\0"
    "currentVolumeRenderModeChanged(CTVolumeRenderMode)\0"
    "reductionFactor\0currentReductionFactorChanged(float)\0"
    "widgetmode\0currentWidgetModeChanged(WidgetMode)\0"
    "updateMainWindowMenus()\0currentImageChanged()\0"
    "wavelengths,hyperPixels,icoords,fname\0"
    "currentHyperPixelsChanged(std::vector<float>,std::vector<float>,const "
    "int*,const std::string*)\0"
    "updateDisplayPanel()\0index\0"
    "updateCurrentSlice(int)\0ort\0"
    "updateCurrentOrientation(CTOrientation)\0"
    "currentVisualization\0"
    "updateCurrentVisualization(CTVisualization)\0"
    "rotateCameraViewUpsideDown()\0"
    "updateCurrentReductionFactor(float)\0"
    "blendType\0updateCurrentBlendType(int)\0"
    "updateCurrentVolumeRenderMode(CTVolumeRenderMode)\0"
    "transform\0updateLightPosition(vtkTransform*)\0"
    "isInterpolateOn\0setInterpolateOn(bool)\0"
    "cellId\0removePointNoteMark(int)\0"
    "cellIds,cornerPoints,isCTVolume\0"
    "removeSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode>,QVector<double"
    "*>,bool)\0"
    "points,normals\0"
    "removeFrustumNoteMark(vtkSmartPointer<vtkPoints>,vtkSmartPointer<vtkDa"
    "taArray>)\0"
    "openPointNoteMark(int)\0"
    "openSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode>,QVector<double*>"
    ",bool)\0"
    "openFrustumNoteMark(vtkSmartPointer<vtkPoints>,vtkSmartPointer<vtkData"
    "Array>)\0"
    "cellId,color,position,isDisplay\0"
    "loadPointNoteMark(int,ColorType,const double*,bool)\0"
    "cellId,color,position\0"
    "loadPointNoteMark(int,ColorType,const double*)\0"
    "cellIds,points,color,isDisplay\0"
    "loadSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode>,QVector<double*>"
    ",ColorType,bool)\0"
    "cellIds,points,color\0"
    "loadSurfaceNoteMark(vtkSmartPointer<vtkSelectionNode>,QVector<double*>"
    ",ColorType)\0"
    "points,normals,color,isDisplay\0"
    "loadFrustumNoteMark(vtkSmartPointer<vtkPoints>,vtkSmartPointer<vtkData"
    "Array>,ColorType,bool)\0"
    "points,normals,color\0"
    "loadFrustumNoteMark(vtkSmartPointer<vtkPoints>,vtkSmartPointer<vtkData"
    "Array>,ColorType)\0"
    "point\0removeSurfaceNote2DMark(double*)\0"
    "openSurfaceNote2DMark(double*)\0"
    "point,color,isDisplay\0"
    "loadSurfaceNote2DMark(double*,ColorType,bool)\0"
    "point,color\0loadSurfaceNote2DMark(double*,ColorType)\0"
    "removePointNote2DMark(double*)\0"
    "openPointNote2DMark(double*)\0"
    "loadPointNote2DMark(double*,ColorType,bool)\0"
    "loadPointNote2DMark(double*,ColorType)\0"
    "polygon\0"
    "removePolygonNote2DMark(std::vector<std::pair<double,double> >*)\0"
    "openPolygonNote2DMark(std::vector<std::pair<double,double> >*)\0"
    "polygon,color,isDisplay\0"
    "loadPolygonNote2DMark(std::vector<std::pair<double,double> >*,ColorTyp"
    "e,bool)\0"
    "polygon,color\0"
    "loadPolygonNote2DMark(std::vector<std::pair<double,double> >*,ColorTyp"
    "e)\0"
    "intensity1,intensity2\0"
    "updateIntensityL12(double,double)\0,,,\0"
    "getHyperPixelsSignals(vtkObject*,ulong,void*,void*)\0"
    "editBox\0saveFileInfo(QWidget*)\0"
    "textureData,textureWidth,textureHeight,FIRST_RTI_RENDERING\0"
    "updateRTIImageVTK(std::vector<unsigned char>,int,int,bool)\0"
};

void VtkWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VtkWidget *_t = static_cast<VtkWidget *>(_o);
        switch (_id) {
        case 0: _t->currentViewerChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->resetLightControl(); break;
        case 2: _t->currentSliceMaxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->currentSliceCurrentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->currentSliceOrientationChanged((*reinterpret_cast< CTOrientation(*)>(_a[1]))); break;
        case 5: _t->currentSliceVisualizationChanged((*reinterpret_cast< CTVisualization(*)>(_a[1]))); break;
        case 6: _t->currentVolumeRenderModeChanged((*reinterpret_cast< CTVolumeRenderMode(*)>(_a[1]))); break;
        case 7: _t->currentReductionFactorChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 8: _t->currentWidgetModeChanged((*reinterpret_cast< WidgetMode(*)>(_a[1]))); break;
        case 9: _t->updateMainWindowMenus(); break;
        case 10: _t->currentImageChanged(); break;
        case 11: _t->currentHyperPixelsChanged((*reinterpret_cast< std::vector<float>(*)>(_a[1])),(*reinterpret_cast< std::vector<float>(*)>(_a[2])),(*reinterpret_cast< const int*(*)>(_a[3])),(*reinterpret_cast< const std::string*(*)>(_a[4]))); break;
        case 12: _t->updateDisplayPanel(); break;
        case 13: _t->updateCurrentSlice((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->updateCurrentOrientation((*reinterpret_cast< CTOrientation(*)>(_a[1]))); break;
        case 15: _t->updateCurrentVisualization((*reinterpret_cast< CTVisualization(*)>(_a[1]))); break;
        case 16: _t->rotateCameraViewUpsideDown(); break;
        case 17: _t->updateCurrentReductionFactor((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 18: _t->updateCurrentBlendType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->updateCurrentVolumeRenderMode((*reinterpret_cast< CTVolumeRenderMode(*)>(_a[1]))); break;
        case 20: _t->updateLightPosition((*reinterpret_cast< vtkTransform*(*)>(_a[1]))); break;
        case 21: _t->setInterpolateOn((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->removePointNoteMark((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 23: _t->removeSurfaceNoteMark((*reinterpret_cast< vtkSmartPointer<vtkSelectionNode>(*)>(_a[1])),(*reinterpret_cast< QVector<double*>(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 24: _t->removeFrustumNoteMark((*reinterpret_cast< vtkSmartPointer<vtkPoints>(*)>(_a[1])),(*reinterpret_cast< vtkSmartPointer<vtkDataArray>(*)>(_a[2]))); break;
        case 25: _t->openPointNoteMark((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 26: _t->openSurfaceNoteMark((*reinterpret_cast< vtkSmartPointer<vtkSelectionNode>(*)>(_a[1])),(*reinterpret_cast< QVector<double*>(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 27: _t->openFrustumNoteMark((*reinterpret_cast< vtkSmartPointer<vtkPoints>(*)>(_a[1])),(*reinterpret_cast< vtkSmartPointer<vtkDataArray>(*)>(_a[2]))); break;
        case 28: _t->loadPointNoteMark((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2])),(*reinterpret_cast< const double*(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 29: _t->loadPointNoteMark((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2])),(*reinterpret_cast< const double*(*)>(_a[3]))); break;
        case 30: _t->loadSurfaceNoteMark((*reinterpret_cast< vtkSmartPointer<vtkSelectionNode>(*)>(_a[1])),(*reinterpret_cast< QVector<double*>(*)>(_a[2])),(*reinterpret_cast< const ColorType(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 31: _t->loadSurfaceNoteMark((*reinterpret_cast< vtkSmartPointer<vtkSelectionNode>(*)>(_a[1])),(*reinterpret_cast< QVector<double*>(*)>(_a[2])),(*reinterpret_cast< const ColorType(*)>(_a[3]))); break;
        case 32: _t->loadFrustumNoteMark((*reinterpret_cast< vtkSmartPointer<vtkPoints>(*)>(_a[1])),(*reinterpret_cast< vtkSmartPointer<vtkDataArray>(*)>(_a[2])),(*reinterpret_cast< const ColorType(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 33: _t->loadFrustumNoteMark((*reinterpret_cast< vtkSmartPointer<vtkPoints>(*)>(_a[1])),(*reinterpret_cast< vtkSmartPointer<vtkDataArray>(*)>(_a[2])),(*reinterpret_cast< const ColorType(*)>(_a[3]))); break;
        case 34: _t->removeSurfaceNote2DMark((*reinterpret_cast< double*(*)>(_a[1]))); break;
        case 35: _t->openSurfaceNote2DMark((*reinterpret_cast< double*(*)>(_a[1]))); break;
        case 36: _t->loadSurfaceNote2DMark((*reinterpret_cast< double*(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 37: _t->loadSurfaceNote2DMark((*reinterpret_cast< double*(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2]))); break;
        case 38: _t->removePointNote2DMark((*reinterpret_cast< double*(*)>(_a[1]))); break;
        case 39: _t->openPointNote2DMark((*reinterpret_cast< double*(*)>(_a[1]))); break;
        case 40: _t->loadPointNote2DMark((*reinterpret_cast< double*(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 41: _t->loadPointNote2DMark((*reinterpret_cast< double*(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2]))); break;
        case 42: _t->removePolygonNote2DMark((*reinterpret_cast< std::vector<std::pair<double,double> >*(*)>(_a[1]))); break;
        case 43: _t->openPolygonNote2DMark((*reinterpret_cast< std::vector<std::pair<double,double> >*(*)>(_a[1]))); break;
        case 44: _t->loadPolygonNote2DMark((*reinterpret_cast< std::vector<std::pair<double,double> >*(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 45: _t->loadPolygonNote2DMark((*reinterpret_cast< std::vector<std::pair<double,double> >*(*)>(_a[1])),(*reinterpret_cast< const ColorType(*)>(_a[2]))); break;
        case 46: _t->updateIntensityL12((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 47: _t->getHyperPixelsSignals((*reinterpret_cast< vtkObject*(*)>(_a[1])),(*reinterpret_cast< ulong(*)>(_a[2])),(*reinterpret_cast< void*(*)>(_a[3])),(*reinterpret_cast< void*(*)>(_a[4]))); break;
        case 48: _t->saveFileInfo((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 49: _t->updateRTIImageVTK((*reinterpret_cast< std::vector<unsigned char>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VtkWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VtkWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VtkWidget,
      qt_meta_data_VtkWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VtkWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VtkWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VtkWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VtkWidget))
        return static_cast<void*>(const_cast< VtkWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int VtkWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void VtkWidget::currentViewerChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VtkWidget::resetLightControl()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void VtkWidget::currentSliceMaxChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VtkWidget::currentSliceCurrentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void VtkWidget::currentSliceOrientationChanged(CTOrientation _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void VtkWidget::currentSliceVisualizationChanged(CTVisualization _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void VtkWidget::currentVolumeRenderModeChanged(CTVolumeRenderMode _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void VtkWidget::currentReductionFactorChanged(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void VtkWidget::currentWidgetModeChanged(WidgetMode _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void VtkWidget::updateMainWindowMenus()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void VtkWidget::currentImageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void VtkWidget::currentHyperPixelsChanged(std::vector<float> _t1, std::vector<float> _t2, const int * _t3, const std::string * _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_END_MOC_NAMESPACE
