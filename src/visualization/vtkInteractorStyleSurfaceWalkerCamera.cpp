#include "vtkInteractorStyleSurfaceWalkerCamera.h"
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkCellPicker.h>
#include <vtkCellLocator.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPolyDataNormals.h>
#include <vtkIdList.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <vtkKdTreePointLocator.h>
#include <vtkExtractCells.h>
#include <vtkUnstructuredGrid.h>
#include <vtkGeometryFilter.h>
#include "lscm_engine.h"
#include "vtkWidget.h"
#include <vtkMatrix4x4.h>
#include <vtkIdFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkViewport.h>
#include <vtkPolyDataConnectivityFilter.h>

vtkCxxRevisionMacro(vtkInteractorStyleSurfaceWalkerCamera, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkInteractorStyleSurfaceWalkerCamera);

vtkInteractorStyleSurfaceWalkerCamera::vtkInteractorStyleSurfaceWalkerCamera()
{
    lscm_engine = new LSCM();
    renderFlat = false;
    mExtractRadius = 0.01;
    mCamDistance = 1.0;
}

void vtkInteractorStyleSurfaceWalkerCamera::OnLeftButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    if (this->Interactor->GetShiftKey())
    {
        this->StartSpin();
    }
    else
    {
        this->StartPan();
    }
}

void vtkInteractorStyleSurfaceWalkerCamera::OnLeftButtonUp()
{
  switch (this->State)
    {
    case VTKIS_DOLLY:
      this->EndDolly();
      break;

    case VTKIS_PAN:
      this->EndPan();
      break;

    case VTKIS_SPIN:
      this->EndSpin();
      break;

    case VTKIS_ROTATE:
      this->EndRotate();
      break;
    }

  if ( this->Interactor )
    {
    this->ReleaseFocus();
    }
}

void vtkInteractorStyleSurfaceWalkerCamera::OnMouseMove()
{
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    switch (this->State)
    {
    case VTKIS_PAN:
        this->FindPokedRenderer(x, y);
        this->surfacePan();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;
    case VTKIS_ROTATE:
        this->FindPokedRenderer(x, y);
        this->Rotate();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;
    case VTKIS_DOLLY:
    case VTKIS_SPIN:
        this->FindPokedRenderer(x, y);
        this->Spin();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;
    }
}

void vtkInteractorStyleSurfaceWalkerCamera::testPointNormals()
{
  //std::cout << "In TestPointNormals: " << mPolyData->GetNumberOfPoints() << std::endl;
  bool hasPointNormals = getPointNormals();

  if(!hasPointNormals)
    {
    std::cout << "No point normals were found. Computing normals..." << std::endl;

    normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
#if VTK_MAJOR_VERSION <= 5
    normalGenerator->SetInput(mPolyData);
#else
    normalGenerator->SetInputData(mPolyData);
#endif
    normalGenerator->ComputePointNormalsOn();
    normalGenerator->ComputeCellNormalsOff();
    normalGenerator->Update();
    /*
    // Optional settings
    normalGenerator->SetFeatureAngle(0.1);
    normalGenerator->SetSplitting(1);
    normalGenerator->SetConsistency(0);
    normalGenerator->SetAutoOrientNormals(0);
    normalGenerator->SetComputePointNormals(1);
    normalGenerator->SetComputeCellNormals(0);
    normalGenerator->SetFlipNormals(0);
    normalGenerator->SetNonManifoldTraversal(1);
    */

    mPolyData = normalGenerator->GetOutput();

    hasPointNormals = getPointNormals();

    std::cout << "On the second try, has point normals? " << hasPointNormals << std::endl;

    }
  else
    {
    std::cout << "Point normals were found!" << std::endl;
    }
}

