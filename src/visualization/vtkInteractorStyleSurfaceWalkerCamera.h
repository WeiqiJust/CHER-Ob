/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)

 - License:  GNU General Public License Usage
   Alternatively, this file may be used under the terms of the GNU General
   Public License version 3.0 as published by the Free Software Foundation
   and appearing in the file LICENSE.GPL included in the packaging of this
   file. Please review the following information to ensure the GNU General
   Public License version 3.0 requirements will be met:
   http://www.gnu.org/copyleft/gpl.html.

 - Warranty: This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.

 - Acknowledgments: This EXR file reader is based on the original code of
   exrreadchannel for MATLAB by Edgar Velazquez-Armendariz.

*****************************************************************************/
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
 * This class is to designed to provide surface walker camera mode. It is not being used!
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
