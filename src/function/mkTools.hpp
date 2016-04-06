/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

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

*****************************************************************************/
#ifndef MKTOOLS_HPP
#define MKTOOLS_HPP

#include <QString>

#include <vtkMatrix4x4.h>
#include <vtkCoordinate.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <assert.h>

class mkDebug
{
public:
  mkDebug() {}
  ~mkDebug() {}

  void qDebugMatrix(QString name, vtkMatrix4x4 *mat)
  {
    if (name!=NULL)
      qDebug() << "[" << name << "]--------------------------------------------";
    if (mat==NULL)
        return;
    for(int j = 0; j <4 ; j ++){
  //     for(int i = 0; i <4 ; i ++)
      qDebug() << "[" << j << "," << 0 << "]" << mat->GetElement(j, 0) << " " \
                  "[" << j << "," << 1 << "]" << mat->GetElement(j, 1) << " " \
                  "[" << j << "," << 2 << "]" << mat->GetElement(j, 2) << " " \
                  "[" << j << "," << 3 << "]" << mat->GetElement(j, 3) << " ";
    }
  }

  void qDebugVector(double *vec)
  {
    qDebug() << "[0] " << vec[0] << "[1] " << vec[1] << "[2] " << vec[2];
  }

  //    mkDebug md;  md.qDebugImageData(mImageData, mImageData);
    void qDebugImageData(vtkImageData *imagedata1)
    {
        assert(imagedata1 != NULL);

        int dimss[4];
        imagedata1->GetDimensions(dimss);
        dimss[3] = imagedata1->GetNumberOfScalarComponents();

        assert(dimss[0]>0 && dimss[1]>0 );

        qDebug()  << "imagedata1: " << dimss[0] << " " << dimss[1] << " " << dimss[2] << ": " << dimss[3];

        float tests = 0;

        tests = imagedata1->GetScalarComponentAsFloat(10,10,0,1);

        qDebug()  << "imagedata1 (10,10,0,1): " << tests;
    }

//    mkDebug md;  md.qDebugImageData(mImageData, mImageData);
  void qDebugImageData(vtkImageData *imagedata1, vtkImageData *imagedata2)
  {
      int dimss[4], dimmh[4];
      imagedata1->GetDimensions(dimss);
      dimss[3] = imagedata1->GetNumberOfScalarComponents();
      imagedata2->GetDimensions(dimmh);
      dimmh[3] = imagedata2->GetNumberOfScalarComponents();

      qDebug() << "assert1";
      assert(dimss[0]>0 && dimss[1]>0 );
      qDebug() << "assert2";
      assert(dimmh[0]>0 && dimmh[1]>0 );

      qDebug()  << "imagedata1: " << dimss[0] << " " << dimss[1] << " " << dimss[2] << ": " << dimss[3];
      qDebug()  << "imagedata2: " << dimmh[0] << " " << dimmh[1] << " " << dimmh[2] << ": " << dimmh[3];

      float tests = 0;
      float testh = 0;

      tests = imagedata1->GetScalarComponentAsFloat(10,10,0,1);
      testh = imagedata2->GetScalarComponentAsFloat(10,10,0,1);

      qDebug()  << "imagedata1 (10,10,0,1): " << tests;
      qDebug()  << "imagedata2 (10,10,0,1): " << testh;
  }

};// end of mkDebug

class mkVtk
{
public:
  mkVtk() {}
  ~mkVtk() {}

  // transformation from World coordinates (3d) to Display coordinates (2d)
  int * transWorldtoDisplay(double *input, vtkRenderer *ren)
  {
    vtkCoordinate *vco = vtkCoordinate::New();
    vco->SetCoordinateSystemToWorld();
    vco->SetValue(input);
    int *output = vco->GetComputedLocalDisplayValue(ren);
    return output;
  }
  // transformation from Display coordinates (2d) to World coordinates (3d)
  double * transDisplaytoWorld(double *input, vtkRenderer *ren)
  {
    vtkCoordinate *vco = vtkCoordinate::New();
    vco->SetCoordinateSystemToWorld();
    vco->SetValue(input);
    double *output = vco->GetComputedWorldValue(ren);
    return output;
  }
};
#endif // MKTOOLS_HPP