bool vtkInteractorStyleSurfaceWalkerCamera::getPointNormals()
{
  vtkIdType numPoints = mPolyData->GetNumberOfPoints();
  vtkIdType numPolys = mPolyData->GetNumberOfPolys();

  vtkDoubleArray* normalDataDouble = vtkDoubleArray::SafeDownCast(mPolyData->GetPointData()->GetArray("Normals"));

  if(normalDataDouble)
    {
    int nc = normalDataDouble->GetNumberOfTuples();
    return true;
    }

  vtkFloatArray* normalDataFloat = vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetArray("Normals"));

  if(normalDataFloat)
    {
    return true;
    }

  vtkDoubleArray* normalsDouble = vtkDoubleArray::SafeDownCast(mPolyData->GetPointData()->GetNormals());

  if(normalsDouble)
    {
    return true;
    }

  vtkFloatArray* normalsFloat =
    vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetNormals());

  if(normalsFloat)
    {
    return true;
    }

  vtkDataArray* normalsGeneric = mPolyData->GetPointData()->GetNormals();
  if(normalsGeneric)
    {
    return true;
    }

  return false;

}

void vtkInteractorStyleSurfaceWalkerCamera::surfacePan()
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();

    if(renderFlat)
    {
        mVTKWidget->setNonFlattenedMesh(mPolyData);
        renderFlat = false;
    }

    vtkRenderWindowInteractor *rwi = this->Interactor;

    double viewFocus[4], focalDepth;
    double newPickPoint[4], oldPickPoint[4], motionVector[3];

    camera->GetFocalPoint(viewFocus);
    this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
    focalDepth = viewFocus[2];

    double epx = rwi->GetEventPosition()[0];
    double epy = rwi->GetEventPosition()[1];

    this->ComputeDisplayToWorld(epx, epy, focalDepth, newPickPoint);

    double lepx = rwi->GetLastEventPosition()[0];
    double lepy = rwi->GetLastEventPosition()[1];

    this->ComputeDisplayToWorld(lepx, lepy, focalDepth, oldPickPoint);

    motionVector[0] = 0.5 * (oldPickPoint[0] - newPickPoint[0]);
    motionVector[1] = 0.5 * (oldPickPoint[1] - newPickPoint[1]);
    motionVector[2] = 0.5 * (oldPickPoint[2] - newPickPoint[2]);

    camera->GetFocalPoint(mViewTarget);
    camera->GetPosition(mCamPos);

    mCurrentGravity[0] = mViewTarget[0] - mCamPos[0];
    mCurrentGravity[1] = mViewTarget[1] - mCamPos[1];
    mCurrentGravity[2] = mViewTarget[2] - mCamPos[2];
    mCamDistance = vtkMath::Norm(mCurrentGravity);
    vtkMath::Normalize(mCurrentGravity);

    mViewTarget[0] = mViewTarget[0] + motionVector[0];
    mViewTarget[1] = mViewTarget[1] + motionVector[1];
    mViewTarget[2] = mViewTarget[2] + motionVector[2];

    stickTargetToSurface(camera, 0.01);

    if (rwi->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    rwi->Render();
}

void vtkInteractorStyleSurfaceWalkerCamera::linkPolyData(vtkPolyData *polyData, vtkCamera *camera)
{
    mPolyData = polyData;

    cellLocator = vtkSmartPointer<vtkCellLocator>::New();
    cellLocator->SetDataSet(polyData);
    cellLocator->BuildLocator();

    kdTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
    kdTree->SetDataSet(polyData);
    kdTree->BuildLocator();

    double aveNormal[3] = {0,0,0};
    testPointNormals();

    /*
    vtkFloatArray* normalDataFloat = vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetArray("Normals"));

    for(vtkIdType k = 0; k < mPolyData->GetNumberOfPoints(); ++k)
    {
        double nk[3];
        normalDataFloat->GetTuple(k, nk);
        aveNormal[0] = aveNormal[0] + nk[0];
        aveNormal[1] = aveNormal[1] + nk[1];
        aveNormal[2] = aveNormal[2] + nk[2];
    }
    aveNormal[0] = aveNormal[0] / normalDataFloat->GetSize();
    aveNormal[1] = aveNormal[1] / normalDataFloat->GetSize();
    aveNormal[2] = aveNormal[2] / normalDataFloat->GetSize();

    mCurrentGravity[0] = -aveNormal[0];
    mCurrentGravity[1] = -aveNormal[1];
    mCurrentGravity[2] = -aveNormal[2];
    vtkMath::Normalize(mCurrentGravity);

    double *meshCenter = mPolyData->GetCenter();
    mViewTarget[0] = meshCenter[0];
    mViewTarget[1] = meshCenter[1];
    mViewTarget[2] = meshCenter[2];
    stickTargetToSurface(camera, 0.01);
    */

    double viewFocus[4];
    double displayPt[4];
    double target2pos[3];
    camera->GetFocalPoint(viewFocus);
    this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], displayPt);
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.0005);
    picker->Pick(displayPt[0], displayPt[1], 0, this->GetDefaultRenderer());
    camera->GetPosition(mCamPos);
    mViewTarget[0] = picker->GetPickPosition()[0];
    mViewTarget[1] = picker->GetPickPosition()[1];
    mViewTarget[2] = picker->GetPickPosition()[2];
    vtkMath::Subtract(mCamPos,mViewTarget,target2pos);
    mCamDistance = vtkMath::Norm(target2pos);
    mCurrentGravity[0] = -(picker->GetPickNormal()[0]);
    mCurrentGravity[1] = -(picker->GetPickNormal()[1]);
    mCurrentGravity[2] = -(picker->GetPickNormal()[2]);
    stickTargetToSurface(camera, 0.01);
}

