#ifndef VTKINTERACTORSTYLESURFACEWALKERCAMERA_H
#define VTKINTERACTORSTYLESURFACEWALKERCAMERA_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <QDebug>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>
#include <vtkCellLocator.h>
#include <vtkKdTreePointLocator.h>
#include <vtkCamera.h>

class vtkMatrix4x4;
class vtkPolyData;
class VtkWidget;
class LSCM;

/**
 * This class is to designed to provide surface walker camera mode. It is buggy and not being used!
 */

class vtkInteractorStyleSurfaceWalkerCamera : public vtkInteractorStyleTrackballCamera
{
public:
    static vtkInteractorStyleSurfaceWalkerCamera* New();
    vtkInteractorStyleSurfaceWalkerCamera();
    vtkTypeRevisionMacro(vtkInteractorStyleSurfaceWalkerCamera,vtkInteractorStyleTrackballCamera);

    void linkPolyData(vtkPolyData *polyData, vtkCamera *camera);
    void linkVtkWidget(VtkWidget *_vtkWidget);

    void flattenSubset();
    vtkPolyData *extractSubset(double R);
    void alignFlatMeshWith3D();

    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    virtual void OnMouseMove();
    virtual void surfacePan();

private:
    double mCamPos[3];
    double mViewTarget[3];
    double mViewTargetFlat[3];
    double mCurrentGravity[3];
    double mCurrentUpVector[3];
    double mCurrentRightVector[3];
    double mCamDistance;
    vtkIdType targetFaceID;
    vtkIdType localTargetFaceID;
    vtkSmartPointer<vtkPolyData> mPolyData;
    vtkSmartPointer<vtkPolyData> mPolyDataFlat;

    void stickTargetToSurface(vtkCamera *camera, double rayRadius);

    bool getPointNormals();
    void testPointNormals();
    vtkSmartPointer<vtkPolyDataNormals> normalGenerator;
    vtkSmartPointer<vtkCellLocator> cellLocator;
    vtkSmartPointer<vtkKdTreePointLocator> kdTree;
    vtkSmartPointer<vtkIdTypeArray> originalCellIDs;

    LSCM *lscm_engine;

    VtkWidget *mVTKWidget;

    bool renderFlat;

    double mExtractRadius;
};

#endif // VTKINTERACTORSTYLESURFACEWALKERCAMERA_H
