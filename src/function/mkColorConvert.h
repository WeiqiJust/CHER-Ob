/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)
              David Tidmarsh (tidmarsh@aya.yale.edu)

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

 - Acknowledgments: This code was converted from Min's original Matlab codes for
   color conversions with help of David Tidmarsh in 2011 summer. This code
   requires CLAPACK/BLAS and Armadillo CLAPACK/BLAS wrapper library.

*****************************************************************************/
#ifndef MKCOLORCONVERT_H
#define MKCOLORCONVERT_H

#include <armadillo>
#include <vector>

using namespace arma;

class MKCC
{
public:
  // main function
  void Rad2sRGB( fvec wvls,  fcube Rad, fcube &sRGB,  fmat refwhite, int islinear, int isstretch);
  void Rad2sRGB( fvec wvls,  fcube Rad, fcube &sRGB,  fmat refwhite, int islinear);
  void Rad2sRGB( fvec wvls,  fcube Rad, fcube &sRGB,  fmat refwhite);

  // User specifies refwhite values
  void Rad2Ref(fmat Rad, fmat refwhite, fmat &wbR);
  void Rad2Ref(fcube Rad, fmat refwhite, fcube &wbR);
  // Default value of refwhite
  void Rad2Ref( fmat Rad, fmat &wbR);
  void Rad2Ref( fcube Rad, fcube &Ref);

  void Ref2XYZ( fvec wvls,  fcube Ref,  std::string illum, fcube &XYZ);
  void Ref2XYZ( fvec wvls,  fmat Ref,  std::string illum, fmat &XYZ);

  // Calculates the pth percentile of the numbers in a 3D vector, ascending
  float prctile( fvec &temp, float p);

  void stretch_hist(fcube &RGB);
  void stretch_hist(fcube &RGB,  float LOW,  float HIGH);

  // Calculates IEC:61966 sRGB values from XYZ
  void XYZ2sRGB( fcube XYZ0, fcube &RGB);
  void XYZ2sRGBlinear( fcube XYZ0, fcube &RGB);

  void d(fmat &allXYZ);
  void d(frowvec &XYZ, int k);
  void d(frowvec &XYZ, std::string k);

  // test function
  void qsidisplay(fmat refwhite, int islinear, std::string sourcepath, int isstretch = 0);
  void qsidisplay(fmat refwhite, int islinear = 0);
  void qsidisplay(void);


  void illuminantFcn( std::string formulary, fvec &lambda, fvec &LFcn);
  void CMF_JUDD_VOS_lambda(fvec &lambda);
  void CMF_JUDD_VOS_xFcn(fvec &xFcn);
  void CMF_JUDD_VOS_yFcn(fvec &yFcn);
  void CMF_JUDD_VOS_zFcn(fvec &zFcn);
#ifdef FULLCMF
// mkcolorconvert.cpp:2592: error: C1063: compiler limit : compiler stack overflow
  void CMF_JUDD_VOS(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void colorMatchFcn(std::string formulary, fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void CMF_CIE_1931(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void CMF_CIE_1964(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void CMF_JUDD(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void CMF_STILES_2(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void CMF_STILES_10(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void CMF_CIE_1931_FULL(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
  void CMF_CIE_1964_FULL(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn);
#endif
protected:

private:
};
#endif // MKCOLORCONVERT_H