void vtkInteractorStyleSurfaceWalkerCamera::linkVtkWidget(VtkWidget *_vtkWidget)
{
    mVTKWidget = _vtkWidget;
}

vtkPolyData* vtkInteractorStyleSurfaceWalkerCamera::extractSubset(double R)
{
    qDebug() << "R: " << R;
    vtkSmartPointer<vtkIdList> vertexList = vtkSmartPointer<vtkIdList>::New();
    kdTree->FindPointsWithinRadius(R, mViewTarget, vertexList);
    int numberOfVertices = vertexList->GetNumberOfIds();
    qDebug() << "numverts: " << numberOfVertices;
    vtkSmartPointer<vtkIdList> cellList = vtkSmartPointer<vtkIdList>::New();
    for(int i = 0; i < numberOfVertices; ++i)
    {
        vtkIdType vi = vertexList->GetId(i);
        vtkSmartPointer<vtkIdList> cellList_i = vtkSmartPointer<vtkIdList>::New();
        mPolyData->GetPointCells(vi,cellList_i);
        for(int j = 0; j < cellList_i->GetNumberOfIds(); ++j)
        {
            vtkIdType cj = cellList_i->GetId(j);
            cellList->InsertUniqueId(cj);
        }
    }
    int numberOfCells = cellList->GetNumberOfIds();
    qDebug() << "numcells: " << numberOfCells;

    vtkSmartPointer<vtkIdFilter> idFilter = vtkSmartPointer<vtkIdFilter>::New();
    idFilter->PointIdsOff();
    idFilter->CellIdsOn();
    idFilter->SetInput(mPolyData);
    idFilter->SetIdsArrayName("originalIDs");
    idFilter->Update();
    vtkSmartPointer<vtkDataSet> polyDataPlusIDs = idFilter->GetOutput();

    vtkSmartPointer<vtkExtractCells> extractCells = vtkSmartPointer<vtkExtractCells>::New();
    extractCells->SetInput(polyDataPlusIDs);
    extractCells->SetCellList(cellList);
    extractCells->Update();
    vtkSmartPointer<vtkUnstructuredGrid> selected = vtkSmartPointer<vtkUnstructuredGrid>::New();
    selected->ShallowCopy(extractCells->GetOutput());

    originalCellIDs = vtkSmartPointer<vtkIdTypeArray>::New();
    originalCellIDs->DeepCopy(vtkIdTypeArray::SafeDownCast(extractCells->GetOutput()->GetCellData()->GetArray("originalIDs")));
    localTargetFaceID = originalCellIDs->LookupValue(targetFaceID);

    vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
    #if VTK_MAJOR_VERSION <= 5
      geometryFilter->SetInput(selected);
    #else
      geometryFilter->SetInputData(selected);
    #endif
      geometryFilter->Update();

    mPolyDataFlat = vtkSmartPointer<vtkPolyData>::New();
    mPolyDataFlat->ShallowCopy(geometryFilter->GetOutput());

    qDebug() << "numvertsflat" << mPolyDataFlat->GetPoints()->GetNumberOfPoints();

    /*
    vtkSmartPointer<vtkPolyDataConnectivityFilter> connectFilter = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
    connectFilter->SetInput(mPolyDataFlat);
    connectFilter->AddSeed(localTargetFaceID);
    connectFilter->SetExtractionModeToCellSeededRegions();
    connectFilter->Update();
    mPolyDataFlat->ShallowCopy(connectFilter->GetOutput());

    qDebug() << "numvertsflat" << mPolyDataFlat->GetPoints()->GetNumberOfPoints();
    */
	vtkPolyData *temp;
	return temp;
}

void vtkInteractorStyleSurfaceWalkerCamera::flattenSubset()
{
    int viewportWidth = GetCurrentRenderer()->GetSize()[0];
    int viewportHeight = GetCurrentRenderer()->GetSize()[1];
    qDebug() << "Viewport: " << viewportWidth << "x" << viewportHeight;
    double viewFocus[3], topRight[3], diag[3];
    ComputeWorldToDisplay(mViewTarget[0], mViewTarget[1], mViewTarget[2], viewFocus);
    double focalDepth = viewFocus[2];
    ComputeDisplayToWorld(viewportWidth, viewportHeight, focalDepth, topRight);

    diag[0] = mViewTarget[0] - topRight[0];
    diag[1] = mViewTarget[1] - topRight[1];
    diag[2] = mViewTarget[2] - topRight[2];
    mExtractRadius = vtkMath::Norm(diag);
    qDebug() << "Radius: " << mExtractRadius;

    extractSubset(mExtractRadius);
    lscm_engine->FlattenMesh(mPolyDataFlat.GetPointer());
    alignFlatMeshWith3D();
    mPolyDataFlat->GetCenter(mViewTargetFlat);
    renderFlat = true;
    mVTKWidget->setFlattenedMesh(mPolyDataFlat);
}

void vtkInteractorStyleSurfaceWalkerCamera::alignFlatMeshWith3D()
{
    vtkSmartPointer<vtkIdList> vertexList = vtkSmartPointer<vtkIdList>::New();
    vtkSmartPointer<vtkIdList> vertexListFlat = vtkSmartPointer<vtkIdList>::New();

    mPolyData->GetCellPoints(targetFaceID,vertexList);
    int vID0 = vertexList->GetId(0);
    int vID1 = vertexList->GetId(1);
    int vID2 = vertexList->GetId(2);
    //qDebug() << "global target: " << vID0 << ", " << vID1 << ", " << vID2;

    mPolyDataFlat->GetCellPoints(localTargetFaceID,vertexListFlat);
    int vID0flat = vertexListFlat->GetId(0);
    int vID1flat = vertexListFlat->GetId(1);
    int vID2flat = vertexListFlat->GetId(2);
    //qDebug() << "local target: " << vID0flat << ", " << vID1flat << ", " << vID2flat;

    double p0[3], p1[3], p2[3];
    double q0[3], q1[3], q2[3];

    mPolyData->GetPoints()->GetPoint(vID0,p0);
    mPolyData->GetPoints()->GetPoint(vID1,p1);
    mPolyData->GetPoints()->GetPoint(vID2,p2);

    mPolyDataFlat->GetPoints()->GetPoint(vID0flat,q0);
    mPolyDataFlat->GetPoints()->GetPoint(vID1flat,q1);
    mPolyDataFlat->GetPoints()->GetPoint(vID2flat,q2);

    vtkSmartPointer<vtkLandmarkTransform> landmarkTransform = vtkSmartPointer<vtkLandmarkTransform>::New();
    vtkSmartPointer<vtkPoints> sourcePts = vtkSmartPointer<vtkPoints>::New();
    sourcePts->InsertNextPoint(q0);
    sourcePts->InsertNextPoint(q1);
    sourcePts->InsertNextPoint(q2);
    vtkSmartPointer<vtkPoints> targetPts = vtkSmartPointer<vtkPoints>::New();
    targetPts->InsertNextPoint(p0);
    targetPts->InsertNextPoint(p1);
    targetPts->InsertNextPoint(p2);
    landmarkTransform->SetSourceLandmarks(sourcePts);
    landmarkTransform->SetTargetLandmarks(targetPts);
    landmarkTransform->SetModeToSimilarity();
    landmarkTransform->Update();

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInput(mPolyDataFlat);
    transformFilter->SetTransform(landmarkTransform);
    transformFilter->Update();
    //vtkMatrix4x4* mat = landmarkTransform->GetMatrix();
    //std::cout << "Matrix: " << *mat << std::endl;
    mPolyDataFlat = transformFilter->GetOutput();
}

void vtkInteractorStyleSurfaceWalkerCamera::stickTargetToSurface(vtkCamera *camera, double rayRadius)
{
    double rayEnd[3];
    rayEnd[0] = mViewTarget[0] + rayRadius * mCurrentGravity[0];
    rayEnd[1] = mViewTarget[1] + rayRadius * mCurrentGravity[1];
    rayEnd[2] = mViewTarget[2] + rayRadius * mCurrentGravity[2];

    double rayStart[3];
    rayStart[0] = mViewTarget[0] - rayRadius * mCurrentGravity[0];
    rayStart[1] = mViewTarget[1] - rayRadius * mCurrentGravity[1];
    rayStart[2] = mViewTarget[2] - rayRadius * mCurrentGravity[2];

    double t;
    double intersectPoint[3];
    int subID;
    vtkIdType cellID = -1;
    double pickUVW[3];

    int intersectionResult = cellLocator->IntersectWithLine(rayStart,rayEnd,0.00000001,t,intersectPoint,pickUVW,subID,cellID);
    if(intersectionResult == 0)
    {
        stickTargetToSurface(camera, rayRadius * 2);
    }
    else
    {
        targetFaceID = cellID;
        pickUVW[2] = 1 - pickUVW[0] - pickUVW[1];

        testPointNormals();

        vtkSmartPointer<vtkIdList> vertexList = vtkSmartPointer<vtkIdList>::New();
        mPolyData->GetCellPoints(cellID,vertexList);
        int vID0 = vertexList->GetId(0);
        int vID1 = vertexList->GetId(1);
        int vID2 = vertexList->GetId(2);

        double n0[3];
        double n1[3];
        double n2[3];
        double n_final[3];

        vtkFloatArray* normalDataFloat = vtkFloatArray::SafeDownCast(mPolyData->GetPointData()->GetArray("Normals"));

        if(normalDataFloat)
        {
            normalDataFloat->GetTuple(vID0, n0);
            normalDataFloat->GetTuple(vID1, n1);
            normalDataFloat->GetTuple(vID2, n2);

            n_final[0] = n0[0]*pickUVW[2] + n1[0]*pickUVW[0] + n2[0]*pickUVW[1];
            n_final[1] = n0[1]*pickUVW[2] + n1[1]*pickUVW[0] + n2[1]*pickUVW[1];
            n_final[2] = n0[2]*pickUVW[2] + n1[2]*pickUVW[0] + n2[2]*pickUVW[1];
        }

        double normal_dot_gravity = mCurrentGravity[0] * n_final[0] + mCurrentGravity[1] * n_final[1] + mCurrentGravity[2] * n_final[2];
        if(normal_dot_gravity > 0)
        {
            n_final[0] = -n_final[0];
            n_final[1] = -n_final[1];
            n_final[2] = -n_final[2];
        }

        camera->SetFocalPoint(intersectPoint);

        camera->GetFocalPoint(mViewTarget);

        vtkMath::Normalize(n_final);

        camera->SetPosition(mCamDistance * n_final[0] + mViewTarget[0],
                            mCamDistance * n_final[1] + mViewTarget[1],
                            mCamDistance * n_final[2] + mViewTarget[2]);

        camera->GetPosition(mCamPos);

        camera->OrthogonalizeViewUp();

        mCurrentGravity[0] = mViewTarget[0] - mCamPos[0];
        mCurrentGravity[1] = mViewTarget[1] - mCamPos[1];
        mCurrentGravity[2] = mViewTarget[2] - mCamPos[2];
        vtkMath::Normalize(mCurrentGravity);
    }
}
