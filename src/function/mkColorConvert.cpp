/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)
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
   color conversions with help of David Tidmarsh in 2012 summer. This code
   requires CLAPACK/BLAS and Armadillo CLAPACK/BLAS wrapper library.

*****************************************************************************/
#define HEIGHT (6)
#define LENGTH (3)
#define NUM_FNCS_4 (4)

#define waveArrayCMF(x) (x - (lambdaCMF(0) ))
#define waveArrayLumin(x) (x - (lambdaLumin(0) ))

#define prct_rank(n, val) (float)(100.f/n * (val - 0.5))

#include "mkColorConvert.h"

#include <stdint.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdarg>
#include <math.h>

using namespace arma;

//============================================================================================
// R2sRGBQSI function: (main function)

void MKCC::Rad2sRGB( fvec wvls, fcube Rad, fcube &sRGB,  fmat refwhite) {
  Rad2sRGB(wvls, Rad, sRGB, refwhite, 0, 0);
}

void MKCC::Rad2sRGB( fvec wvls, fcube Rad, fcube &sRGB,  fmat refwhite, int islinear) {
  Rad2sRGB(wvls, Rad, sRGB, refwhite, islinear, 0);
}

void MKCC::Rad2sRGB( fvec wvls, fcube Rad, fcube &sRGB,  fmat refwhite, int islinear, int isstretch) {

  //===============================================================================
  // Calculating reflectance from radiance (of material and the reference white)
  // then calculate XYZ values from it.

  // remove noise floor in solid-state signals
  Rad = Rad - 0.0031f; // lowest signals in the 8 channels (UV) -> this is only for QSI
  fcube Ref, Ref_XYZ;
  fmat WhiteRef(refwhite), White_XYZ;
  std::string illumin = "d65";

  if(refwhite.is_empty()) { // if the reference white is empty, then estimate the reference white
    // automatic white balance
    Rad2Ref(Rad, refwhite, Ref);
  } else {
    /*	manual white balance. -> 3D scanning
      white reference coefficient
      (The Yale Graphics Spectralon, measured 3/13/2012
      (1) 97.81% --> Reference White
      (2) 50.14%
      (3) 26.58%
      (4) 14.89%
    */
    fmat refwhite2 = refwhite/0.9781f; // considering the actual reflectance of the spectralon
    fmat WhiteRef2(refwhite2);
    WhiteRef = WhiteRef2;
    // workflow
    // 1. radiance to reflectance (scaling by refwhite)
    Rad2Ref(Rad, refwhite2, Ref);
    Rad2Ref(refwhite, refwhite2, WhiteRef);
  }

  Ref2XYZ(wvls, Ref, illumin, Ref_XYZ);
  Ref2XYZ(wvls, WhiteRef, illumin, White_XYZ); // this is not necessary for the case without the reference white

  // 2. reflectance to CIEXYZ (2-degree, D50)
  //cout << "white point (" << illumin << "):" << endl;  
//  Ref_XYZ.print();
//  White_XYZ.print();
  //===============================================================================
  // White balancing
  fcube Ref_XYZw;
  float maxY;
  if(refwhite.is_empty()) {
    // automatic white balance (grayworld)
    fcube mmean(1,1,Ref_XYZ.n_slices);
    for(unsigned int i = 0; i < mmean.n_slices; ++i) {
      mmean(0,0,i) = mean(mean(Ref_XYZ.slice(i)));
    }
    maxY = max(max(Ref_XYZ.slice(1)));

    Ref_XYZw = (mmean * maxY)/mmean(0,0,1);
    fcube rpmean(Rad.n_rows, Rad.n_cols, Ref_XYZw.n_slices);
    for(unsigned int i = 0; i < Ref_XYZw.n_slices; ++i) {
      rpmean.slice(i).fill(Ref_XYZw(0,0,i));
    }
    Ref_XYZw = Ref_XYZ/rpmean;

    isstretch = 1;
  } else {
    // 3. Von Kries White Balancing in D65
    fcube White_XYZrep(Rad.n_rows, Rad.n_cols, Ref_XYZw.n_slices);
    for(unsigned int i = 0; i < White_XYZ.n_elem; ++i) {
      White_XYZrep.slice(i).fill(White_XYZ(i));
    }
    Ref_XYZw = Ref_XYZ/White_XYZrep;
  }

  //===============================================================================
  // chromatic adaptation into D65
  // apply D65 white point for sRGB
  frowvec dXYZ;
  d(dXYZ, 65);
  dXYZ = dXYZ/100;
  fcube wpD65(Rad.n_rows, Rad.n_cols, dXYZ.n_elem);
  for(unsigned int i = 0; i < dXYZ.n_elem; ++i) {
    wpD65.slice(i).fill(dXYZ(i));
  }
  Ref_XYZw = wpD65 % Ref_XYZw;

  //===============================================================================
  // signal level adjustment for calculating display signals
  // optional process (stretching tone).
    // don't use this by default (only for the final screen view)
  if(isstretch == 1) {
    // normalization based on Y -> fluctuate depending on image properties
    float minY = min(min(Ref_XYZw.slice(1)));
    maxY = max(max(Ref_XYZw.slice(1)));
    fcube temp(Ref_XYZw.n_rows, Ref_XYZw.n_cols, 3);
    temp.fill(minY);
    Ref_XYZw = (Ref_XYZw - temp)/(maxY - minY);
    for(fcube::iterator it = Ref_XYZw.begin(); it != Ref_XYZw.end(); ++it) {
      if(*it > 1) *it = 1;
      else if(*it < 0) *it = 0;
    }
    // histogram stretching from 1% to 99%
    stretch_hist(Ref_XYZw, 1, 99);
  }

  //===============================================================================

  // 3. CIEXYZ to sRGB (gamma = 2.2);
  if(islinear == 1)
    XYZ2sRGBlinear(Ref_XYZw, sRGB);
  else
    XYZ2sRGB(Ref_XYZw, sRGB);

}
//============================================================================================
// Rad2Ref function:

// Ref is 2D matrix
void MKCC::Ref2XYZ(fvec wvls, fmat Ref, std::string illum, fmat &XYZ)
{
  if (Ref.n_elem == 0)
    return;

  // maximum photographic luminous efficacy
  int Km = 683;

  // derive color matching function
    fvec lambdaCMF, xFcn, yFcn, zFcn;
//  colorMatchFcn("judd_vos", lambdaCMF, xFcn, yFcn, zFcn);// Don't use this
//    CMF_JUDD_VOS(lambdaCMF, xFcn, yFcn, zFcn);
    CMF_JUDD_VOS_lambda(lambdaCMF);
    CMF_JUDD_VOS_xFcn(xFcn);
    CMF_JUDD_VOS_yFcn(yFcn);
    CMF_JUDD_VOS_zFcn(zFcn);

  // derive illuminant function
  fvec lambdaLumin, LFcn;
  illuminantFcn(illum, lambdaLumin, LFcn);

  int dd = Ref.n_rows;
  int vv = Ref.n_cols;

  fmat Lybar;
  Lybar.zeros(dd,vv);
  fmat XYZ1(Ref);
  fmat XYZ2(Ref);
  fmat XYZ3(Ref);

  uint8_t wvarrayCMF, wvarrayLumin;
  // i is wavelength
  for(int i = 0; i < dd; ++i) {
    if(waveArrayCMF(floor(float(wvls(i)))) < 0) wvarrayCMF = 0;
    else if(waveArrayCMF(floor(float(wvls(i)))) > 255) wvarrayCMF = 255;
    else wvarrayCMF = uint8_t(waveArrayCMF(floor(float(wvls(i)))));

    if(waveArrayLumin(floor(float(wvls(i)))) < 0) wvarrayLumin = 0;
    else if(waveArrayLumin(floor(float(wvls(i)))) > 255) wvarrayLumin = 255;
    else wvarrayLumin = uint8_t(waveArrayLumin(floor(float(wvls(i)))));
    // fit the luminance range
    if(floor(float(wvls(i))) < 380 || floor(float(wvls(i))) > 730) {
      XYZ1.row(i).zeros();
      XYZ2.row(i).zeros();
      XYZ3.row(i).zeros();
      Lybar.row(i).zeros();
    } else {
      XYZ1.row(i) = XYZ1.row(i) * xFcn(wvarrayCMF - 1) * LFcn(wvarrayLumin - 1);
      XYZ2.row(i) = XYZ2.row(i) * yFcn(wvarrayCMF - 1) * LFcn(wvarrayLumin - 1);
      XYZ3.row(i) = XYZ3.row(i) * zFcn(wvarrayCMF - 1) * LFcn(wvarrayLumin - 1);
      Lybar.row(i).fill(yFcn(wvarrayCMF - 1) * LFcn(wvarrayLumin - 1));
    }
  }

  // summation of energy (3D -> 2D)
  XYZ.col(0) = sum(XYZ1);
  XYZ.col(1) = sum(XYZ2);
  XYZ.col(2) = sum(XYZ3);

  fvec k;
  if(Lybar.size() > 0) {
    k = sum(Lybar);
    for(unsigned int i = 0; i < k.n_rows; ++i) if(k(i) != 0) k(i) = 100/k(i);
  } else {
    k.set_size(1,1);
    k(0,0) = 0;
  }

  fmat k2;
  if(vv > 0) k2 = repmat(k, vv, 3);
  else k2 = repmat(k, 1, 3);
  if(XYZ.size() > 0) XYZ = k2 % XYZ;
  else XYZ = k2;

  for(fcube::iterator it = XYZ.begin(); it != XYZ.end(); ++it) {
    if(*it < 0) *it = 0.0;
  }
}

// Rad is 2D. User specifies refwhite values
void MKCC::Rad2Ref(fmat Rad, fmat refwhite, fmat &wbR)
{
  // set all values in Rad to 0 or greater
  for(fcube::iterator i = Rad.begin(); i != Rad.end(); ++i) {
    if(*i < 0) *i = 0.0;
  }

  if(refwhite.is_empty()) {
    wbR = Rad;
    return;
  }

  // element-wise division
  wbR = Rad/refwhite;
}

// default value of refwhite
void MKCC::Rad2Ref(fmat Rad, fmat &wbR)
{
  fmat refwhite;
  Rad2Ref(Rad, refwhite, wbR);
}

// R is 3D. User specifies refwhite values
void MKCC::Rad2Ref(fcube Rad, fmat refwhite, fcube &wbR)
{
  // set all values in Rad to 0 or greater
  for(fcube::iterator i = Rad.begin(); i != Rad.end(); ++i) {
    if(*i < 0) *i = 0.0;
  }

  // get dimensions of Rad
  int h = Rad.n_rows;
  int w = Rad.n_cols;
  int d = Rad.n_slices;

  if(refwhite.is_empty()) {
    wbR = Rad;
    return;
  }

  // repeat refwhite to create h x w x d matrix
  fcube temp(h, w, d);
  for(int i = 0; i < d; ++i) {
    temp.slice(i).fill(refwhite(i));
  }

  // element-wise division
  wbR = Rad/temp;

  return;
}

// default value of refwhite
void MKCC::Rad2Ref(fcube Rad, fcube &Ref)
{
  fmat refwhite;
  Rad2Ref(Rad, refwhite, Ref);
}

//============================================================================================
// Rad2XYZ function:

// Ref is 3D cube
void MKCC::Ref2XYZ(fvec wvls, fcube Ref, std::string illum, fcube &XYZ)
{
  // maximum photographic luminous efficacy
  int Km = 683;

  // derive color matching function
  fvec lambdaCMF, xFcn, yFcn, zFcn;
  //  colorMatchFcn("judd_vos", lambdaCMF, xFcn, yFcn, zFcn);// Don't use this
  //    CMF_JUDD_VOS(lambdaCMF, xFcn, yFcn, zFcn);
      CMF_JUDD_VOS_lambda(lambdaCMF);
      CMF_JUDD_VOS_xFcn(xFcn);
      CMF_JUDD_VOS_yFcn(yFcn);
      CMF_JUDD_VOS_zFcn(zFcn);

  // derive illuminant function
  fvec lambdaLumin, LFcn;
  illuminantFcn(illum, lambdaLumin, LFcn);

  int hh = Ref.n_rows;
  int ww = Ref.n_cols;
  int dd = Ref.n_slices;

  fcube Lybar = zeros<fcube>(hh,ww,dd);
  fcube XYZ1(Ref);
  fcube XYZ2(Ref);
  fcube XYZ3(Ref);

  uint8_t wvarrayCMF, wvarrayLumin;
  // i is wavelength
  for(int i = 0; i < dd; ++i) {
    if(waveArrayCMF(floor(float(wvls(i)))) < 0) wvarrayCMF = 0;
    else if(waveArrayCMF(floor(float(wvls(i)))) > 255) wvarrayCMF = 255;
    else wvarrayCMF = uint8_t(waveArrayCMF(floor(float(wvls(i)))));

    if(waveArrayLumin(floor(float(wvls(i)))) < 0) wvarrayLumin = 0;
    else if(waveArrayLumin(floor(float(wvls(i)))) > 255) wvarrayLumin = 255;
    else wvarrayLumin = uint8_t(waveArrayLumin(floor(float(wvls(i)))));
    // fit the luminance range
    if(floor(float(wvls(i))) < 380 || floor(float(wvls(i))) > 730) {
      XYZ1.slice(i).zeros();
      XYZ2.slice(i).zeros();
      XYZ3.slice(i).zeros();
    } else {
      XYZ1.slice(i) = XYZ1.slice(i) * xFcn(wvarrayCMF) * LFcn(wvarrayLumin);
      XYZ2.slice(i) = XYZ2.slice(i) * yFcn(wvarrayCMF) * LFcn(wvarrayLumin);
      XYZ3.slice(i) = XYZ3.slice(i) * zFcn(wvarrayCMF) * LFcn(wvarrayLumin);
      Lybar.slice(i).fill(yFcn(wvarrayCMF) * LFcn(wvarrayLumin));
    }
  }

  // summation of energy (4D -> 3D)
  XYZ.reset();
  float sum1, sum2, sum3;
  hh = XYZ1.n_rows;
  ww = XYZ1.n_cols;
  dd = XYZ1.n_slices;
  XYZ.set_size(hh,ww,3);
  fmat temp1(hh,ww);
  fmat temp2(hh,ww);
  fmat temp3(hh,ww);
  int i, j, k;
  for(i = 0; i < hh; ++i) {
    for(j = 0; j < ww; ++j) {
      sum1 = sum2 = sum3 = 0;
      for(k = 0; k < dd; ++k) {
        sum1 += XYZ1(i,j,k);
        sum2 += XYZ2(i,j,k);
        sum3 += XYZ3(i,j,k);
      }
      temp1(i,j) = sum1;
      temp2(i,j) = sum2;
      temp3(i,j) = sum3;
    }
  }
  XYZ.slice(0) = temp1;
  XYZ.slice(1) = temp2;
  XYZ.slice(2) = temp3;

  fmat Ltemp(hh,ww);
  float Lsum;
  for(int i = 0; i < hh; ++i) {
    for(int j = 0; j < ww; ++j) {
      Lsum = 0;
      for(int k = 0; k < dd; ++k) Lsum += Lybar(i,j,k);
      Ltemp(i,j) = Lsum;
    }
  }

  // normalization
  fmat m(hh,ww);
  for(i = 0; i < hh; ++i) {
    for(j = 0; j < ww; ++j) {
      if(Ltemp(i,j) != 0) m(i,j) = 100/Ltemp(i,j);
      else m(i,j) = 0;
    }
  }

  fcube k3(hh,ww,3);
  k3.slice(0) = k3.slice(1) = k3.slice(2) = m;
  XYZ = k3 % XYZ;

  // set all values in XYZ to 0 or greater
  for(fcube::iterator it = XYZ.begin(); it != XYZ.end(); ++it) {
    if(*it < 0) *it = 0.0;
  }

}

//============================================================================================
// stretch_hist function:

float MKCC::prctile( fvec &temp, float p)
{
  int x_len = temp.size();
  float p_first = prct_rank(x_len, 1);
  float p_last = prct_rank(x_len, x_len);
  // if the desired percentage is outside the bounds, return the greatest or least value available
  if(p <= p_first) return temp[0];
  if(p >= p_last) return temp[x_len - 1];

  int prct_found = 0;
  int calc = (int)((p/100.f)*x_len);
  float tester = prct_rank(x_len, calc + 1);

  while(!prct_found) {
    if(tester > p) {
      tester = prct_rank(x_len, --calc + 1);
    } else if(tester < p) {
      if((prct_rank(x_len, calc + 2) < p) && (temp[calc + 1] > temp[calc])) {
        ++calc;
      } else prct_found = 1;
    } else {
      return temp[calc];
    }
  }

  // formula for percentile through linear interpolation
  return (x_len * ((p - prct_rank(x_len, calc+1))/100) * (temp[calc+1] - temp[calc]) + temp[calc]);
}

void MKCC::stretch_hist(fcube &RGB, float LOW, float HIGH)
{
  if(HIGH > 100 || HIGH < 0) {
    cerr << "Error: stretch_hist:badHighValue: Value of HIGH must be between 0 and 100.\n";
    return;
  } else if(LOW > 100 || LOW < 0) {
    cerr << "Error: stretch_hist:badLowValue: Value of LOW must be between 0 and 100.\n";
    return;
  } else if(LOW > HIGH) {
    cerr << "Error: stretch_hist:lowIsBigger: Value of LOW must not be greater than that of HIGH.\n";
    return;
  }

  // transfer cube values into 1D vector
  int h = RGB.n_cols;
  int w = RGB.n_rows;
  int d = RGB.n_slices;
  fvec temp;
  fmat RGBslice;
  for(int i = 0; i < d; ++i) {
    RGBslice = RGB.slice(i);
    RGBslice.reshape(h*w,1);
    temp.insert_rows(temp.n_rows, RGBslice);
  }

  temp = arma::sort(temp);

  float pcnt_high = prctile(temp, HIGH);
  float pcnt_low = prctile(temp, LOW);

  for(fcube::iterator i = RGB.begin(); i != RGB.end(); ++i) {
    // if values in RGB are greater than pcnt_high or less than pcnt_low, replace them with pcnt_high/low respectively
    if(*i > pcnt_high) *i = pcnt_high;
    else if(*i < pcnt_low) *i = pcnt_low;
    // re-normalizing
    *i = (*i - pcnt_low)/(pcnt_high - pcnt_low);
    if(*i > 1.0) *i = 1.0;
    else if(*i < 0.0) *i = 0.0;
  }

}

void MKCC::stretch_hist(fcube &RGB)
{
  stretch_hist(RGB, 0.1f, 99.9f);
}

//============================================================================================
// XYZ2sRGB function:
void MKCC::XYZ2sRGB( fcube XYZ0, fcube &RGB)
{
  fmat::fixed<3,3> M;

  M << 3.2406f << -1.5372f << -0.4986f << endr
    << -0.9689f << 1.8758f << 0.0415f << endr
    << 0.0557f << -0.2040f << 1.0570f << endr;

  int row = XYZ0.n_rows;
  int col = XYZ0.n_cols;
  int slice = XYZ0.n_slices;
  int rc = row*col;

  // reshape XYZ0 into a (rows * columns)-by-3 matrix
  fmat XYZ, XYZ0slice;
  for(int i = 0; i < slice; ++i) {
    XYZ0slice = XYZ0.slice(i);
    XYZ0slice.reshape(rc,1);
    XYZ.insert_cols(i, XYZ0slice);
  }

  fmat sRGB = trans(M * trans(XYZ));

  // apply gamma function
  float g = 1.f/2.2f;
  for(fcube::iterator q = sRGB.begin(); q != sRGB.end(); ++q) {
    if(*q > 1) *q = 1;
    else if(*q < 0) *q = 0;
    // scale to range 0-255
    *q = pow(*q, g);
    // clip to range
    if(*q > 1) *q = 1;
    else if(*q < 0) *q = 0;
  }

  RGB.reset();
  RGB.copy_size(XYZ0);
  fmat temp;
  for(int i = 0; i < slice; ++i) {
    temp = sRGB.col(i);
    temp.reshape(row, col);
    RGB.slice(i) = temp;
  }

  return;
}

void MKCC::XYZ2sRGBlinear( fcube XYZ0, fcube &RGB)
{
  fmat::fixed<3,3> M;

  M << 3.2406f << -1.5372f << -0.4986f << endr
    << -0.9689f << 1.8758f << 0.0415f << endr
    << 0.0557f << -0.2040f << 1.0570f << endr;

  int row = XYZ0.n_rows;
  int col = XYZ0.n_cols;
  int slice = XYZ0.n_slices;
  int rc = row*col;

  // reshape XYZ0 into a (rows * columns)-by-3 matrix
  fmat XYZ, XYZ0slice;
  for(int i = 0; i < slice; ++i) {
    XYZ0slice = XYZ0.slice(i);
    XYZ0slice.reshape(rc,1);
    XYZ.insert_cols(i, XYZ0slice);
  }

  fmat sRGB = trans(M * trans(XYZ));

  // apply gamma function
//  float g = 1.f/2.2f;
  for(fcube::iterator q = sRGB.begin(); q != sRGB.end(); ++q) {
    if(*q > 1) *q = 1;
    else if(*q < 0) *q = 0;
    // scale to range 0-255
//    *q = pow(*q, g);
    // clip to range
    if(*q > 1) *q = 1;
    else if(*q < 0) *q = 0;
  }

  RGB.reset();
  RGB.copy_size(XYZ0);
  fmat temp;
  for(int i = 0; i < slice; ++i) {
    temp = sRGB.col(i);
    temp.reshape(row, col);
    RGB.slice(i) = temp;
  }

  return;
}
//============================================================================================
// qsidisplay (test function by David)

void MKCC::qsidisplay(fmat refwhite, int islinear, std::string sourcepath, int isstretch)
{
  if(sourcepath.empty()) {
    cerr << "No filename entered. Exiting..." << endl;
    return;
  }
  char *str;
  str = &sourcepath[0];
  //fcube cdata;
  fcube sRGB;
  fvec wvls;
  wvls << 370 << 417 << 475 << 525 << 585 << 628 << 675 << 755 << endr;
  cout << "Reading data..." << endl;
  //readEXR(str);
  fcube cdata(832, 626, 8);
  cdata.slice(0).load("cdata1.dat", csv_ascii);
  cdata.slice(1).load("cdata2.dat", csv_ascii);
  cdata.slice(2).load("cdata3.dat", csv_ascii);
  cdata.slice(3).load("cdata4.dat", csv_ascii);
  cdata.slice(4).load("cdata5.dat", csv_ascii);
  cdata.slice(5).load("cdata6.dat", csv_ascii);
  cdata.slice(6).load("cdata7.dat", csv_ascii);
  cdata.slice(7).load("cdata8.dat", csv_ascii);
  cout << "Done." << endl;

  /*ofstream myfile;
  ucube randomcube(10,10,8);
  srand ( time(NULL) );
  for(ucube::iterator i = randomcube.begin(); i != randomcube.end(); ++i) {
    *i = rand() % 256;
  }
  myfile.open("random.txt");
  for(ucube::iterator i = randomcube.begin(); i != randomcube.end(); ++i) {
    myfile << *i << " ";
  }*/


  /* -----------------------------------------------------
     * This is the white scaling (normalization) part.
     * Don't try to scale the values in other places.
     * -----------------------------------------------------
   */

  fcube wcdata;
  if(refwhite.n_rows > 2) { // if there is a reference white input
    // input: raw radiance before white balancing
    Rad2sRGB(wvls, cdata, sRGB, refwhite, islinear, isstretch);
    // output: reflectance after calculation
    Rad2Ref(cdata, refwhite, wcdata);
  }
  else if(refwhite(0) == 1) { // reflectance mode
    cout << "Calculating reflectance now." << endl;
    Rad2sRGB(wvls, cdata, sRGB, refwhite, islinear, isstretch);
    Rad2Ref(cdata, refwhite, wcdata);
  }
  else {
    cout << "This is the radiance mode." << endl;
    wcdata = cdata;
    refwhite.clear();
    Rad2sRGB(wvls, cdata, sRGB, refwhite, islinear, isstretch);
  }

  cout << "Finished." << endl;
  sRGB.slice(1).submat(300, 300, 320, 307).print();
}

void MKCC::qsidisplay(fmat refwhite, int islinear)
{
  std::string sourcepath;
  cout << "Please enter the name of the input file." << endl;
  std::getline(std::cin, sourcepath);
  if(sourcepath.empty()) {
    cerr << "No filename entered. Exiting..." << endl;
    return;
  }
  qsidisplay(refwhite, islinear, sourcepath);
}

void MKCC::qsidisplay(void)
{
  fmat refwhite;
  qsidisplay(refwhite);
}

//============================================================================================
// D function:

/*	I changed the white point values to get it more accurate
  by reference of CIE 15.3 and ISO13655:1996 and ASTM E308-95
  D: returns XYZ tristimulus values of various CIE illuminants.

        xyz=D(50) returns XYZ values for the D50 illuminant
        xyz=D(55) returns XYZ values for the D55 illuminant
        xyz=D(65) returns XYZ values for the D65 illuminant
        xyz=D(75) returns XYZ values for the D75 illuminant
        xyz=D('A') returns XYZ values for Illuminant A
        xyz=D('C') returns XYZ values for Illuminant C

  'D' with no arguments displays the tristimulus values of
  all four D illuminants together with A and C illuminants.

  XYZ=D displays the values as above and returns the array
  of tristimulus values for all the illuminants

  old version of Phil Green
  A=[109.85,100,35.8];
  C=[98.07,100,118.23];
  D50=[96.42,100,82.49];
  D55=[96.68,100,92.14];
  D65=[95.04,100,108.89];
  D75=[94.96,100,122.62];
  allXYZ=[A;C;D50;D55;D65;D75];

  Revised version by CIE 15.3 */

void MKCC::d(fmat &allXYZ)
{
  allXYZ << 109.850f << 100.f << 35.585f << endr
         << 98.074f << 100.f << 118.232f << endr
         << 96.422f << 100.f << 82.521f << endr
         << 95.682f << 100.f << 92.149f << endr
         << 95.047f << 100.f << 108.883f << endr
         << 94.96f << 100.f << 122.62f << endr;

  std::string labels[HEIGHT] = {" A    ", " C    ", " D50  ", " D55  ", " D65  ", " D75  "};

  int i, j;
  for(i = 0; i < HEIGHT; ++i) {
    cout << labels[i];
    for(j = 0; j < LENGTH; ++j) {
      cout << std::setw(8) << std::setiosflags(ios::fixed) << std::setprecision(2) << allXYZ(i,j);
    }
    cout << "\n";
  }
}

void MKCC::d(frowvec &XYZ, int k)
{
  switch(k)
  {
    case 50: // D50
      {
        XYZ << 96.422f << 100.f << 82.521f << endr;
        break;
      }
    case 55: // D55
      {
        XYZ << 95.682f << 100.f << 92.149f << endr;
        break;
      }
    case 60: // D60
      {
        // nothing here (yet)
        break;
      }
    case 65: // D65
      {
        XYZ << 95.047f << 100.f << 108.883f << endr;
        break;
      }
    case 75: // D75
      {
        XYZ << 94.96f << 100.f << 122.62f << endr;
        break;
      }
    default:
      {
        cerr << "Error: d:unrecognizedFcn: Unrecognized function.\n";
        break;
      }
  }
}

void MKCC::d(frowvec &XYZ, std::string k)
{
  std::string names[] =
    {
      "cie_a", "cie_c", "apple63", "apple60"
    };

  int i;
  for(i = 0; k[i] != '\0'; i++) {
    k[i] = tolower(k[i]);
  }

  size_t index;
  index = NUM_FNCS_4;
  for(i = 0; i < NUM_FNCS_4; ++i) {
    if(names[i].find(k) != std::string::npos) {
      index = i;
      break;
    }
  }

  switch(index)
  {
    case 0: // cie_a
      {
        XYZ << 109.850f << 100.f << 35.585f << endr;
        break;
      }
    case 1: // cie_c
      {
        XYZ << 98.074f << 100.f << 118.232f << endr;
        break;
      }
    case 2: // apple63
      {
        // nothing here (yet)
        break;
      }
    case 3: // apple60
      {
        // nothing here (yet)
        break;
      }
    default:
      {
        cerr << "Error: d:unrecognizedFcn: Unrecognized function.\n";
        break;
      }
  }
}


//============================================================================================
// illuminantFcn function:

/*
  CIE Illumination Function

  Returns the CIE illuminant function at each wavelength in
    the vector LAMBDA. FORMULARY is a string specifying which set of
    CIE illuminants to return.

    CIE D50
    CIE D65
    CIE A
*/
#define NUM_FNCS_3 (3)
void MKCC::illuminantFcn(std::string formulary, fvec &lambda, fvec &LFcn)
{
  lambda.reset();
  LFcn.reset();

  std::string names[] =
    {
      "d65", "d50", "a"
    };

  int i;
  for(i = 0; formulary[i] != '\0'; i++) {
    formulary[i] = tolower(formulary[i]);
  }

  size_t index;
  index = NUM_FNCS_3;
  for(i = 0; i < NUM_FNCS_3; ++i) {
    if(names[i].find(formulary) != std::string::npos) {
      index = i;
      break;
    }
  }

  switch(index)
  {
    case 0: // d65
      {
        fmat::fixed<351,2> illumin;
        illumin << 380.f << 24.4610000000000f << endr
            << 381.f << 24.9989400000000f << endr
            << 382.f << 25.5368800000000f << endr
            << 383.f << 26.0748200000000f << endr
            << 384.f << 26.6127600000000f << endr
            << 385.f << 27.1507000000000f << endr
            << 386.f << 27.6886402000000f << endr
            << 387.f << 28.2265804000000f << endr
            << 388.f << 28.7645206000000f << endr
            << 389.f << 29.3024608000000f << endr
            << 390.f << 29.8404010000000f << endr
            << 391.f << 31.7823810000000f << endr
            << 392.f << 33.7243610000000f << endr
            << 393.f << 35.6663410000000f << endr
            << 394.f << 37.6083210000000f << endr
            << 395.f << 39.5503010000000f << endr
            << 396.f << 41.5730410000000f << endr
            << 397.f << 43.5957810000000f << endr
            << 398.f << 45.6185210000000f << endr
            << 399.f << 47.6412610000000f << endr
            << 400.f << 49.6640010000000f << endr
            << 401.f << 50.3624004000000f << endr
            << 402.f << 51.0607998000000f << endr
            << 403.f << 51.7591992000000f << endr
            << 404.f << 52.4575986000000f << endr
            << 405.f << 53.1559980000000f << endr
            << 406.f << 53.8543982000000f << endr
            << 407.f << 54.5527984000000f << endr
            << 408.f << 55.2511986000000f << endr
            << 409.f << 55.9495988000000f << endr
            << 410.f << 56.6479990000000f << endr
            << 411.f << 57.0075990000000f << endr
            << 412.f << 57.3671990000000f << endr
            << 413.f << 57.7267990000000f << endr
            << 414.f << 58.0863990000000f << endr
            << 415.f << 58.4459990000000f << endr
            << 416.f << 58.8055990000000f << endr
            << 417.f << 59.1651990000000f << endr
            << 418.f << 59.5247990000000f << endr
            << 419.f << 59.8843990000000f << endr
            << 420.f << 60.2439990000000f << endr
            << 421.f << 60.0411992000000f << endr
            << 422.f << 59.8383994000000f << endr
            << 423.f << 59.6355996000000f << endr
            << 424.f << 59.4327998000000f << endr
            << 425.f << 59.2300000000000f << endr
            << 426.f << 59.0272000000000f << endr
            << 427.f << 58.8244000000000f << endr
            << 428.f << 58.6216000000000f << endr
            << 429.f << 58.4188000000000f << endr
            << 430.f << 58.2160000000000f << endr
            << 431.f << 59.9675998000000f << endr
            << 432.f << 61.7191996000000f << endr
            << 433.f << 63.4707994000000f << endr
            << 434.f << 65.2223992000000f << endr
            << 435.f << 66.9739990000000f << endr
            << 436.f << 68.7255996000000f << endr
            << 437.f << 70.4772002000000f << endr
            << 438.f << 72.2288008000000f << endr
            << 439.f << 73.9804014000000f << endr
            << 440.f << 75.7320020000000f << endr
            << 441.f << 76.9852018000000f << endr
            << 442.f << 78.2384016000000f << endr
            << 443.f << 79.4916014000000f << endr
            << 444.f << 80.7448012000000f << endr
            << 445.f << 81.9980010000000f << endr
            << 446.f << 83.2512008000000f << endr
            << 447.f << 84.5044006000000f << endr
            << 448.f << 85.7576004000000f << endr
            << 449.f << 87.0108002000000f << endr
            << 450.f << 88.2640000000000f << endr
            << 451.f << 88.5972000000000f << endr
            << 452.f << 88.9304000000000f << endr
            << 453.f << 89.2636000000000f << endr
            << 454.f << 89.5968000000000f << endr
            << 455.f << 89.9300000000000f << endr
            << 456.f << 90.2632002000000f << endr
            << 457.f << 90.5964004000000f << endr
            << 458.f << 90.9296006000000f << endr
            << 459.f << 91.2628008000000f << endr
            << 460.f << 91.5960010000000f << endr
            << 461.f << 91.6648012000000f << endr
            << 462.f << 91.7336014000000f << endr
            << 463.f << 91.8024016000000f << endr
            << 464.f << 91.8712018000000f << endr
            << 465.f << 91.9400020000000f << endr
            << 466.f << 92.0088010000000f << endr
            << 467.f << 92.0776000000000f << endr
            << 468.f << 92.1463990000000f << endr
            << 469.f << 92.2151980000000f << endr
            << 470.f << 92.2839970000000f << endr
            << 471.f << 92.6583972000000f << endr
            << 472.f << 93.0327974000000f << endr
            << 473.f << 93.4071976000000f << endr
            << 474.f << 93.7815978000000f << endr
            << 475.f << 94.1559980000000f << endr
            << 476.f << 94.5303984000000f << endr
            << 477.f << 94.9047988000000f << endr
            << 478.f << 95.2791992000000f << endr
            << 479.f << 95.6535996000000f << endr
            << 480.f << 96.0280000000000f << endr
            << 481.f << 95.6847992000000f << endr
            << 482.f << 95.3415984000000f << endr
            << 483.f << 94.9983976000000f << endr
            << 484.f << 94.6551968000000f << endr
            << 485.f << 94.3119960000000f << endr
            << 486.f << 93.9687970000000f << endr
            << 487.f << 93.6255980000000f << endr
            << 488.f << 93.2823990000000f << endr
            << 489.f << 92.9392000000000f << endr
            << 490.f << 92.5960010000000f << endr
            << 491.f << 92.9616016000000f << endr
            << 492.f << 93.3272022000000f << endr
            << 493.f << 93.6928028000000f << endr
            << 494.f << 94.0584034000000f << endr
            << 495.f << 94.4240040000000f << endr
            << 496.f << 94.7896030000000f << endr
            << 497.f << 95.1552020000000f << endr
            << 498.f << 95.5208010000000f << endr
            << 499.f << 95.8864000000000f << endr
            << 500.f << 96.2519990000000f << endr
            << 501.f << 96.3339998000000f << endr
            << 502.f << 96.4160006000000f << endr
            << 503.f << 96.4980014000000f << endr
            << 504.f << 96.5800022000000f << endr
            << 505.f << 96.6620030000000f << endr
            << 506.f << 96.7440022000000f << endr
            << 507.f << 96.8260014000000f << endr
            << 508.f << 96.9080006000000f << endr
            << 509.f << 96.9899998000000f << endr
            << 510.f << 97.0719990000000f << endr
            << 511.f << 97.1203998000000f << endr
            << 512.f << 97.1688006000000f << endr
            << 513.f << 97.2172014000000f << endr
            << 514.f << 97.2656022000000f << endr
            << 515.f << 97.3140030000000f << endr
            << 516.f << 97.3624024000000f << endr
            << 517.f << 97.4108018000000f << endr
            << 518.f << 97.4592012000000f << endr
            << 519.f << 97.5076006000000f << endr
            << 520.f << 97.5560000000000f << endr
            << 521.f << 98.0459994000000f << endr
            << 522.f << 98.5359988000000f << endr
            << 523.f << 99.0259982000000f << endr
            << 524.f << 99.5159976000000f << endr
            << 525.f << 100.005997000000f << endr
            << 526.f << 100.495997800000f << endr
            << 527.f << 100.985998600000f << endr
            << 528.f << 101.475999400000f << endr
            << 529.f << 101.966000200000f << endr
            << 530.f << 102.456001000000f << endr
            << 531.f << 102.303600800000f << endr
            << 532.f << 102.151200600000f << endr
            << 533.f << 101.998800400000f << endr
            << 534.f << 101.846400200000f << endr
            << 535.f << 101.694000000000f << endr
            << 536.f << 101.541599800000f << endr
            << 537.f << 101.389199600000f << endr
            << 538.f << 101.236799400000f << endr
            << 539.f << 101.084399200000f << endr
            << 540.f << 100.931999000000f << endr
            << 541.f << 101.081199400000f << endr
            << 542.f << 101.230399800000f << endr
            << 543.f << 101.379600200000f << endr
            << 544.f << 101.528800600000f << endr
            << 545.f << 101.678001000000f << endr
            << 546.f << 101.827201600000f << endr
            << 547.f << 101.976402200000f << endr
            << 548.f << 102.125602800000f << endr
            << 549.f << 102.274803400000f << endr
            << 550.f << 102.424004000000f << endr
            << 551.f << 102.181602800000f << endr
            << 552.f << 101.939201600000f << endr
            << 553.f << 101.696800400000f << endr
            << 554.f << 101.454399200000f << endr
            << 555.f << 101.211998000000f << endr
            << 556.f << 100.969598400000f << endr
            << 557.f << 100.727198800000f << endr
            << 558.f << 100.484799200000f << endr
            << 559.f << 100.242399600000f << endr
            << 560.f << 100.f << endr
            << 561.f << 99.6073394000000f << endr
            << 562.f << 99.2146788000000f << endr
            << 563.f << 98.8220182000000f << endr
            << 564.f << 98.4293576000000f << endr
            << 565.f << 98.0366970000000f << endr
            << 566.f << 97.6440380000000f << endr
            << 567.f << 97.2513790000000f << endr
            << 568.f << 96.8587200000000f << endr
            << 569.f << 96.4660610000000f << endr
            << 570.f << 96.0734020000000f << endr
            << 571.f << 95.9944012000000f << endr
            << 572.f << 95.9154004000000f << endr
            << 573.f << 95.8363996000000f << endr
            << 574.f << 95.7573988000000f << endr
            << 575.f << 95.6783980000000f << endr
            << 576.f << 95.5994186000000f << endr
            << 577.f << 95.5204392000000f << endr
            << 578.f << 95.4414598000000f << endr
            << 579.f << 95.3624804000000f << endr
            << 580.f << 95.2835010000000f << endr
            << 581.f << 94.7422214000000f << endr
            << 582.f << 94.2009418000000f << endr
            << 583.f << 93.6596622000000f << endr
            << 584.f << 93.1183826000000f << endr
            << 585.f << 92.5771030000000f << endr
            << 586.f << 92.0358218000000f << endr
            << 587.f << 91.4945406000000f << endr
            << 588.f << 90.9532594000000f << endr
            << 589.f << 90.4119782000000f << endr
            << 590.f << 89.8706970000000f << endr
            << 591.f << 90.0510574000000f << endr
            << 592.f << 90.2314178000000f << endr
            << 593.f << 90.4117782000000f << endr
            << 594.f << 90.5921386000000f << endr
            << 595.f << 90.7724990000000f << endr
            << 596.f << 90.9528792000000f << endr
            << 597.f << 91.1332594000000f << endr
            << 598.f << 91.3136396000000f << endr
            << 599.f << 91.4940198000000f << endr
            << 600.f << 91.6744000000000f << endr
            << 601.f << 91.6874204000000f << endr
            << 602.f << 91.7004408000000f << endr
            << 603.f << 91.7134612000000f << endr
            << 604.f << 91.7264816000000f << endr
            << 605.f << 91.7395020000000f << endr
            << 606.f << 91.7525422000000f << endr
            << 607.f << 91.7655824000000f << endr
            << 608.f << 91.7786226000000f << endr
            << 609.f << 91.7916628000000f << endr
            << 610.f << 91.8047030000000f << endr
            << 611.f << 91.6367220000000f << endr
            << 612.f << 91.4687410000000f << endr
            << 613.f << 91.3007600000000f << endr
            << 614.f << 91.1327790000000f << endr
            << 615.f << 90.9647980000000f << endr
            << 616.f << 90.7968186000000f << endr
            << 617.f << 90.6288392000000f << endr
            << 618.f << 90.4608598000000f << endr
            << 619.f << 90.2928804000000f << endr
            << 620.f << 90.1249010000000f << endr
            << 621.f << 89.6995806000000f << endr
            << 622.f << 89.2742602000000f << endr
            << 623.f << 88.8489398000000f << endr
            << 624.f << 88.4236194000000f << endr
            << 625.f << 87.9982990000000f << endr
            << 626.f << 87.5729784000000f << endr
            << 627.f << 87.1476578000000f << endr
            << 628.f << 86.7223372000000f << endr
            << 629.f << 86.2970166000000f << endr
            << 630.f << 85.8716960000000f << endr
            << 631.f << 86.0404172000000f << endr
            << 632.f << 86.2091384000000f << endr
            << 633.f << 86.3778596000000f << endr
            << 634.f << 86.5465808000000f << endr
            << 635.f << 86.7153020000000f << endr
            << 636.f << 86.8840214000000f << endr
            << 637.f << 87.0527408000000f << endr
            << 638.f << 87.2214602000000f << endr
            << 639.f << 87.3901796000000f << endr
            << 640.f << 87.5588990000000f << endr
            << 641.f << 87.2609192000000f << endr
            << 642.f << 86.9629394000000f << endr
            << 643.f << 86.6649596000000f << endr
            << 644.f << 86.3669798000000f << endr
            << 645.f << 86.0690000000000f << endr
            << 646.f << 85.7710204000000f << endr
            << 647.f << 85.4730408000000f << endr
            << 648.f << 85.1750612000000f << endr
            << 649.f << 84.8770816000000f << endr
            << 650.f << 84.5791020000000f << endr
            << 651.f << 84.6968022000000f << endr
            << 652.f << 84.8145024000000f << endr
            << 653.f << 84.9322026000000f << endr
            << 654.f << 85.0499028000000f << endr
            << 655.f << 85.1676030000000f << endr
            << 656.f << 85.2853230000000f << endr
            << 657.f << 85.4030430000000f << endr
            << 658.f << 85.5207630000000f << endr
            << 659.f << 85.6384830000000f << endr
            << 660.f << 85.7562030000000f << endr
            << 661.f << 86.0302432000000f << endr
            << 662.f << 86.3042834000000f << endr
            << 663.f << 86.5783236000000f << endr
            << 664.f << 86.8523638000000f << endr
            << 665.f << 87.1264040000000f << endr
            << 666.f << 87.4004426000000f << endr
            << 667.f << 87.6744812000000f << endr
            << 668.f << 87.9485198000000f << endr
            << 669.f << 88.2225584000000f << endr
            << 670.f << 88.4965970000000f << endr
            << 671.f << 88.1512770000000f << endr
            << 672.f << 87.8059570000000f << endr
            << 673.f << 87.4606370000000f << endr
            << 674.f << 87.1153170000000f << endr
            << 675.f << 86.7699970000000f << endr
            << 676.f << 86.4246784000000f << endr
            << 677.f << 86.0793598000000f << endr
            << 678.f << 85.7340412000000f << endr
            << 679.f << 85.3887226000000f << endr
            << 680.f << 85.0434040000000f << endr
            << 681.f << 84.0336628000000f << endr
            << 682.f << 83.0239216000000f << endr
            << 683.f << 82.0141804000000f << endr
            << 684.f << 81.0044392000000f << endr
            << 685.f << 79.9946980000000f << endr
            << 686.f << 78.9849780000000f << endr
            << 687.f << 77.9752580000000f << endr
            << 688.f << 76.9655380000000f << endr
            << 689.f << 75.9558180000000f << endr
            << 690.f << 74.9460980000000f << endr
            << 691.f << 75.2337980000000f << endr
            << 692.f << 75.5214980000000f << endr
            << 693.f << 75.8091980000000f << endr
            << 694.f << 76.0968980000000f << endr
            << 695.f << 76.3845980000000f << endr
            << 696.f << 76.6723176000000f << endr
            << 697.f << 76.9600372000000f << endr
            << 698.f << 77.2477568000000f << endr
            << 699.f << 77.5354764000000f << endr
            << 700.f << 77.8231960000000f << endr
            << 701.f << 77.9928174000000f << endr
            << 702.f << 78.1624388000000f << endr
            << 703.f << 78.3320602000000f << endr
            << 704.f << 78.5016816000000f << endr
            << 705.f << 78.6713030000000f << endr
            << 706.f << 78.8409426000000f << endr
            << 707.f << 79.0105822000000f << endr
            << 708.f << 79.1802218000000f << endr
            << 709.f << 79.3498614000000f << endr
            << 710.f << 79.5195010000000f << endr
            << 711.f << 78.1544406000000f << endr
            << 712.f << 76.7893802000000f << endr
            << 713.f << 75.4243198000000f << endr
            << 714.f << 74.0592594000000f << endr
            << 715.f << 72.6941990000000f << endr
            << 716.f << 71.3291598000000f << endr
            << 717.f << 69.9641206000000f << endr
            << 718.f << 68.5990814000000f << endr
            << 719.f << 67.2340422000000f << endr
            << 720.f << 65.8690030000000f << endr
            << 721.f << 66.7310224000000f << endr
            << 722.f << 67.5930418000000f << endr
            << 723.f << 68.4550612000000f << endr
            << 724.f << 69.3170806000000f << endr
            << 725.f << 70.1791000000000f << endr
            << 726.f << 71.0411194000000f << endr
            << 727.f << 71.9031388000000f << endr
            << 728.f << 72.7651582000000f << endr
            << 729.f << 73.6271776000000f << endr
            << 730.f << 74.4891970000000f << endr;

        lambda = illumin.col(0);
        LFcn = illumin.col(1);
        break;
      }
    case 1: // d50
      {
        fmat::fixed<351,2> illumin;
        illumin << 380.f << 49.9754980000000f << endr
            << 381.f << 50.4427580000000f << endr
            << 382.f << 50.9100180000000f << endr
            << 383.f << 51.3772780000000f << endr
            << 384.f << 51.8445380000000f << endr
            << 385.f << 52.3117980000000f << endr
            << 386.f << 52.7790786000000f << endr
            << 387.f << 53.2463592000000f << endr
            << 388.f << 53.7136398000000f << endr
            << 389.f << 54.1809204000000f << endr
            << 390.f << 54.6482010000000f << endr
            << 391.f << 57.4588608000000f << endr
            << 392.f << 60.2695206000000f << endr
            << 393.f << 63.0801804000000f << endr
            << 394.f << 65.8908402000000f << endr
            << 395.f << 68.7015000000000f << endr
            << 396.f << 71.5121796000000f << endr
            << 397.f << 74.3228592000000f << endr
            << 398.f << 77.1335388000000f << endr
            << 399.f << 79.9442184000000f << endr
            << 400.f << 82.7548980000000f << endr
            << 401.f << 83.6279982000000f << endr
            << 402.f << 84.5010984000000f << endr
            << 403.f << 85.3741986000000f << endr
            << 404.f << 86.2472988000000f << endr
            << 405.f << 87.1203990000000f << endr
            << 406.f << 87.9935192000000f << endr
            << 407.f << 88.8666394000000f << endr
            << 408.f << 89.7397596000000f << endr
            << 409.f << 90.6128798000000f << endr
            << 410.f << 91.4860000000000f << endr
            << 411.f << 91.6805800000000f << endr
            << 412.f << 91.8751600000000f << endr
            << 413.f << 92.0697400000000f << endr
            << 414.f << 92.2643200000000f << endr
            << 415.f << 92.4589000000000f << endr
            << 416.f << 92.6534802000000f << endr
            << 417.f << 92.8480604000000f << endr
            << 418.f << 93.0426406000000f << endr
            << 419.f << 93.2372208000000f << endr
            << 420.f << 93.4318010000000f << endr
            << 421.f << 92.7568406000000f << endr
            << 422.f << 92.0818802000000f << endr
            << 423.f << 91.4069198000000f << endr
            << 424.f << 90.7319594000000f << endr
            << 425.f << 90.0569990000000f << endr
            << 426.f << 89.3820586000000f << endr
            << 427.f << 88.7071182000000f << endr
            << 428.f << 88.0321778000000f << endr
            << 429.f << 87.3572374000000f << endr
            << 430.f << 86.6822970000000f << endr
            << 431.f << 88.5005572000000f << endr
            << 432.f << 90.3188174000000f << endr
            << 433.f << 92.1370776000000f << endr
            << 434.f << 93.9553378000000f << endr
            << 435.f << 95.7735980000000f << endr
            << 436.f << 97.5918780000000f << endr
            << 437.f << 99.4101580000000f << endr
            << 438.f << 101.228438000000f << endr
            << 439.f << 103.046718000000f << endr
            << 440.f << 104.864998000000f << endr
            << 441.f << 106.079197800000f << endr
            << 442.f << 107.293397600000f << endr
            << 443.f << 108.507597400000f << endr
            << 444.f << 109.721797200000f << endr
            << 445.f << 110.935997000000f << endr
            << 446.f << 112.150398200000f << endr
            << 447.f << 113.364799400000f << endr
            << 448.f << 114.579200600000f << endr
            << 449.f << 115.793601800000f << endr
            << 450.f << 117.008003000000f << endr
            << 451.f << 117.088403200000f << endr
            << 452.f << 117.168803400000f << endr
            << 453.f << 117.249203600000f << endr
            << 454.f << 117.329603800000f << endr
            << 455.f << 117.410004000000f << endr
            << 456.f << 117.490402400000f << endr
            << 457.f << 117.570800800000f << endr
            << 458.f << 117.651199200000f << endr
            << 459.f << 117.731597600000f << endr
            << 460.f << 117.811996000000f << endr
            << 461.f << 117.516796600000f << endr
            << 462.f << 117.221597200000f << endr
            << 463.f << 116.926397800000f << endr
            << 464.f << 116.631198400000f << endr
            << 465.f << 116.335999000000f << endr
            << 466.f << 116.040999200000f << endr
            << 467.f << 115.745999400000f << endr
            << 468.f << 115.450999600000f << endr
            << 469.f << 115.155999800000f << endr
            << 470.f << 114.861000000000f << endr
            << 471.f << 114.967199600000f << endr
            << 472.f << 115.073399200000f << endr
            << 473.f << 115.179598800000f << endr
            << 474.f << 115.285798400000f << endr
            << 475.f << 115.391998000000f << endr
            << 476.f << 115.498197800000f << endr
            << 477.f << 115.604397600000f << endr
            << 478.f << 115.710597400000f << endr
            << 479.f << 115.816797200000f << endr
            << 480.f << 115.922997000000f << endr
            << 481.f << 115.211797000000f << endr
            << 482.f << 114.500597000000f << endr
            << 483.f << 113.789397000000f << endr
            << 484.f << 113.078197000000f << endr
            << 485.f << 112.366997000000f << endr
            << 486.f << 111.655797000000f << endr
            << 487.f << 110.944597000000f << endr
            << 488.f << 110.233397000000f << endr
            << 489.f << 109.522197000000f << endr
            << 490.f << 108.810997000000f << endr
            << 491.f << 108.865197800000f << endr
            << 492.f << 108.919398600000f << endr
            << 493.f << 108.973599400000f << endr
            << 494.f << 109.027800200000f << endr
            << 495.f << 109.082001000000f << endr
            << 496.f << 109.136400000000f << endr
            << 497.f << 109.190799000000f << endr
            << 498.f << 109.245198000000f << endr
            << 499.f << 109.299597000000f << endr
            << 500.f << 109.353996000000f << endr
            << 501.f << 109.198797400000f << endr
            << 502.f << 109.043598800000f << endr
            << 503.f << 108.888400200000f << endr
            << 504.f << 108.733201600000f << endr
            << 505.f << 108.578003000000f << endr
            << 506.f << 108.422802800000f << endr
            << 507.f << 108.267602600000f << endr
            << 508.f << 108.112402400000f << endr
            << 509.f << 107.957202200000f << endr
            << 510.f << 107.802002000000f << endr
            << 511.f << 107.500801200000f << endr
            << 512.f << 107.199600400000f << endr
            << 513.f << 106.898399600000f << endr
            << 514.f << 106.597198800000f << endr
            << 515.f << 106.295998000000f << endr
            << 516.f << 105.994798600000f << endr
            << 517.f << 105.693599200000f << endr
            << 518.f << 105.392399800000f << endr
            << 519.f << 105.091200400000f << endr
            << 520.f << 104.790001000000f << endr
            << 521.f << 105.079800400000f << endr
            << 522.f << 105.369599800000f << endr
            << 523.f << 105.659399200000f << endr
            << 524.f << 105.949198600000f << endr
            << 525.f << 106.238998000000f << endr
            << 526.f << 106.528999000000f << endr
            << 527.f << 106.819000000000f << endr
            << 528.f << 107.109001000000f << endr
            << 529.f << 107.399002000000f << endr
            << 530.f << 107.689003000000f << endr
            << 531.f << 107.360601800000f << endr
            << 532.f << 107.032200600000f << endr
            << 533.f << 106.703799400000f << endr
            << 534.f << 106.375398200000f << endr
            << 535.f << 106.046997000000f << endr
            << 536.f << 105.718597400000f << endr
            << 537.f << 105.390197800000f << endr
            << 538.f << 105.061798200000f << endr
            << 539.f << 104.733398600000f << endr
            << 540.f << 104.404999000000f << endr
            << 541.f << 104.368998800000f << endr
            << 542.f << 104.332998600000f << endr
            << 543.f << 104.296998400000f << endr
            << 544.f << 104.260998200000f << endr
            << 545.f << 104.224998000000f << endr
            << 546.f << 104.189198000000f << endr
            << 547.f << 104.153398000000f << endr
            << 548.f << 104.117598000000f << endr
            << 549.f << 104.081798000000f << endr
            << 550.f << 104.045998000000f << endr
            << 551.f << 103.641399000000f << endr
            << 552.f << 103.236800000000f << endr
            << 553.f << 102.832201000000f << endr
            << 554.f << 102.427602000000f << endr
            << 555.f << 102.023003000000f << endr
            << 556.f << 101.618402400000f << endr
            << 557.f << 101.213801800000f << endr
            << 558.f << 100.809201200000f << endr
            << 559.f << 100.404600600000f << endr
            << 560.f << 100.f << endr
            << 561.f << 99.6334198000000f << endr
            << 562.f << 99.2668396000000f << endr
            << 563.f << 98.9002594000000f << endr
            << 564.f << 98.5336792000000f << endr
            << 565.f << 98.1670990000000f << endr
            << 566.f << 97.8005188000000f << endr
            << 567.f << 97.4339386000000f << endr
            << 568.f << 97.0673584000000f << endr
            << 569.f << 96.7007782000000f << endr
            << 570.f << 96.3341980000000f << endr
            << 571.f << 96.2795776000000f << endr
            << 572.f << 96.2249572000000f << endr
            << 573.f << 96.1703368000000f << endr
            << 574.f << 96.1157164000000f << endr
            << 575.f << 96.0610960000000f << endr
            << 576.f << 96.0064772000000f << endr
            << 577.f << 95.9518584000000f << endr
            << 578.f << 95.8972396000000f << endr
            << 579.f << 95.8426208000000f << endr
            << 580.f << 95.7880020000000f << endr
            << 581.f << 95.0777618000000f << endr
            << 582.f << 94.3675216000000f << endr
            << 583.f << 93.6572814000000f << endr
            << 584.f << 92.9470412000000f << endr
            << 585.f << 92.2368010000000f << endr
            << 586.f << 91.5265608000000f << endr
            << 587.f << 90.8163206000000f << endr
            << 588.f << 90.1060804000000f << endr
            << 589.f << 89.3958402000000f << endr
            << 590.f << 88.6856000000000f << endr
            << 591.f << 88.8176602000000f << endr
            << 592.f << 88.9497204000000f << endr
            << 593.f << 89.0817806000000f << endr
            << 594.f << 89.2138408000000f << endr
            << 595.f << 89.3459010000000f << endr
            << 596.f << 89.4779614000000f << endr
            << 597.f << 89.6100218000000f << endr
            << 598.f << 89.7420822000000f << endr
            << 599.f << 89.8741426000000f << endr
            << 600.f << 90.0062030000000f << endr
            << 601.f << 89.9654818000000f << endr
            << 602.f << 89.9247606000000f << endr
            << 603.f << 89.8840394000000f << endr
            << 604.f << 89.8433182000000f << endr
            << 605.f << 89.8025970000000f << endr
            << 606.f << 89.7618972000000f << endr
            << 607.f << 89.7211974000000f << endr
            << 608.f << 89.6804976000000f << endr
            << 609.f << 89.6397978000000f << endr
            << 610.f << 89.5990980000000f << endr
            << 611.f << 89.4090590000000f << endr
            << 612.f << 89.2190200000000f << endr
            << 613.f << 89.0289810000000f << endr
            << 614.f << 88.8389420000000f << endr
            << 615.f << 88.6489030000000f << endr
            << 616.f << 88.4588624000000f << endr
            << 617.f << 88.2688218000000f << endr
            << 618.f << 88.0787812000000f << endr
            << 619.f << 87.8887406000000f << endr
            << 620.f << 87.6987000000000f << endr
            << 621.f << 87.2576798000000f << endr
            << 622.f << 86.8166596000000f << endr
            << 623.f << 86.3756394000000f << endr
            << 624.f << 85.9346192000000f << endr
            << 625.f << 85.4935990000000f << endr
            << 626.f << 85.0525986000000f << endr
            << 627.f << 84.6115982000000f << endr
            << 628.f << 84.1705978000000f << endr
            << 629.f << 83.7295974000000f << endr
            << 630.f << 83.2885970000000f << endr
            << 631.f << 83.3296568000000f << endr
            << 632.f << 83.3707166000000f << endr
            << 633.f << 83.4117764000000f << endr
            << 634.f << 83.4528362000000f << endr
            << 635.f << 83.4938960000000f << endr
            << 636.f << 83.5349574000000f << endr
            << 637.f << 83.5760188000000f << endr
            << 638.f << 83.6170802000000f << endr
            << 639.f << 83.6581416000000f << endr
            << 640.f << 83.6992030000000f << endr
            << 641.f << 83.3319622000000f << endr
            << 642.f << 82.9647214000000f << endr
            << 643.f << 82.5974806000000f << endr
            << 644.f << 82.2302398000000f << endr
            << 645.f << 81.8629990000000f << endr
            << 646.f << 81.4957596000000f << endr
            << 647.f << 81.1285202000000f << endr
            << 648.f << 80.7612808000000f << endr
            << 649.f << 80.3940414000000f << endr
            << 650.f << 80.0268020000000f << endr
            << 651.f << 80.0455810000000f << endr
            << 652.f << 80.0643600000000f << endr
            << 653.f << 80.0831390000000f << endr
            << 654.f << 80.1019180000000f << endr
            << 655.f << 80.1206970000000f << endr
            << 656.f << 80.1394776000000f << endr
            << 657.f << 80.1582582000000f << endr
            << 658.f << 80.1770388000000f << endr
            << 659.f << 80.1958194000000f << endr
            << 660.f << 80.2146000000000f << endr
            << 661.f << 80.4209202000000f << endr
            << 662.f << 80.6272404000000f << endr
            << 663.f << 80.8335606000000f << endr
            << 664.f << 81.0398808000000f << endr
            << 665.f << 81.2462010000000f << endr
            << 666.f << 81.4525212000000f << endr
            << 667.f << 81.6588414000000f << endr
            << 668.f << 81.8651616000000f << endr
            << 669.f << 82.0714818000000f << endr
            << 670.f << 82.2778020000000f << endr
            << 671.f << 81.8784412000000f << endr
            << 672.f << 81.4790804000000f << endr
            << 673.f << 81.0797196000000f << endr
            << 674.f << 80.6803588000000f << endr
            << 675.f << 80.2809980000000f << endr
            << 676.f << 79.8816390000000f << endr
            << 677.f << 79.4822800000000f << endr
            << 678.f << 79.0829210000000f << endr
            << 679.f << 78.6835620000000f << endr
            << 680.f << 78.2842030000000f << endr
            << 681.f << 77.4279026000000f << endr
            << 682.f << 76.5716022000000f << endr
            << 683.f << 75.7153018000000f << endr
            << 684.f << 74.8590014000000f << endr
            << 685.f << 74.0027010000000f << endr
            << 686.f << 73.1464204000000f << endr
            << 687.f << 72.2901398000000f << endr
            << 688.f << 71.4338592000000f << endr
            << 689.f << 70.5775786000000f << endr
            << 690.f << 69.7212980000000f << endr
            << 691.f << 69.9100782000000f << endr
            << 692.f << 70.0988584000000f << endr
            << 693.f << 70.2876386000000f << endr
            << 694.f << 70.4764188000000f << endr
            << 695.f << 70.6651990000000f << endr
            << 696.f << 70.8539792000000f << endr
            << 697.f << 71.0427594000000f << endr
            << 698.f << 71.2315396000000f << endr
            << 699.f << 71.4203198000000f << endr
            << 700.f << 71.6091000000000f << endr
            << 701.f << 71.8830792000000f << endr
            << 702.f << 72.1570584000000f << endr
            << 703.f << 72.4310376000000f << endr
            << 704.f << 72.7050168000000f << endr
            << 705.f << 72.9789960000000f << endr
            << 706.f << 73.2529966000000f << endr
            << 707.f << 73.5269972000000f << endr
            << 708.f << 73.8009978000000f << endr
            << 709.f << 74.0749984000000f << endr
            << 710.f << 74.3489990000000f << endr
            << 711.f << 73.0744994000000f << endr
            << 712.f << 71.7999998000000f << endr
            << 713.f << 70.5255002000000f << endr
            << 714.f << 69.2510006000000f << endr
            << 715.f << 67.9765010000000f << endr
            << 716.f << 66.7020008000000f << endr
            << 717.f << 65.4275006000000f << endr
            << 718.f << 64.1530004000000f << endr
            << 719.f << 62.8785002000000f << endr
            << 720.f << 61.6040000000000f << endr
            << 721.f << 62.4321594000000f << endr
            << 722.f << 63.2603188000000f << endr
            << 723.f << 64.0884782000000f << endr
            << 724.f << 64.9166376000000f << endr
            << 725.f << 65.7447970000000f << endr
            << 726.f << 66.5729570000000f << endr
            << 727.f << 67.4011170000000f << endr
            << 728.f << 68.2292770000000f << endr
            << 729.f << 69.0574370000000f << endr
            << 730.f << 69.8855970000000f << endr;

        lambda = illumin.col(0);
        LFcn = illumin.col(1);
        break;
      }
    case 2: // a
      {
        fmat::fixed<351,2> illumin;
        illumin << 380.f << 9.79510000000000f << endr
            << 381.f << 10.0160000000000f << endr
            << 382.f << 10.2369000000000f << endr
            << 383.f << 10.4578000000000f << endr
            << 384.f << 10.6787000000000f << endr
            << 385.f << 10.8996000000000f << endr
            << 386.f << 11.1367400000000f << endr
            << 387.f << 11.3738800000000f << endr
            << 388.f << 11.6110200000000f << endr
            << 389.f << 11.8481600000000f << endr
            << 390.f << 12.0853000000000f << endr
            << 391.f << 12.3391000000000f << endr
            << 392.f << 12.5929000000000f << endr
            << 393.f << 12.8467000000000f << endr
            << 394.f << 13.1005000000000f << endr
            << 395.f << 13.3543000000000f << endr
            << 396.f << 13.6250400000000f << endr
            << 397.f << 13.8957800000000f << endr
            << 398.f << 14.1665200000000f << endr
            << 399.f << 14.4372600000000f << endr
            << 400.f << 14.7080000000000f << endr
            << 401.f << 14.9960002000000f << endr
            << 402.f << 15.2840004000000f << endr
            << 403.f << 15.5720006000000f << endr
            << 404.f << 15.8600008000000f << endr
            << 405.f << 16.1480010000000f << endr
            << 406.f << 16.4534610000000f << endr
            << 407.f << 16.7589210000000f << endr
            << 408.f << 17.0643810000000f << endr
            << 409.f << 17.3698410000000f << endr
            << 410.f << 17.6753010000000f << endr
            << 411.f << 17.9983810000000f << endr
            << 412.f << 18.3214610000000f << endr
            << 413.f << 18.6445410000000f << endr
            << 414.f << 18.9676210000000f << endr
            << 415.f << 19.2907010000000f << endr
            << 416.f << 19.6315610000000f << endr
            << 417.f << 19.9724210000000f << endr
            << 418.f << 20.3132810000000f << endr
            << 419.f << 20.6541410000000f << endr
            << 420.f << 20.9950010000000f << endr
            << 421.f << 21.3536608000000f << endr
            << 422.f << 21.7123206000000f << endr
            << 423.f << 22.0709804000000f << endr
            << 424.f << 22.4296402000000f << endr
            << 425.f << 22.7883000000000f << endr
            << 426.f << 23.1648200000000f << endr
            << 427.f << 23.5413400000000f << endr
            << 428.f << 23.9178600000000f << endr
            << 429.f << 24.2943800000000f << endr
            << 430.f << 24.6709000000000f << endr
            << 431.f << 25.0652200000000f << endr
            << 432.f << 25.4595400000000f << endr
            << 433.f << 25.8538600000000f << endr
            << 434.f << 26.2481800000000f << endr
            << 435.f << 26.6425000000000f << endr
            << 436.f << 27.0545400000000f << endr
            << 437.f << 27.4665800000000f << endr
            << 438.f << 27.8786200000000f << endr
            << 439.f << 28.2906600000000f << endr
            << 440.f << 28.7027000000000f << endr
            << 441.f << 29.1323200000000f << endr
            << 442.f << 29.5619400000000f << endr
            << 443.f << 29.9915600000000f << endr
            << 444.f << 30.4211800000000f << endr
            << 445.f << 30.8508000000000f << endr
            << 446.f << 31.2978198000000f << endr
            << 447.f << 31.7448396000000f << endr
            << 448.f << 32.1918594000000f << endr
            << 449.f << 32.6388792000000f << endr
            << 450.f << 33.0858990000000f << endr
            << 451.f << 33.5500790000000f << endr
            << 452.f << 34.0142590000000f << endr
            << 453.f << 34.4784390000000f << endr
            << 454.f << 34.9426190000000f << endr
            << 455.f << 35.4067990000000f << endr
            << 456.f << 35.8878590000000f << endr
            << 457.f << 36.3689190000000f << endr
            << 458.f << 36.8499790000000f << endr
            << 459.f << 37.3310390000000f << endr
            << 460.f << 37.8120990000000f << endr
            << 461.f << 38.3097194000000f << endr
            << 462.f << 38.8073398000000f << endr
            << 463.f << 39.3049602000000f << endr
            << 464.f << 39.8025806000000f << endr
            << 465.f << 40.3002010000000f << endr
            << 466.f << 40.8140210000000f << endr
            << 467.f << 41.3278410000000f << endr
            << 468.f << 41.8416610000000f << endr
            << 469.f << 42.3554810000000f << endr
            << 470.f << 42.8693010000000f << endr
            << 471.f << 43.3989206000000f << endr
            << 472.f << 43.9285402000000f << endr
            << 473.f << 44.4581598000000f << endr
            << 474.f << 44.9877794000000f << endr
            << 475.f << 45.5173990000000f << endr
            << 476.f << 46.0623788000000f << endr
            << 477.f << 46.6073586000000f << endr
            << 478.f << 47.1523384000000f << endr
            << 479.f << 47.6973182000000f << endr
            << 480.f << 48.2422980000000f << endr
            << 481.f << 48.8021986000000f << endr
            << 482.f << 49.3620992000000f << endr
            << 483.f << 49.9219998000000f << endr
            << 484.f << 50.4819004000000f << endr
            << 485.f << 51.0418010000000f << endr
            << 486.f << 51.6160808000000f << endr
            << 487.f << 52.1903606000000f << endr
            << 488.f << 52.7646404000000f << endr
            << 489.f << 53.3389202000000f << endr
            << 490.f << 53.9132000000000f << endr
            << 491.f << 54.5013402000000f << endr
            << 492.f << 55.0894804000000f << endr
            << 493.f << 55.6776206000000f << endr
            << 494.f << 56.2657608000000f << endr
            << 495.f << 56.8539010000000f << endr
            << 496.f << 57.4553406000000f << endr
            << 497.f << 58.0567802000000f << endr
            << 498.f << 58.6582198000000f << endr
            << 499.f << 59.2596594000000f << endr
            << 500.f << 59.8610990000000f << endr
            << 501.f << 60.4752790000000f << endr
            << 502.f << 61.0894590000000f << endr
            << 503.f << 61.7036390000000f << endr
            << 504.f << 62.3178190000000f << endr
            << 505.f << 62.9319990000000f << endr
            << 506.f << 63.5582990000000f << endr
            << 507.f << 64.1845990000000f << endr
            << 508.f << 64.8108990000000f << endr
            << 509.f << 65.4371990000000f << endr
            << 510.f << 66.0634990000000f << endr
            << 511.f << 66.7012996000000f << endr
            << 512.f << 67.3391002000000f << endr
            << 513.f << 67.9769008000000f << endr
            << 514.f << 68.6147014000000f << endr
            << 515.f << 69.2525020000000f << endr
            << 516.f << 69.9011822000000f << endr
            << 517.f << 70.5498624000000f << endr
            << 518.f << 71.1985426000000f << endr
            << 519.f << 71.8472228000000f << endr
            << 520.f << 72.4959030000000f << endr
            << 521.f << 73.1547820000000f << endr
            << 522.f << 73.8136610000000f << endr
            << 523.f << 74.4725400000000f << endr
            << 524.f << 75.1314190000000f << endr
            << 525.f << 75.7902980000000f << endr
            << 526.f << 76.4587582000000f << endr
            << 527.f << 77.1272184000000f << endr
            << 528.f << 77.7956786000000f << endr
            << 529.f << 78.4641388000000f << endr
            << 530.f << 79.1325990000000f << endr
            << 531.f << 79.8099396000000f << endr
            << 532.f << 80.4872802000000f << endr
            << 533.f << 81.1646208000000f << endr
            << 534.f << 81.8419614000000f << endr
            << 535.f << 82.5193020000000f << endr
            << 536.f << 83.2048414000000f << endr
            << 537.f << 83.8903808000000f << endr
            << 538.f << 84.5759202000000f << endr
            << 539.f << 85.2614596000000f << endr
            << 540.f << 85.9469990000000f << endr
            << 541.f << 86.6400790000000f << endr
            << 542.f << 87.3331590000000f << endr
            << 543.f << 88.0262390000000f << endr
            << 544.f << 88.7193190000000f << endr
            << 545.f << 89.4123990000000f << endr
            << 546.f << 90.1123198000000f << endr
            << 547.f << 90.8122406000000f << endr
            << 548.f << 91.5121614000000f << endr
            << 549.f << 92.2120822000000f << endr
            << 550.f << 92.9120030000000f << endr
            << 551.f << 93.6180622000000f << endr
            << 552.f << 94.3241214000000f << endr
            << 553.f << 95.0301806000000f << endr
            << 554.f << 95.7362398000000f << endr
            << 555.f << 96.4422990000000f << endr
            << 556.f << 97.1538392000000f << endr
            << 557.f << 97.8653794000000f << endr
            << 558.f << 98.5769196000000f << endr
            << 559.f << 99.2884598000000f << endr
            << 560.f << 100.f << endr
            << 561.f << 100.716400200000f << endr
            << 562.f << 101.432800400000f << endr
            << 563.f << 102.149200600000f << endr
            << 564.f << 102.865600800000f << endr
            << 565.f << 103.582001000000f << endr
            << 566.f << 104.302400400000f << endr
            << 567.f << 105.022799800000f << endr
            << 568.f << 105.743199200000f << endr
            << 569.f << 106.463598600000f << endr
            << 570.f << 107.183998000000f << endr
            << 571.f << 107.907798600000f << endr
            << 572.f << 108.631599200000f << endr
            << 573.f << 109.355399800000f << endr
            << 574.f << 110.079200400000f << endr
            << 575.f << 110.803001000000f << endr
            << 576.f << 111.529600200000f << endr
            << 577.f << 112.256199400000f << endr
            << 578.f << 112.982798600000f << endr
            << 579.f << 113.709397800000f << endr
            << 580.f << 114.435997000000f << endr
            << 581.f << 115.164798000000f << endr
            << 582.f << 115.893599000000f << endr
            << 583.f << 116.622400000000f << endr
            << 584.f << 117.351201000000f << endr
            << 585.f << 118.080002000000f << endr
            << 586.f << 118.810202200000f << endr
            << 587.f << 119.540402400000f << endr
            << 588.f << 120.270602600000f << endr
            << 589.f << 121.000802800000f << endr
            << 590.f << 121.731003000000f << endr
            << 591.f << 122.462002800000f << endr
            << 592.f << 123.193002600000f << endr
            << 593.f << 123.924002400000f << endr
            << 594.f << 124.655002200000f << endr
            << 595.f << 125.386002000000f << endr
            << 596.f << 126.117401400000f << endr
            << 597.f << 126.848800800000f << endr
            << 598.f << 127.580200200000f << endr
            << 599.f << 128.311599600000f << endr
            << 600.f << 129.042999000000f << endr
            << 601.f << 129.773800400000f << endr
            << 602.f << 130.504601800000f << endr
            << 603.f << 131.235403200000f << endr
            << 604.f << 131.966204600000f << endr
            << 605.f << 132.697006000000f << endr
            << 606.f << 133.426803400000f << endr
            << 607.f << 134.156600800000f << endr
            << 608.f << 134.886398200000f << endr
            << 609.f << 135.616195600000f << endr
            << 610.f << 136.345993000000f << endr
            << 611.f << 137.074395800000f << endr
            << 612.f << 137.802798600000f << endr
            << 613.f << 138.531201400000f << endr
            << 614.f << 139.259604200000f << endr
            << 615.f << 139.988007000000f << endr
            << 616.f << 140.714004800000f << endr
            << 617.f << 141.440002600000f << endr
            << 618.f << 142.166000400000f << endr
            << 619.f << 142.891998200000f << endr
            << 620.f << 143.617996000000f << endr
            << 621.f << 144.341397000000f << endr
            << 622.f << 145.064798000000f << endr
            << 623.f << 145.788199000000f << endr
            << 624.f << 146.511600000000f << endr
            << 625.f << 147.235001000000f << endr
            << 626.f << 147.955200600000f << endr
            << 627.f << 148.675400200000f << endr
            << 628.f << 149.395599800000f << endr
            << 629.f << 150.115799400000f << endr
            << 630.f << 150.835999000000f << endr
            << 631.f << 151.552399000000f << endr
            << 632.f << 152.268799000000f << endr
            << 633.f << 152.985199000000f << endr
            << 634.f << 153.701599000000f << endr
            << 635.f << 154.417999000000f << endr
            << 636.f << 155.130200000000f << endr
            << 637.f << 155.842401000000f << endr
            << 638.f << 156.554602000000f << endr
            << 639.f << 157.266803000000f << endr
            << 640.f << 157.979004000000f << endr
            << 641.f << 158.686404400000f << endr
            << 642.f << 159.393804800000f << endr
            << 643.f << 160.101205200000f << endr
            << 644.f << 160.808605600000f << endr
            << 645.f << 161.516006000000f << endr
            << 646.f << 162.218404800000f << endr
            << 647.f << 162.920803600000f << endr
            << 648.f << 163.623202400000f << endr
            << 649.f << 164.325601200000f << endr
            << 650.f << 165.028000000000f << endr
            << 651.f << 165.724399000000f << endr
            << 652.f << 166.420798000000f << endr
            << 653.f << 167.117197000000f << endr
            << 654.f << 167.813596000000f << endr
            << 655.f << 168.509995000000f << endr
            << 656.f << 169.200595400000f << endr
            << 657.f << 169.891195800000f << endr
            << 658.f << 170.581796200000f << endr
            << 659.f << 171.272396600000f << endr
            << 660.f << 171.962997000000f << endr
            << 661.f << 172.646996800000f << endr
            << 662.f << 173.330996600000f << endr
            << 663.f << 174.014996400000f << endr
            << 664.f << 174.698996200000f << endr
            << 665.f << 175.382996000000f << endr
            << 666.f << 176.060196200000f << endr
            << 667.f << 176.737396400000f << endr
            << 668.f << 177.414596600000f << endr
            << 669.f << 178.091796800000f << endr
            << 670.f << 178.768997000000f << endr
            << 671.f << 179.438796800000f << endr
            << 672.f << 180.108596600000f << endr
            << 673.f << 180.778396400000f << endr
            << 674.f << 181.448196200000f << endr
            << 675.f << 182.117996000000f << endr
            << 676.f << 182.780197000000f << endr
            << 677.f << 183.442398000000f << endr
            << 678.f << 184.104599000000f << endr
            << 679.f << 184.766800000000f << endr
            << 680.f << 185.429001000000f << endr
            << 681.f << 186.083401600000f << endr
            << 682.f << 186.737802200000f << endr
            << 683.f << 187.392202800000f << endr
            << 684.f << 188.046603400000f << endr
            << 685.f << 188.701004000000f << endr
            << 686.f << 189.347003200000f << endr
            << 687.f << 189.993002400000f << endr
            << 688.f << 190.639001600000f << endr
            << 689.f << 191.285000800000f << endr
            << 690.f << 191.931000000000f << endr
            << 691.f << 192.568399200000f << endr
            << 692.f << 193.205798400000f << endr
            << 693.f << 193.843197600000f << endr
            << 694.f << 194.480596800000f << endr
            << 695.f << 195.117996000000f << endr
            << 696.f << 195.746597200000f << endr
            << 697.f << 196.375198400000f << endr
            << 698.f << 197.003799600000f << endr
            << 699.f << 197.632400800000f << endr
            << 700.f << 198.261002000000f << endr
            << 701.f << 198.880600400000f << endr
            << 702.f << 199.500198800000f << endr
            << 703.f << 200.119797200000f << endr
            << 704.f << 200.739395600000f << endr
            << 705.f << 201.358994000000f << endr
            << 706.f << 201.968994600000f << endr
            << 707.f << 202.578995200000f << endr
            << 708.f << 203.188995800000f << endr
            << 709.f << 203.798996400000f << endr
            << 710.f << 204.408997000000f << endr
            << 711.f << 205.009396600000f << endr
            << 712.f << 205.609796200000f << endr
            << 713.f << 206.210195800000f << endr
            << 714.f << 206.810595400000f << endr
            << 715.f << 207.410995000000f << endr
            << 716.f << 208.001797000000f << endr
            << 717.f << 208.592599000000f << endr
            << 718.f << 209.183401000000f << endr
            << 719.f << 209.774203000000f << endr
            << 720.f << 210.365005000000f << endr
            << 721.f << 210.945605000000f << endr
            << 722.f << 211.526205000000f << endr
            << 723.f << 212.106805000000f << endr
            << 724.f << 212.687405000000f << endr
            << 725.f << 213.268005000000f << endr
            << 726.f << 213.838403000000f << endr
            << 727.f << 214.408801000000f << endr
            << 728.f << 214.979199000000f << endr
            << 729.f << 215.549597000000f << endr
            << 730.f << 216.119995000000f << endr;

        lambda = illumin.col(0);
        LFcn = illumin.col(1);
        break;
      }
    default:
      {
        // only executes if function is unrecognized (shouldn't happen)
        cerr << "Error: illuminantFcn:unrecognizedIlluminantFcn: Unrecognized illuminant function.\n";
        break;
      }
  }
  return;
}

//============================================================================================

// mkcolorconvert.cpp:2592: error: C1063: compiler limit : compiler stack overflow => seperated into many sub functions
//============================================================================================
// colorMatchFcn function:
void MKCC::CMF_JUDD_VOS_lambda(fvec &lambda)
{
  fvec::fixed<446> tlambda;
  tlambda     << 380.f << endr
      << 381.f << endr
      << 382.f << endr
      << 383.f << endr
      << 384.f << endr
      << 385.f << endr
      << 386.f << endr
      << 387.f << endr
      << 388.f << endr
      << 389.f << endr
      << 390.f << endr
      << 391.f << endr
      << 392.f << endr
      << 393.f << endr
      << 394.f << endr
      << 395.f << endr
      << 396.f << endr
      << 397.f << endr
      << 398.f << endr
      << 399.f << endr
      << 400.f << endr
      << 401.f << endr
      << 402.f << endr
      << 403.f << endr
      << 404.f << endr
      << 405.f << endr
      << 406.f << endr
      << 407.f << endr
      << 408.f << endr
      << 409.f << endr
      << 410.f << endr
      << 411.f << endr
      << 412.f << endr
      << 413.f << endr
      << 414.f << endr
      << 415.f << endr
      << 416.f << endr
      << 417.f << endr
      << 418.f << endr
      << 419.f << endr
      << 420.f << endr
      << 421.f << endr
      << 422.f << endr
      << 423.f << endr
      << 424.f << endr
      << 425.f << endr
      << 426.f << endr
      << 427.f << endr
      << 428.f << endr
      << 429.f << endr
      << 430.f << endr
      << 431.f << endr
      << 432.f << endr
      << 433.f << endr
      << 434.f << endr
      << 435.f << endr
      << 436.f << endr
      << 437.f << endr
      << 438.f << endr
      << 439.f << endr
      << 440.f << endr
      << 441.f << endr
      << 442.f << endr
      << 443.f << endr
      << 444.f << endr
      << 445.f << endr
      << 446.f << endr
      << 447.f << endr
      << 448.f << endr
      << 449.f << endr
      << 450.f << endr
      << 451.f << endr
      << 452.f << endr
      << 453.f << endr
      << 454.f << endr
      << 455.f << endr
      << 456.f << endr
      << 457.f << endr
      << 458.f << endr
      << 459.f << endr
      << 460.f << endr
      << 461.f << endr
      << 462.f << endr
      << 463.f << endr
      << 464.f << endr
      << 465.f << endr
      << 466.f << endr
      << 467.f << endr
      << 468.f << endr
      << 469.f << endr
      << 470.f << endr
      << 471.f << endr
      << 472.f << endr
      << 473.f << endr
      << 474.f << endr
      << 475.f << endr
      << 476.f << endr
      << 477.f << endr
      << 478.f << endr
      << 479.f << endr
      << 480.f << endr
      << 481.f << endr
      << 482.f << endr
      << 483.f << endr
      << 484.f << endr
      << 485.f << endr
      << 486.f << endr
      << 487.f << endr
      << 488.f << endr
      << 489.f << endr
      << 490.f << endr
      << 491.f << endr
      << 492.f << endr
      << 493.f << endr
      << 494.f << endr
      << 495.f << endr
      << 496.f << endr
      << 497.f << endr
      << 498.f << endr
      << 499.f << endr
      << 500.f << endr
      << 501.f << endr
      << 502.f << endr
      << 503.f << endr
      << 504.f << endr
      << 505.f << endr
      << 506.f << endr
      << 507.f << endr
      << 508.f << endr
      << 509.f << endr
      << 510.f << endr
      << 511.f << endr
      << 512.f << endr
      << 513.f << endr
      << 514.f << endr
      << 515.f << endr
      << 516.f << endr
      << 517.f << endr
      << 518.f << endr
      << 519.f << endr
      << 520.f << endr
      << 521.f << endr
      << 522.f << endr
      << 523.f << endr
      << 524.f << endr
      << 525.f << endr
      << 526.f << endr
      << 527.f << endr
      << 528.f << endr
      << 529.f << endr
      << 530.f << endr
      << 531.f << endr
      << 532.f << endr
      << 533.f << endr
      << 534.f << endr
      << 535.f << endr
      << 536.f << endr
      << 537.f << endr
      << 538.f << endr
      << 539.f << endr
      << 540.f << endr
      << 541.f << endr
      << 542.f << endr
      << 543.f << endr
      << 544.f << endr
      << 545.f << endr
      << 546.f << endr
      << 547.f << endr
      << 548.f << endr
      << 549.f << endr
      << 550.f << endr
      << 551.f << endr
      << 552.f << endr
      << 553.f << endr
      << 554.f << endr
      << 555.f << endr
      << 556.f << endr
      << 557.f << endr
      << 558.f << endr
      << 559.f << endr
      << 560.f << endr
      << 561.f << endr
      << 562.f << endr
      << 563.f << endr
      << 564.f << endr
      << 565.f << endr
      << 566.f << endr
      << 567.f << endr
      << 568.f << endr
      << 569.f << endr
      << 570.f << endr
      << 571.f << endr
      << 572.f << endr
      << 573.f << endr
      << 574.f << endr
      << 575.f << endr
      << 576.f << endr
      << 577.f << endr
      << 578.f << endr
      << 579.f << endr
      << 580.f << endr
      << 581.f << endr
      << 582.f << endr
      << 583.f << endr
      << 584.f << endr
      << 585.f << endr
      << 586.f << endr
      << 587.f << endr
      << 588.f << endr
      << 589.f << endr
      << 590.f << endr
      << 591.f << endr
      << 592.f << endr
      << 593.f << endr
      << 594.f << endr
      << 595.f << endr
      << 596.f << endr
      << 597.f << endr
      << 598.f << endr
      << 599.f << endr
      << 600.f << endr
      << 601.f << endr
      << 602.f << endr
      << 603.f << endr
      << 604.f << endr
      << 605.f << endr
      << 606.f << endr
      << 607.f << endr
      << 608.f << endr
      << 609.f << endr
      << 610.f << endr
      << 611.f << endr
      << 612.f << endr
      << 613.f << endr
      << 614.f << endr
      << 615.f << endr
      << 616.f << endr
      << 617.f << endr
      << 618.f << endr
      << 619.f << endr
      << 620.f << endr
      << 621.f << endr
      << 622.f << endr
      << 623.f << endr
      << 624.f << endr
      << 625.f << endr
      << 626.f << endr
      << 627.f << endr
      << 628.f << endr
      << 629.f << endr
      << 630.f << endr
      << 631.f << endr
      << 632.f << endr
      << 633.f << endr
      << 634.f << endr
      << 635.f << endr
      << 636.f << endr
      << 637.f << endr
      << 638.f << endr
      << 639.f << endr
      << 640.f << endr
      << 641.f << endr
      << 642.f << endr
      << 643.f << endr
      << 644.f << endr
      << 645.f << endr
      << 646.f << endr
      << 647.f << endr
      << 648.f << endr
      << 649.f << endr
      << 650.f << endr
      << 651.f << endr
      << 652.f << endr
      << 653.f << endr
      << 654.f << endr
      << 655.f << endr
      << 656.f << endr
      << 657.f << endr
      << 658.f << endr
      << 659.f << endr
      << 660.f << endr
      << 661.f << endr
      << 662.f << endr
      << 663.f << endr
      << 664.f << endr
      << 665.f << endr
      << 666.f << endr
      << 667.f << endr
      << 668.f << endr
      << 669.f << endr
      << 670.f << endr
      << 671.f << endr
      << 672.f << endr
      << 673.f << endr
      << 674.f << endr
      << 675.f << endr
      << 676.f << endr
      << 677.f << endr
      << 678.f << endr
      << 679.f << endr
      << 680.f << endr
      << 681.f << endr
      << 682.f << endr
      << 683.f << endr
      << 684.f << endr
      << 685.f << endr
      << 686.f << endr
      << 687.f << endr
      << 688.f << endr
      << 689.f << endr
      << 690.f << endr
      << 691.f << endr
      << 692.f << endr
      << 693.f << endr
      << 694.f << endr
      << 695.f << endr
      << 696.f << endr
      << 697.f << endr
      << 698.f << endr
      << 699.f << endr
      << 700.f << endr
      << 701.f << endr
      << 702.f << endr
      << 703.f << endr
      << 704.f << endr
      << 705.f << endr
      << 706.f << endr
      << 707.f << endr
      << 708.f << endr
      << 709.f << endr
      << 710.f << endr
      << 711.f << endr
      << 712.f << endr
      << 713.f << endr
      << 714.f << endr
      << 715.f << endr
      << 716.f << endr
      << 717.f << endr
      << 718.f << endr
      << 719.f << endr
      << 720.f << endr
      << 721.f << endr
      << 722.f << endr
      << 723.f << endr
      << 724.f << endr
      << 725.f << endr
      << 726.f << endr
      << 727.f << endr
      << 728.f << endr
      << 729.f << endr
      << 730.f << endr
      << 731.f << endr
      << 732.f << endr
      << 733.f << endr
      << 734.f << endr
      << 735.f << endr
      << 736.f << endr
      << 737.f << endr
      << 738.f << endr
      << 739.f << endr
      << 740.f << endr
      << 741.f << endr
      << 742.f << endr
      << 743.f << endr
      << 744.f << endr
      << 745.f << endr
      << 746.f << endr
      << 747.f << endr
      << 748.f << endr
      << 749.f << endr
      << 750.f << endr
      << 751.f << endr
      << 752.f << endr
      << 753.f << endr
      << 754.f << endr
      << 755.f << endr
      << 756.f << endr
      << 757.f << endr
      << 758.f << endr
      << 759.f << endr
      << 760.f << endr
      << 761.f << endr
      << 762.f << endr
      << 763.f << endr
      << 764.f << endr
      << 765.f << endr
      << 766.f << endr
      << 767.f << endr
      << 768.f << endr
      << 769.f << endr
      << 770.f << endr
      << 771.f << endr
      << 772.f << endr
      << 773.f << endr
      << 774.f << endr
      << 775.f << endr
      << 776.f << endr
      << 777.f << endr
      << 778.f << endr
      << 779.f << endr
      << 780.f << endr
      << 781.f << endr
      << 782.f << endr
      << 783.f << endr
      << 784.f << endr
      << 785.f << endr
      << 786.f << endr
      << 787.f << endr
      << 788.f << endr
      << 789.f << endr
      << 790.f << endr
      << 791.f << endr
      << 792.f << endr
      << 793.f << endr
      << 794.f << endr
      << 795.f << endr
      << 796.f << endr
      << 797.f << endr
      << 798.f << endr
      << 799.f << endr
      << 800.f << endr
      << 801.f << endr
      << 802.f << endr
      << 803.f << endr
      << 804.f << endr
      << 805.f << endr
      << 806.f << endr
      << 807.f << endr
      << 808.f << endr
      << 809.f << endr
      << 810.f << endr
      << 811.f << endr
      << 812.f << endr
      << 813.f << endr
      << 814.f << endr
      << 815.f << endr
      << 816.f << endr
      << 817.f << endr
      << 818.f << endr
      << 819.f << endr
      << 820.f << endr
      << 821.f << endr
      << 822.f << endr
      << 823.f << endr
      << 824.f << endr
      << 825.f << endr;

 lambda =  tlambda;
}

void MKCC::CMF_JUDD_VOS_xFcn(fvec &xFcn)
{
  fvec::fixed<446> txFcn;

txFcn << 0.00268990000000000f  << endr
      << 0.00321402000000000f  << endr
      << 0.00373814000000000f  << endr
      << 0.00426226000000000f  << endr
      << 0.00478638000000000f  << endr
      << 0.00531050000000000f  << endr
      << 0.00640460000000000f  << endr
      << 0.00749870000000000f  << endr
      << 0.00859280000000000f  << endr
      << 0.00968690000000000f  << endr
      << 0.0107810000000000f   << endr
      << 0.0127832000000000f   << endr
      << 0.0147854000000000f   << endr
      << 0.0167876000000000f   << endr
      << 0.0187898000000000f   << endr
      << 0.0207920000000000f   << endr
      << 0.0242298000000000f   << endr
      << 0.0276676000000000f   << endr
      << 0.0311054000000000f   << endr
      << 0.0345432000000000f   << endr
      << 0.0379810000000000f   << endr
      << 0.0430162000000000f   << endr
      << 0.0480514000000000f   << endr
      << 0.0530866000000000f   << endr
      << 0.0581218000000000f   << endr
      << 0.0631570000000000f   << endr
      << 0.0705138000000000f   << endr
      << 0.0778706000000000f   << endr
      << 0.0852274000000000f   << endr
      << 0.0925842000000000f   << endr
      << 0.0999410000000000f   << endr
      << 0.111600800000000f    << endr
      << 0.123260600000000f    << endr
      << 0.134920400000000f    << endr
      << 0.146580200000000f    << endr
      << 0.158240000000000f    << endr
      << 0.172488000000000f    << endr
      << 0.186736000000000f    << endr
      << 0.200984000000000f    << endr
      << 0.215232000000000f    << endr
      << 0.229480000000000f    << endr
      << 0.239800000000000f    << endr
      << 0.250120000000000f    << endr
      << 0.260440000000000f    << endr
      << 0.270760000000000f    << endr
      << 0.281080000000000f    << endr
      << 0.287054000000000f    << endr
      << 0.293028000000000f    << endr
      << 0.299002000000000f    << endr
      << 0.304976000000000f    << endr
      << 0.310950000000000f    << endr
      << 0.314904000000000f    << endr
      << 0.318858000000000f    << endr
      << 0.322812000000000f    << endr
      << 0.326766000000000f    << endr
      << 0.330720000000000f    << endr
      << 0.331248000000000f    << endr
      << 0.331776000000000f    << endr
      << 0.332304000000000f    << endr
      << 0.332832000000000f    << endr
      << 0.333360000000000f    << endr
      << 0.330032000000000f    << endr
      << 0.326704000000000f    << endr
      << 0.323376000000000f    << endr
      << 0.320048000000000f    << endr
      << 0.316720000000000f    << endr
      << 0.311140000000000f    << endr
      << 0.305560000000000f    << endr
      << 0.299980000000000f    << endr
      << 0.294400000000000f    << endr
      << 0.288820000000000f    << endr
      << 0.282994000000000f    << endr
      << 0.277168000000000f    << endr
      << 0.271342000000000f    << endr
      << 0.265516000000000f    << endr
      << 0.259690000000000f    << endr
      << 0.254304000000000f    << endr
      << 0.248918000000000f    << endr
      << 0.243532000000000f    << endr
      << 0.238146000000000f    << endr
      << 0.232760000000000f    << endr
      << 0.228206000000000f    << endr
      << 0.223652000000000f    << endr
      << 0.219098000000000f    << endr
      << 0.214544000000000f    << endr
      << 0.209990000000000f    << endr
      << 0.202944000000000f    << endr
      << 0.195898000000000f    << endr
      << 0.188852000000000f    << endr
      << 0.181806000000000f    << endr
      << 0.174760000000000f    << endr
      << 0.166382000000000f    << endr
      << 0.158004000000000f    << endr
      << 0.149626000000000f    << endr
      << 0.141248000000000f    << endr
      << 0.132870000000000f    << endr
      << 0.124684800000000f    << endr
      << 0.116499600000000f    << endr
      << 0.108314400000000f    << endr
      << 0.100129200000000f    << endr
      << 0.0919440000000000f   << endr
      << 0.0849522000000000f   << endr
      << 0.0779604000000000f   << endr
      << 0.0709686000000000f   << endr
      << 0.0639768000000000f   << endr
      << 0.0569850000000000f   << endr
      << 0.0519342000000000f   << endr
      << 0.0468834000000000f   << endr
      << 0.0418326000000000f   << endr
      << 0.0367818000000000f   << endr
      << 0.0317310000000000f   << endr
      << 0.0283074000000000f   << endr
      << 0.0248838000000000f   << endr
      << 0.0214602000000000f   << endr
      << 0.0180366000000000f   << endr
      << 0.0146130000000000f   << endr
      << 0.0126602200000000f   << endr
      << 0.0107074400000000f   << endr
      << 0.00875466000000000f  << endr
      << 0.00680188000000000f  << endr
      << 0.00484910000000000f  << endr
      << 0.00434358000000000f  << endr
      << 0.00383806000000000f  << endr
      << 0.00333254000000000f  << endr
      << 0.00282702000000000f  << endr
      << 0.00232150000000000f  << endr
      << 0.00371518000000000f  << endr
      << 0.00510886000000000f  << endr
      << 0.00650254000000000f  << endr
      << 0.00789622000000000f  << endr
      << 0.00928990000000000f  << endr
      << 0.0132875200000000f   << endr
      << 0.0172851400000000f   << endr
      << 0.0212827600000000f   << endr
      << 0.0252803800000000f   << endr
      << 0.0292780000000000f   << endr
      << 0.0361806000000000f   << endr
      << 0.0430832000000000f   << endr
      << 0.0499858000000000f   << endr
      << 0.0568884000000000f   << endr
      << 0.0637910000000000f   << endr
      << 0.0731948000000000f   << endr
      << 0.0825986000000000f   << endr
      << 0.0920024000000000f   << endr
      << 0.101406200000000f    << endr
      << 0.110810000000000f    << endr
      << 0.122032000000000f    << endr
      << 0.133254000000000f    << endr
      << 0.144476000000000f    << endr
      << 0.155698000000000f    << endr
      << 0.166920000000000f    << endr
      << 0.179072000000000f    << endr
      << 0.191224000000000f    << endr
      << 0.203376000000000f    << endr
      << 0.215528000000000f    << endr
      << 0.227680000000000f    << endr
      << 0.240682000000000f    << endr
      << 0.253684000000000f    << endr
      << 0.266686000000000f    << endr
      << 0.279688000000000f    << endr
      << 0.292690000000000f    << endr
      << 0.306602000000000f    << endr
      << 0.320514000000000f    << endr
      << 0.334426000000000f    << endr
      << 0.348338000000000f    << endr
      << 0.362250000000000f    << endr
      << 0.377070000000000f    << endr
      << 0.391890000000000f    << endr
      << 0.406710000000000f    << endr
      << 0.421530000000000f    << endr
      << 0.436350000000000f    << endr
      << 0.452106000000000f    << endr
      << 0.467862000000000f    << endr
      << 0.483618000000000f    << endr
      << 0.499374000000000f    << endr
      << 0.515130000000000f    << endr
      << 0.531600000000000f    << endr
      << 0.548070000000000f    << endr
      << 0.564540000000000f    << endr
      << 0.581010000000000f    << endr
      << 0.597480000000000f    << endr
      << 0.614226000000000f    << endr
      << 0.630972000000000f    << endr
      << 0.647718000000000f    << endr
      << 0.664464000000000f    << endr
      << 0.681210000000000f    << endr
      << 0.697818000000000f    << endr
      << 0.714426000000000f    << endr
      << 0.731034000000000f    << endr
      << 0.747642000000000f    << endr
      << 0.764250000000000f    << endr
      << 0.780188000000000f    << endr
      << 0.796126000000000f    << endr
      << 0.812064000000000f    << endr
      << 0.828002000000000f    << endr
      << 0.843940000000000f    << endr
      << 0.858422000000000f    << endr
      << 0.872904000000000f    << endr
      << 0.887386000000000f    << endr
      << 0.901868000000000f    << endr
      << 0.916350000000000f    << endr
      << 0.928486000000000f    << endr
      << 0.940622000000000f    << endr
      << 0.952758000000000f    << endr
      << 0.964894000000000f    << endr
      << 0.977030000000000f    << endr
      << 0.986224000000000f    << endr
      << 0.995418000000000f    << endr
      << 1.00461200000000f     << endr
      << 1.01380600000000f     << endr
      << 1.02300000000000f     << endr
      << 1.02866000000000f     << endr
      << 1.03432000000000f     << endr
      << 1.03998000000000f     << endr
      << 1.04564000000000f     << endr
      << 1.05130000000000f     << endr
      << 1.05204000000000f     << endr
      << 1.05278000000000f     << endr
      << 1.05352000000000f     << endr
      << 1.05426000000000f     << endr
      << 1.05500000000000f     << endr
      << 1.05124000000000f     << endr
      << 1.04748000000000f     << endr
      << 1.04372000000000f     << endr
      << 1.03996000000000f     << endr
      << 1.03620000000000f     << endr
      << 1.02743800000000f     << endr
      << 1.01867600000000f     << endr
      << 1.00991400000000f     << endr
      << 1.00115200000000f     << endr
      << 0.992390000000000f    << endr
      << 0.979634000000000f    << endr
      << 0.966878000000000f    << endr
      << 0.954122000000000f    << endr
      << 0.941366000000000f    << endr
      << 0.928610000000000f    << endr
      << 0.911580000000000f    << endr
      << 0.894550000000000f    << endr
      << 0.877520000000000f    << endr
      << 0.860490000000000f    << endr
      << 0.843460000000000f    << endr
      << 0.822734000000000f    << endr
      << 0.802008000000000f    << endr
      << 0.781282000000000f    << endr
      << 0.760556000000000f    << endr
      << 0.739830000000000f    << endr
      << 0.718442000000000f    << endr
      << 0.697054000000000f    << endr
      << 0.675666000000000f    << endr
      << 0.654278000000000f    << endr
      << 0.632890000000000f    << endr
      << 0.613014000000000f    << endr
      << 0.593138000000000f    << endr
      << 0.573262000000000f    << endr
      << 0.553386000000000f    << endr
      << 0.533510000000000f    << endr
      << 0.514932000000000f    << endr
      << 0.496354000000000f    << endr
      << 0.477776000000000f    << endr
      << 0.459198000000000f    << endr
      << 0.440620000000000f    << endr
      << 0.423402000000000f    << endr
      << 0.406184000000000f    << endr
      << 0.388966000000000f    << endr
      << 0.371748000000000f    << endr
      << 0.354530000000000f    << endr
      << 0.339348000000000f    << endr
      << 0.324166000000000f    << endr
      << 0.308984000000000f    << endr
      << 0.293802000000000f    << endr
      << 0.278620000000000f    << endr
      << 0.265866000000000f    << endr
      << 0.253112000000000f    << endr
      << 0.240358000000000f    << endr
      << 0.227604000000000f    << endr
      << 0.214850000000000f    << endr
      << 0.204202000000000f    << endr
      << 0.193554000000000f    << endr
      << 0.182906000000000f    << endr
      << 0.172258000000000f    << endr
      << 0.161610000000000f    << endr
      << 0.152928000000000f    << endr
      << 0.144246000000000f    << endr
      << 0.135564000000000f    << endr
      << 0.126882000000000f    << endr
      << 0.118200000000000f    << endr
      << 0.111710600000000f    << endr
      << 0.105221200000000f    << endr
      << 0.0987318000000000f   << endr
      << 0.0922424000000000f   << endr
      << 0.0857530000000000f   << endr
      << 0.0812178000000000f   << endr
      << 0.0766826000000000f   << endr
      << 0.0721474000000000f   << endr
      << 0.0676122000000000f   << endr
      << 0.0630770000000000f   << endr
      << 0.0596284000000000f   << endr
      << 0.0561798000000000f   << endr
      << 0.0527312000000000f   << endr
      << 0.0492826000000000f   << endr
      << 0.0458340000000000f   << endr
      << 0.0430786000000000f   << endr
      << 0.0403232000000000f   << endr
      << 0.0375678000000000f   << endr
      << 0.0348124000000000f   << endr
      << 0.0320570000000000f   << endr
      << 0.0300830000000000f   << endr
      << 0.0281090000000000f   << endr
      << 0.0261350000000000f   << endr
      << 0.0241610000000000f   << endr
      << 0.0221870000000000f   << endr
      << 0.0208720000000000f   << endr
      << 0.0195570000000000f   << endr
      << 0.0182420000000000f   << endr
      << 0.0169270000000000f   << endr
      << 0.0156120000000000f   << endr
      << 0.0147092000000000f   << endr
      << 0.0138064000000000f   << endr
      << 0.0129036000000000f   << endr
      << 0.0120008000000000f   << endr
      << 0.0110980000000000f   << endr
      << 0.0104630600000000f   << endr
      << 0.00982812000000000f  << endr
      << 0.00919318000000000f  << endr
      << 0.00855824000000000f  << endr
      << 0.00792330000000000f  << endr
      << 0.00746926000000000f  << endr
      << 0.00701522000000000f  << endr
      << 0.00656118000000000f  << endr
      << 0.00610714000000000f  << endr
      << 0.00565310000000000f  << endr
      << 0.00532326000000000f  << endr
      << 0.00499342000000000f  << endr
      << 0.00466358000000000f  << endr
      << 0.00433374000000000f  << endr
      << 0.00400390000000000f  << endr
      << 0.00376818000000000f  << endr
      << 0.00353246000000000f  << endr
      << 0.00329674000000000f  << endr
      << 0.00306102000000000f  << endr
      << 0.00282530000000000f  << endr
      << 0.00265918000000000f  << endr
      << 0.00249306000000000f  << endr
      << 0.00232694000000000f  << endr
      << 0.00216082000000000f  << endr
      << 0.00199470000000000f  << endr
      << 0.00187564000000000f  << endr
      << 0.00175658000000000f  << endr
      << 0.00163752000000000f  << endr
      << 0.00151846000000000f  << endr
      << 0.00139940000000000f  << endr
      << 0.00131348000000000f  << endr
      << 0.00122756000000000f  << endr
      << 0.00114164000000000f  << endr
      << 0.00105572000000000f  << endr
      << 0.000969800000000000f << endr
      << 0.000909534000000000f << endr
      << 0.000849268000000000f << endr
      << 0.000789002000000000f << endr
      << 0.000728736000000000f << endr
      << 0.000668470000000000f << endr
      << 0.000627058000000000f << endr
      << 0.000585646000000000f << endr
      << 0.000544234000000000f << endr
      << 0.000502822000000000f << endr
      << 0.000461410000000000f << endr
      << 0.000433274000000000f << endr
      << 0.000405138000000000f << endr
      << 0.000377002000000000f << endr
      << 0.000348866000000000f << endr
      << 0.000320730000000000f << endr
      << 0.000301730000000000f << endr
      << 0.000282730000000000f << endr
      << 0.000263730000000000f << endr
      << 0.000244730000000000f << endr
      << 0.000225730000000000f << endr
      << 0.000212530000000000f << endr
      << 0.000199330000000000f << endr
      << 0.000186130000000000f << endr
      << 0.000172930000000000f << endr
      << 0.000159730000000000f << endr
      << 0.000150334000000000f << endr
      << 0.000140938000000000f << endr
      << 0.000131542000000000f << endr
      << 0.000122146000000000f << endr
      << 0.000112750000000000f << endr
      << 0.000106102600000000f << endr
      << 9.94552000000000e-05f << endr
      << 9.28078000000000e-05f << endr
      << 8.61604000000000e-05f << endr
      << 7.95130000000000e-05f << endr
      << 7.48278000000000e-05f << endr
      << 7.01426000000000e-05f << endr
      << 6.54574000000000e-05f << endr
      << 6.07722000000000e-05f << endr
      << 5.60870000000000e-05f << endr
      << 5.27778000000000e-05f << endr
      << 4.94686000000000e-05f << endr
      << 4.61594000000000e-05f << endr
      << 4.28502000000000e-05f << endr
      << 3.95410000000000e-05f << endr
      << 3.72032000000000e-05f << endr
      << 3.48654000000000e-05f << endr
      << 3.25276000000000e-05f << endr
      << 3.01898000000000e-05f << endr
      << 2.78520000000000e-05f << endr
      << 2.62010000000000e-05f << endr
      << 2.45500000000000e-05f << endr
      << 2.28990000000000e-05f << endr
      << 2.12480000000000e-05f << endr
      << 1.95970000000000e-05f << endr
      << 1.84316000000000e-05f << endr
      << 1.72662000000000e-05f << endr
      << 1.61008000000000e-05f << endr
      << 1.49354000000000e-05f << endr
      << 1.37700000000000e-05f << endr
      << 1.29500000000000e-05f << endr
      << 1.21300000000000e-05f << endr
      << 1.13100000000000e-05f << endr
      << 1.04900000000000e-05f << endr
      << 9.67000000000000e-06f << endr
      << 9.09436000000000e-06f << endr
      << 8.51872000000000e-06f << endr
      << 7.94308000000000e-06f << endr
      << 7.36744000000000e-06f << endr
      << 6.79180000000000e-06f << endr
      << 6.38756000000000e-06f << endr
      << 5.98332000000000e-06f << endr
      << 5.57908000000000e-06f << endr
      << 5.17484000000000e-06f << endr
      << 4.77060000000000e-06f << endr
      << 4.48748000000000e-06f << endr
      << 4.20436000000000e-06f << endr
      << 3.92124000000000e-06f << endr
      << 3.63812000000000e-06f << endr
      << 3.35500000000000e-06f << endr
      << 3.15468000000000e-06f << endr
      << 2.95436000000000e-06f << endr
      << 2.75404000000000e-06f << endr
      << 2.55372000000000e-06f << endr
      << 2.35340000000000e-06f << endr
      << 2.21026000000000e-06f << endr
      << 2.06712000000000e-06f << endr
      << 1.92398000000000e-06f << endr
      << 1.78084000000000e-06f << endr
      << 1.63770000000000e-06f << endr;

 xFcn = txFcn;
}


void MKCC::CMF_JUDD_VOS_yFcn(fvec &yFcn)
{
  fvec::fixed<446> tyFcn;

tyFcn<< 0.000200000000000000f << endr
     << 0.000239112000000000f << endr
     << 0.000278224000000000f << endr
     << 0.000317336000000000f << endr
     << 0.000356448000000000f << endr
     << 0.000395560000000000f << endr
     << 0.000476448000000000f << endr
     << 0.000557336000000000f << endr
     << 0.000638224000000000f << endr
     << 0.000719112000000000f << endr
     << 0.000800000000000000f << endr
     << 0.000949140000000000f << endr
     << 0.00109828000000000f  << endr
     << 0.00124742000000000f  << endr
     << 0.00139656000000000f  << endr
     << 0.00154570000000000f  << endr
     << 0.00179656000000000f  << endr
     << 0.00204742000000000f  << endr
     << 0.00229828000000000f  << endr
     << 0.00254914000000000f  << endr
     << 0.00280000000000000f  << endr
     << 0.00317124000000000f  << endr
     << 0.00354248000000000f  << endr
     << 0.00391372000000000f  << endr
     << 0.00428496000000000f  << endr
     << 0.00465620000000000f  << endr
     << 0.00520496000000000f  << endr
     << 0.00575372000000000f  << endr
     << 0.00630248000000000f  << endr
     << 0.00685124000000000f  << endr
     << 0.00740000000000000f  << endr
     << 0.00827580000000000f  << endr
     << 0.00915160000000000f  << endr
     << 0.0100274000000000f   << endr
     << 0.0109032000000000f   << endr
     << 0.0117790000000000f   << endr
     << 0.0129232000000000f   << endr
     << 0.0140674000000000f   << endr
     << 0.0152116000000000f   << endr
     << 0.0163558000000000f   << endr
     << 0.0175000000000000f   << endr
     << 0.0185356000000000f   << endr
     << 0.0195712000000000f   << endr
     << 0.0206068000000000f   << endr
     << 0.0216424000000000f   << endr
     << 0.0226780000000000f   << endr
     << 0.0236024000000000f   << endr
     << 0.0245268000000000f   << endr
     << 0.0254512000000000f   << endr
     << 0.0263756000000000f   << endr
     << 0.0273000000000000f   << endr
     << 0.0283568000000000f   << endr
     << 0.0294136000000000f   << endr
     << 0.0304704000000000f   << endr
     << 0.0315272000000000f   << endr
     << 0.0325840000000000f   << endr
     << 0.0336472000000000f   << endr
     << 0.0347104000000000f   << endr
     << 0.0357736000000000f   << endr
     << 0.0368368000000000f   << endr
     << 0.0379000000000000f   << endr
     << 0.0387982000000000f   << endr
     << 0.0396964000000000f   << endr
     << 0.0405946000000000f   << endr
     << 0.0414928000000000f   << endr
     << 0.0423910000000000f   << endr
     << 0.0432728000000000f   << endr
     << 0.0441546000000000f   << endr
     << 0.0450364000000000f   << endr
     << 0.0459182000000000f   << endr
     << 0.0468000000000000f   << endr
     << 0.0478644000000000f   << endr
     << 0.0489288000000000f   << endr
     << 0.0499932000000000f   << endr
     << 0.0510576000000000f   << endr
     << 0.0521220000000000f   << endr
     << 0.0536976000000000f   << endr
     << 0.0552732000000000f   << endr
     << 0.0568488000000000f   << endr
     << 0.0584244000000000f   << endr
     << 0.0600000000000000f   << endr
     << 0.0625884000000000f   << endr
     << 0.0651768000000000f   << endr
     << 0.0677652000000000f   << endr
     << 0.0703536000000000f   << endr
     << 0.0729420000000000f   << endr
     << 0.0765496000000000f   << endr
     << 0.0801572000000000f   << endr
     << 0.0837648000000000f   << endr
     << 0.0873724000000000f   << endr
     << 0.0909800000000000f   << endr
     << 0.0953520000000000f   << endr
     << 0.0997240000000000f   << endr
     << 0.104096000000000f    << endr
     << 0.108468000000000f    << endr
     << 0.112840000000000f    << endr
     << 0.118076000000000f    << endr
     << 0.123312000000000f    << endr
     << 0.128548000000000f    << endr
     << 0.133784000000000f    << endr
     << 0.139020000000000f    << endr
     << 0.145190000000000f    << endr
     << 0.151360000000000f    << endr
     << 0.157530000000000f    << endr
     << 0.163700000000000f    << endr
     << 0.169870000000000f    << endr
     << 0.177500000000000f    << endr
     << 0.185130000000000f    << endr
     << 0.192760000000000f    << endr
     << 0.200390000000000f    << endr
     << 0.208020000000000f    << endr
     << 0.218032000000000f    << endr
     << 0.228044000000000f    << endr
     << 0.238056000000000f    << endr
     << 0.248068000000000f    << endr
     << 0.258080000000000f    << endr
     << 0.271064000000000f    << endr
     << 0.284048000000000f    << endr
     << 0.297032000000000f    << endr
     << 0.310016000000000f    << endr
     << 0.323000000000000f    << endr
     << 0.339480000000000f    << endr
     << 0.355960000000000f    << endr
     << 0.372440000000000f    << endr
     << 0.388920000000000f    << endr
     << 0.405400000000000f    << endr
     << 0.424920000000000f    << endr
     << 0.444440000000000f    << endr
     << 0.463960000000000f    << endr
     << 0.483480000000000f    << endr
     << 0.503000000000000f    << endr
     << 0.524022000000000f    << endr
     << 0.545044000000000f    << endr
     << 0.566066000000000f    << endr
     << 0.587088000000000f    << endr
     << 0.608110000000000f    << endr
     << 0.628488000000000f    << endr
     << 0.648866000000000f    << endr
     << 0.669244000000000f    << endr
     << 0.689622000000000f    << endr
     << 0.710000000000000f    << endr
     << 0.727020000000000f    << endr
     << 0.744040000000000f    << endr
     << 0.761060000000000f    << endr
     << 0.778080000000000f    << endr
     << 0.795100000000000f    << endr
     << 0.808480000000000f    << endr
     << 0.821860000000000f    << endr
     << 0.835240000000000f    << endr
     << 0.848620000000000f    << endr
     << 0.862000000000000f    << endr
     << 0.872610000000000f    << endr
     << 0.883220000000000f    << endr
     << 0.893830000000000f    << endr
     << 0.904440000000000f    << endr
     << 0.915050000000000f    << endr
     << 0.922840000000000f    << endr
     << 0.930630000000000f    << endr
     << 0.938420000000000f    << endr
     << 0.946210000000000f    << endr
     << 0.954000000000000f    << endr
     << 0.959208000000000f    << endr
     << 0.964416000000000f    << endr
     << 0.969624000000000f    << endr
     << 0.974832000000000f    << endr
     << 0.980040000000000f    << endr
     << 0.983022000000000f    << endr
     << 0.986004000000000f    << endr
     << 0.988986000000000f    << endr
     << 0.991968000000000f    << endr
     << 0.994950000000000f    << endr
     << 0.995980000000000f    << endr
     << 0.997010000000000f    << endr
     << 0.998040000000000f    << endr
     << 0.999070000000000f    << endr
     << 1.00010000000000f     << endr
     << 0.999080000000000f    << endr
     << 0.998060000000000f    << endr
     << 0.997040000000000f    << endr
     << 0.996020000000000f    << endr
     << 0.995000000000000f    << endr
     << 0.991750000000000f    << endr
     << 0.988500000000000f    << endr
     << 0.985250000000000f    << endr
     << 0.982000000000000f    << endr
     << 0.978750000000000f    << endr
     << 0.973400000000000f    << endr
     << 0.968050000000000f    << endr
     << 0.962700000000000f    << endr
     << 0.957350000000000f    << endr
     << 0.952000000000000f    << endr
     << 0.944716000000000f    << endr
     << 0.937432000000000f    << endr
     << 0.930148000000000f    << endr
     << 0.922864000000000f    << endr
     << 0.915580000000000f    << endr
     << 0.906464000000000f    << endr
     << 0.897348000000000f    << endr
     << 0.888232000000000f    << endr
     << 0.879116000000000f    << endr
     << 0.870000000000000f    << endr
     << 0.859246000000000f    << endr
     << 0.848492000000000f    << endr
     << 0.837738000000000f    << endr
     << 0.826984000000000f    << endr
     << 0.816230000000000f    << endr
     << 0.804384000000000f    << endr
     << 0.792538000000000f    << endr
     << 0.780692000000000f    << endr
     << 0.768846000000000f    << endr
     << 0.757000000000000f    << endr
     << 0.744566000000000f    << endr
     << 0.732132000000000f    << endr
     << 0.719698000000000f    << endr
     << 0.707264000000000f    << endr
     << 0.694830000000000f    << endr
     << 0.682064000000000f    << endr
     << 0.669298000000000f    << endr
     << 0.656532000000000f    << endr
     << 0.643766000000000f    << endr
     << 0.631000000000000f    << endr
     << 0.618108000000000f    << endr
     << 0.605216000000000f    << endr
     << 0.592324000000000f    << endr
     << 0.579432000000000f    << endr
     << 0.566540000000000f    << endr
     << 0.553832000000000f    << endr
     << 0.541124000000000f    << endr
     << 0.528416000000000f    << endr
     << 0.515708000000000f    << endr
     << 0.503000000000000f    << endr
     << 0.490744000000000f    << endr
     << 0.478488000000000f    << endr
     << 0.466232000000000f    << endr
     << 0.453976000000000f    << endr
     << 0.441720000000000f    << endr
     << 0.429576000000000f    << endr
     << 0.417432000000000f    << endr
     << 0.405288000000000f    << endr
     << 0.393144000000000f    << endr
     << 0.381000000000000f    << endr
     << 0.368904000000000f    << endr
     << 0.356808000000000f    << endr
     << 0.344712000000000f    << endr
     << 0.332616000000000f    << endr
     << 0.320520000000000f    << endr
     << 0.309416000000000f    << endr
     << 0.298312000000000f    << endr
     << 0.287208000000000f    << endr
     << 0.276104000000000f    << endr
     << 0.265000000000000f    << endr
     << 0.255404000000000f    << endr
     << 0.245808000000000f    << endr
     << 0.236212000000000f    << endr
     << 0.226616000000000f    << endr
     << 0.217020000000000f    << endr
     << 0.208616000000000f    << endr
     << 0.200212000000000f    << endr
     << 0.191808000000000f    << endr
     << 0.183404000000000f    << endr
     << 0.175000000000000f    << endr
     << 0.167624000000000f    << endr
     << 0.160248000000000f    << endr
     << 0.152872000000000f    << endr
     << 0.145496000000000f    << endr
     << 0.138120000000000f    << endr
     << 0.131896000000000f    << endr
     << 0.125672000000000f    << endr
     << 0.119448000000000f    << endr
     << 0.113224000000000f    << endr
     << 0.107000000000000f    << endr
     << 0.101930400000000f    << endr
     << 0.0968608000000000f   << endr
     << 0.0917912000000000f   << endr
     << 0.0867216000000000f   << endr
     << 0.0816520000000000f   << endr
     << 0.0775216000000000f   << endr
     << 0.0733912000000000f   << endr
     << 0.0692608000000000f   << endr
     << 0.0651304000000000f   << endr
     << 0.0610000000000000f   << endr
     << 0.0576654000000000f   << endr
     << 0.0543308000000000f   << endr
     << 0.0509962000000000f   << endr
     << 0.0476616000000000f   << endr
     << 0.0443270000000000f   << endr
     << 0.0418616000000000f   << endr
     << 0.0393962000000000f   << endr
     << 0.0369308000000000f   << endr
     << 0.0344654000000000f   << endr
     << 0.0320000000000000f   << endr
     << 0.0302908000000000f   << endr
     << 0.0285816000000000f   << endr
     << 0.0268724000000000f   << endr
     << 0.0251632000000000f   << endr
     << 0.0234540000000000f   << endr
     << 0.0221632000000000f   << endr
     << 0.0208724000000000f   << endr
     << 0.0195816000000000f   << endr
     << 0.0182908000000000f   << endr
     << 0.0170000000000000f   << endr
     << 0.0159744000000000f   << endr
     << 0.0149488000000000f   << endr
     << 0.0139232000000000f   << endr
     << 0.0128976000000000f   << endr
     << 0.0118720000000000f   << endr
     << 0.0111396000000000f   << endr
     << 0.0104072000000000f   << endr
     << 0.00967480000000000f  << endr
     << 0.00894240000000000f  << endr
     << 0.00821000000000000f  << endr
     << 0.00772246000000000f  << endr
     << 0.00723492000000000f  << endr
     << 0.00674738000000000f  << endr
     << 0.00625984000000000f  << endr
     << 0.00577230000000000f  << endr
     << 0.00543824000000000f  << endr
     << 0.00510418000000000f  << endr
     << 0.00477012000000000f  << endr
     << 0.00443606000000000f  << endr
     << 0.00410200000000000f  << endr
     << 0.00386742000000000f  << endr
     << 0.00363284000000000f  << endr
     << 0.00339826000000000f  << endr
     << 0.00316368000000000f  << endr
     << 0.00292910000000000f  << endr
     << 0.00276148000000000f  << endr
     << 0.00259386000000000f  << endr
     << 0.00242624000000000f  << endr
     << 0.00225862000000000f  << endr
     << 0.00209100000000000f  << endr
     << 0.00196924000000000f  << endr
     << 0.00184748000000000f  << endr
     << 0.00172572000000000f  << endr
     << 0.00160396000000000f  << endr
     << 0.00148220000000000f  << endr
     << 0.00139516000000000f  << endr
     << 0.00130812000000000f  << endr
     << 0.00122108000000000f  << endr
     << 0.00113404000000000f  << endr
     << 0.00104700000000000f  << endr
     << 0.000985630000000000f << endr
     << 0.000924260000000000f << endr
     << 0.000862890000000000f << endr
     << 0.000801520000000000f << endr
     << 0.000740150000000000f << endr
     << 0.000696120000000000f << endr
     << 0.000652090000000000f << endr
     << 0.000608060000000000f << endr
     << 0.000564030000000000f << endr
     << 0.000520000000000000f << endr
     << 0.000488186000000000f << endr
     << 0.000456372000000000f << endr
     << 0.000424558000000000f << endr
     << 0.000392744000000000f << endr
     << 0.000360930000000000f << endr
     << 0.000338584000000000f << endr
     << 0.000316238000000000f << endr
     << 0.000293892000000000f << endr
     << 0.000271546000000000f << endr
     << 0.000249200000000000f << endr
     << 0.000233822000000000f << endr
     << 0.000218444000000000f << endr
     << 0.000203066000000000f << endr
     << 0.000187688000000000f << endr
     << 0.000172310000000000f << endr
     << 0.000161848000000000f << endr
     << 0.000151386000000000f << endr
     << 0.000140924000000000f << endr
     << 0.000130462000000000f << endr
     << 0.000120000000000000f << endr
     << 0.000112924000000000f << endr
     << 0.000105848000000000f << endr
     << 9.87720000000000e-05f << endr
     << 9.16960000000000e-05f << endr
     << 8.46200000000000e-05f << endr
     << 7.96960000000000e-05f << endr
     << 7.47720000000000e-05f << endr
     << 6.98480000000000e-05f << endr
     << 6.49240000000000e-05f << endr
     << 6.00000000000000e-05f << endr
     << 5.64892000000000e-05f << endr
     << 5.29784000000000e-05f << endr
     << 4.94676000000000e-05f << endr
     << 4.59568000000000e-05f << endr
     << 4.24460000000000e-05f << endr
     << 3.99568000000000e-05f << endr
     << 3.74676000000000e-05f << endr
     << 3.49784000000000e-05f << endr
     << 3.24892000000000e-05f << endr
     << 3.00000000000000e-05f << endr
     << 2.82420000000000e-05f << endr
     << 2.64840000000000e-05f << endr
     << 2.47260000000000e-05f << endr
     << 2.29680000000000e-05f << endr
     << 2.12100000000000e-05f << endr
     << 1.99658000000000e-05f << endr
     << 1.87216000000000e-05f << endr
     << 1.74774000000000e-05f << endr
     << 1.62332000000000e-05f << endr
     << 1.49890000000000e-05f << endr
     << 1.41080000000000e-05f << endr
     << 1.32270000000000e-05f << endr
     << 1.23460000000000e-05f << endr
     << 1.14650000000000e-05f << endr
     << 1.05840000000000e-05f << endr
     << 9.96032000000000e-06f << endr
     << 9.33664000000000e-06f << endr
     << 8.71296000000000e-06f << endr
     << 8.08928000000000e-06f << endr
     << 7.46560000000000e-06f << endr
     << 7.02432000000000e-06f << endr
     << 6.58304000000000e-06f << endr
     << 6.14176000000000e-06f << endr
     << 5.70048000000000e-06f << endr
     << 5.25920000000000e-06f << endr
     << 4.94792000000000e-06f << endr
     << 4.63664000000000e-06f << endr
     << 4.32536000000000e-06f << endr
     << 4.01408000000000e-06f << endr
     << 3.70280000000000e-06f << endr
     << 3.48376000000000e-06f << endr
     << 3.26472000000000e-06f << endr
     << 3.04568000000000e-06f << endr
     << 2.82664000000000e-06f << endr
     << 2.60760000000000e-06f << endr
     << 2.45338000000000e-06f << endr
     << 2.29916000000000e-06f << endr
     << 2.14494000000000e-06f << endr
     << 1.99072000000000e-06f << endr
     << 1.83650000000000e-06f << endr
     << 1.72820000000000e-06f << endr
     << 1.61990000000000e-06f << endr
     << 1.51160000000000e-06f << endr
     << 1.40330000000000e-06f << endr
     << 1.29500000000000e-06f << endr
     << 1.21818400000000e-06f << endr
     << 1.14136800000000e-06f << endr
     << 1.06455200000000e-06f << endr
     << 9.87736000000000e-07f << endr
     << 9.10920000000000e-07f << endr
     << 8.55864000000000e-07f << endr
     << 8.00808000000000e-07f << endr
     << 7.45752000000000e-07f << endr
     << 6.90696000000000e-07f << endr
     << 6.35640000000000e-07f << endr;

 yFcn = tyFcn;
}


void MKCC::CMF_JUDD_VOS_zFcn(fvec &zFcn)
{
  fvec::fixed<446> tzFcn;

tzFcn << 0.0122600000000000f   << endr
      << 0.0146524000000000f   << endr
      << 0.0170448000000000f   << endr
      << 0.0194372000000000f   << endr
      << 0.0218296000000000f   << endr
      << 0.0242220000000000f   << endr
      << 0.0292276000000000f   << endr
      << 0.0342332000000000f   << endr
      << 0.0392388000000000f   << endr
      << 0.0442444000000000f   << endr
      << 0.0492500000000000f   << endr
      << 0.0584270000000000f   << endr
      << 0.0676040000000000f   << endr
      << 0.0767810000000000f   << endr
      << 0.0859580000000000f   << endr
      << 0.0951350000000000f   << endr
      << 0.110926000000000f    << endr
      << 0.126717000000000f    << endr
      << 0.142508000000000f    << endr
      << 0.158299000000000f    << endr
      << 0.174090000000000f    << endr
      << 0.197298000000000f    << endr
      << 0.220506000000000f    << endr
      << 0.243714000000000f    << endr
      << 0.266922000000000f    << endr
      << 0.290130000000000f    << endr
      << 0.324210000000000f    << endr
      << 0.358290000000000f    << endr
      << 0.392370000000000f    << endr
      << 0.426450000000000f    << endr
      << 0.460530000000000f    << endr
      << 0.514756000000000f    << endr
      << 0.568982000000000f    << endr
      << 0.623208000000000f    << endr
      << 0.677434000000000f    << endr
      << 0.731660000000000f    << endr
      << 0.798488000000000f    << endr
      << 0.865316000000000f    << endr
      << 0.932144000000000f    << endr
      << 0.998972000000000f    << endr
      << 1.06580000000000f     << endr
      << 1.11556000000000f     << endr
      << 1.16532000000000f     << endr
      << 1.21508000000000f     << endr
      << 1.26484000000000f     << endr
      << 1.31460000000000f     << endr
      << 1.34512000000000f     << endr
      << 1.37564000000000f     << endr
      << 1.40616000000000f     << endr
      << 1.43668000000000f     << endr
      << 1.46720000000000f     << endr
      << 1.48968000000000f     << endr
      << 1.51216000000000f     << endr
      << 1.53464000000000f     << endr
      << 1.55712000000000f     << endr
      << 1.57960000000000f     << endr
      << 1.58700000000000f     << endr
      << 1.59440000000000f     << endr
      << 1.60180000000000f     << endr
      << 1.60920000000000f     << endr
      << 1.61660000000000f     << endr
      << 1.60692000000000f     << endr
      << 1.59724000000000f     << endr
      << 1.58756000000000f     << endr
      << 1.57788000000000f     << endr
      << 1.56820000000000f     << endr
      << 1.54890000000000f     << endr
      << 1.52960000000000f     << endr
      << 1.51030000000000f     << endr
      << 1.49100000000000f     << endr
      << 1.47170000000000f     << endr
      << 1.45216000000000f     << endr
      << 1.43262000000000f     << endr
      << 1.41308000000000f     << endr
      << 1.39354000000000f     << endr
      << 1.37400000000000f     << endr
      << 1.35754000000000f     << endr
      << 1.34108000000000f     << endr
      << 1.32462000000000f     << endr
      << 1.30816000000000f     << endr
      << 1.29170000000000f     << endr
      << 1.28048000000000f     << endr
      << 1.26926000000000f     << endr
      << 1.25804000000000f     << endr
      << 1.24682000000000f     << endr
      << 1.23560000000000f     << endr
      << 1.21124000000000f     << endr
      << 1.18688000000000f     << endr
      << 1.16252000000000f     << endr
      << 1.13816000000000f     << endr
      << 1.11380000000000f     << endr
      << 1.07948000000000f     << endr
      << 1.04516000000000f     << endr
      << 1.01084000000000f     << endr
      << 0.976520000000000f    << endr
      << 0.942200000000000f    << endr
      << 0.904952000000000f    << endr
      << 0.867704000000000f    << endr
      << 0.830456000000000f    << endr
      << 0.793208000000000f    << endr
      << 0.755960000000000f    << endr
      << 0.722048000000000f    << endr
      << 0.688136000000000f    << endr
      << 0.654224000000000f    << endr
      << 0.620312000000000f    << endr
      << 0.586400000000000f    << endr
      << 0.558458000000000f    << endr
      << 0.530516000000000f    << endr
      << 0.502574000000000f    << endr
      << 0.474632000000000f    << endr
      << 0.446690000000000f    << endr
      << 0.425584000000000f    << endr
      << 0.404478000000000f    << endr
      << 0.383372000000000f    << endr
      << 0.362266000000000f    << endr
      << 0.341160000000000f    << endr
      << 0.325802000000000f    << endr
      << 0.310444000000000f    << endr
      << 0.295086000000000f    << endr
      << 0.279728000000000f    << endr
      << 0.264370000000000f    << endr
      << 0.252684000000000f    << endr
      << 0.240998000000000f    << endr
      << 0.229312000000000f    << endr
      << 0.217626000000000f    << endr
      << 0.205940000000000f    << endr
      << 0.195642000000000f    << endr
      << 0.185344000000000f    << endr
      << 0.175046000000000f    << endr
      << 0.164748000000000f    << endr
      << 0.154450000000000f    << endr
      << 0.145396000000000f    << endr
      << 0.136342000000000f    << endr
      << 0.127288000000000f    << endr
      << 0.118234000000000f    << endr
      << 0.109180000000000f    << endr
      << 0.102661000000000f    << endr
      << 0.0961420000000000f   << endr
      << 0.0896230000000000f   << endr
      << 0.0831040000000000f   << endr
      << 0.0765850000000000f   << endr
      << 0.0725134000000000f   << endr
      << 0.0684418000000000f   << endr
      << 0.0643702000000000f   << endr
      << 0.0602986000000000f   << endr
      << 0.0562270000000000f   << endr
      << 0.0532548000000000f   << endr
      << 0.0502826000000000f   << endr
      << 0.0473104000000000f   << endr
      << 0.0443382000000000f   << endr
      << 0.0413660000000000f   << endr
      << 0.0389634000000000f   << endr
      << 0.0365608000000000f   << endr
      << 0.0341582000000000f   << endr
      << 0.0317556000000000f   << endr
      << 0.0293530000000000f   << endr
      << 0.0274908000000000f   << endr
      << 0.0256286000000000f   << endr
      << 0.0237664000000000f   << endr
      << 0.0219042000000000f   << endr
      << 0.0200420000000000f   << endr
      << 0.0186960000000000f   << endr
      << 0.0173500000000000f   << endr
      << 0.0160040000000000f   << endr
      << 0.0146580000000000f   << endr
      << 0.0133120000000000f   << endr
      << 0.0124060600000000f   << endr
      << 0.0115001200000000f   << endr
      << 0.0105941800000000f   << endr
      << 0.00968824000000000f  << endr
      << 0.00878230000000000f  << endr
      << 0.00819730000000000f  << endr
      << 0.00761230000000000f  << endr
      << 0.00702730000000000f  << endr
      << 0.00644230000000000f  << endr
      << 0.00585730000000000f  << endr
      << 0.00549570000000000f  << endr
      << 0.00513410000000000f  << endr
      << 0.00477250000000000f  << endr
      << 0.00441090000000000f  << endr
      << 0.00404930000000000f  << endr
      << 0.00382378000000000f  << endr
      << 0.00359826000000000f  << endr
      << 0.00337274000000000f  << endr
      << 0.00314722000000000f  << endr
      << 0.00292170000000000f  << endr
      << 0.00279278000000000f  << endr
      << 0.00266386000000000f  << endr
      << 0.00253494000000000f  << endr
      << 0.00240602000000000f  << endr
      << 0.00227710000000000f  << endr
      << 0.00221580000000000f  << endr
      << 0.00215450000000000f  << endr
      << 0.00209320000000000f  << endr
      << 0.00203190000000000f  << endr
      << 0.00197060000000000f  << endr
      << 0.00193780000000000f  << endr
      << 0.00190500000000000f  << endr
      << 0.00187220000000000f  << endr
      << 0.00183940000000000f  << endr
      << 0.00180660000000000f  << endr
      << 0.00175426000000000f  << endr
      << 0.00170192000000000f  << endr
      << 0.00164958000000000f  << endr
      << 0.00159724000000000f  << endr
      << 0.00154490000000000f  << endr
      << 0.00148288000000000f  << endr
      << 0.00142086000000000f  << endr
      << 0.00135884000000000f  << endr
      << 0.00129682000000000f  << endr
      << 0.00123480000000000f  << endr
      << 0.00121138000000000f  << endr
      << 0.00118796000000000f  << endr
      << 0.00116454000000000f  << endr
      << 0.00114112000000000f  << endr
      << 0.00111770000000000f  << endr
      << 0.00107528800000000f  << endr
      << 0.00103287600000000f  << endr
      << 0.000990464000000000f << endr
      << 0.000948052000000000f << endr
      << 0.000905640000000000f << endr
      << 0.000863446000000000f << endr
      << 0.000821252000000000f << endr
      << 0.000779058000000000f << endr
      << 0.000736864000000000f << endr
      << 0.000694670000000000f << endr
      << 0.000641506000000000f << endr
      << 0.000588342000000000f << endr
      << 0.000535178000000000f << endr
      << 0.000482014000000000f << endr
      << 0.000428850000000000f << endr
      << 0.000406714000000000f << endr
      << 0.000384578000000000f << endr
      << 0.000362442000000000f << endr
      << 0.000340306000000000f << endr
      << 0.000318170000000000f << endr
      << 0.000305732000000000f << endr
      << 0.000293294000000000f << endr
      << 0.000280856000000000f << endr
      << 0.000268418000000000f << endr
      << 0.000255980000000000f << endr
      << 0.000236142000000000f << endr
      << 0.000216304000000000f << endr
      << 0.000196466000000000f << endr
      << 0.000176628000000000f << endr
      << 0.000156790000000000f << endr
      << 0.000144970800000000f << endr
      << 0.000133151600000000f << endr
      << 0.000121332400000000f << endr
      << 0.000109513200000000f << endr
      << 9.76940000000000e-05f << endr
      << 9.19440000000000e-05f << endr
      << 8.61940000000000e-05f << endr
      << 8.04440000000000e-05f << endr
      << 7.46940000000000e-05f << endr
      << 6.89440000000000e-05f << endr
      << 6.53882000000000e-05f << endr
      << 6.18324000000000e-05f << endr
      << 5.82766000000000e-05f << endr
      << 5.47208000000000e-05f << endr
      << 5.11650000000000e-05f << endr
      << 4.81352000000000e-05f << endr
      << 4.51054000000000e-05f << endr
      << 4.20756000000000e-05f << endr
      << 3.90458000000000e-05f << endr
      << 3.60160000000000e-05f << endr
      << 3.36604000000000e-05f << endr
      << 3.13048000000000e-05f << endr
      << 2.89492000000000e-05f << endr
      << 2.65936000000000e-05f << endr
      << 2.42380000000000e-05f << endr
      << 2.27734000000000e-05f << endr
      << 2.13088000000000e-05f << endr
      << 1.98442000000000e-05f << endr
      << 1.83796000000000e-05f << endr
      << 1.69150000000000e-05f << endr
      << 1.59132000000000e-05f << endr
      << 1.49114000000000e-05f << endr
      << 1.39096000000000e-05f << endr
      << 1.29078000000000e-05f << endr
      << 1.19060000000000e-05f << endr
      << 1.11545800000000e-05f << endr
      << 1.04031600000000e-05f << endr
      << 9.65174000000000e-06f << endr
      << 8.90032000000000e-06f << endr
      << 8.14890000000000e-06f << endr
      << 7.63924000000000e-06f << endr
      << 7.12958000000000e-06f << endr
      << 6.61992000000000e-06f << endr
      << 6.11026000000000e-06f << endr
      << 5.60060000000000e-06f << endr
      << 5.27136000000000e-06f << endr
      << 4.94212000000000e-06f << endr
      << 4.61288000000000e-06f << endr
      << 4.28364000000000e-06f << endr
      << 3.95440000000000e-06f << endr
      << 3.72176000000000e-06f << endr
      << 3.48912000000000e-06f << endr
      << 3.25648000000000e-06f << endr
      << 3.02384000000000e-06f << endr
      << 2.79120000000000e-06f << endr
      << 2.61648000000000e-06f << endr
      << 2.44176000000000e-06f << endr
      << 2.26704000000000e-06f << endr
      << 2.09232000000000e-06f << endr
      << 1.91760000000000e-06f << endr
      << 1.79678000000000e-06f << endr
      << 1.67596000000000e-06f << endr
      << 1.55514000000000e-06f << endr
      << 1.43432000000000e-06f << endr
      << 1.31350000000000e-06f << endr
      << 1.23383800000000e-06f << endr
      << 1.15417600000000e-06f << endr
      << 1.07451400000000e-06f << endr
      << 9.94852000000000e-07f << endr
      << 9.15190000000000e-07f << endr
      << 8.61686000000000e-07f << endr
      << 8.08182000000000e-07f << endr
      << 7.54678000000000e-07f << endr
      << 7.01174000000000e-07f << endr
      << 6.47670000000000e-07f << endr
      << 6.10840000000000e-07f << endr
      << 5.74010000000000e-07f << endr
      << 5.37180000000000e-07f << endr
      << 5.00350000000000e-07f << endr
      << 4.63520000000000e-07f << endr
      << 4.37424000000000e-07f << endr
      << 4.11328000000000e-07f << endr
      << 3.85232000000000e-07f << endr
      << 3.59136000000000e-07f << endr
      << 3.33040000000000e-07f << endr
      << 3.14078000000000e-07f << endr
      << 2.95116000000000e-07f << endr
      << 2.76154000000000e-07f << endr
      << 2.57192000000000e-07f << endr
      << 2.38230000000000e-07f << endr
      << 2.24636000000000e-07f << endr
      << 2.11042000000000e-07f << endr
      << 1.97448000000000e-07f << endr
      << 1.83854000000000e-07f << endr
      << 1.70260000000000e-07f << endr
      << 1.60622000000000e-07f << endr
      << 1.50984000000000e-07f << endr
      << 1.41346000000000e-07f << endr
      << 1.31708000000000e-07f << endr
      << 1.22070000000000e-07f << endr
      << 1.15077400000000e-07f << endr
      << 1.08084800000000e-07f << endr
      << 1.01092200000000e-07f << endr
      << 9.40996000000000e-08f << endr
      << 8.71070000000000e-08f << endr
      << 8.19766000000000e-08f << endr
      << 7.68462000000000e-08f << endr
      << 7.17158000000000e-08f << endr
      << 6.65854000000000e-08f << endr
      << 6.14550000000000e-08f << endr
      << 5.77964000000000e-08f << endr
      << 5.41378000000000e-08f << endr
      << 5.04792000000000e-08f << endr
      << 4.68206000000000e-08f << endr
      << 4.31620000000000e-08f << endr
      << 4.06054000000000e-08f << endr
      << 3.80488000000000e-08f << endr
      << 3.54922000000000e-08f << endr
      << 3.29356000000000e-08f << endr
      << 3.03790000000000e-08f << endr
      << 2.86140000000000e-08f << endr
      << 2.68490000000000e-08f << endr
      << 2.50840000000000e-08f << endr
      << 2.33190000000000e-08f << endr
      << 2.15540000000000e-08f << endr
      << 2.03418000000000e-08f << endr
      << 1.91296000000000e-08f << endr
      << 1.79174000000000e-08f << endr
      << 1.67052000000000e-08f << endr
      << 1.54930000000000e-08f << endr
      << 1.46352000000000e-08f << endr
      << 1.37774000000000e-08f << endr
      << 1.29196000000000e-08f << endr
      << 1.20618000000000e-08f << endr
      << 1.12040000000000e-08f << endr
      << 1.05806600000000e-08f << endr
      << 9.95732000000000e-09f << endr
      << 9.33398000000000e-09f << endr
      << 8.71064000000000e-09f << endr
      << 8.08730000000000e-09f << endr
      << 7.63664000000000e-09f << endr
      << 7.18598000000000e-09f << endr
      << 6.73532000000000e-09f << endr
      << 6.28466000000000e-09f << endr
      << 5.83400000000000e-09f << endr
      << 5.50940000000000e-09f << endr
      << 5.18480000000000e-09f << endr
      << 4.86020000000000e-09f << endr
      << 4.53560000000000e-09f << endr
      << 4.21100000000000e-09f << endr
      << 3.97646000000000e-09f << endr
      << 3.74192000000000e-09f << endr
      << 3.50738000000000e-09f << endr
      << 3.27284000000000e-09f << endr
      << 3.03830000000000e-09f << endr
      << 2.86878000000000e-09f << endr
      << 2.69926000000000e-09f << endr
      << 2.52974000000000e-09f << endr
      << 2.36022000000000e-09f << endr
      << 2.19070000000000e-09f << endr
      << 2.06812000000000e-09f << endr
      << 1.94554000000000e-09f << endr
      << 1.82296000000000e-09f << endr
      << 1.70038000000000e-09f << endr
      << 1.57780000000000e-09f << endr
      << 1.48920000000000e-09f << endr
      << 1.40060000000000e-09f << endr
      << 1.31200000000000e-09f << endr
      << 1.22340000000000e-09f << endr
      << 1.13480000000000e-09f << endr
      << 1.07097000000000e-09f << endr
      << 1.00714000000000e-09f << endr
      << 9.43310000000000e-10f << endr
      << 8.79480000000000e-10f << endr
      << 8.15650000000000e-10f << endr
      << 7.69772000000000e-10f << endr
      << 7.23894000000000e-10f << endr
      << 6.78016000000000e-10f << endr
      << 6.32138000000000e-10f << endr
      << 5.86260000000000e-10f << endr
      << 5.53284000000000e-10f << endr
      << 5.20308000000000e-10f << endr
      << 4.87332000000000e-10f << endr
      << 4.54356000000000e-10f << endr
      << 4.21380000000000e-10f << endr
      << 3.97742000000000e-10f << endr
      << 3.74104000000000e-10f << endr
      << 3.50466000000000e-10f << endr
      << 3.26828000000000e-10f << endr
      << 3.03190000000000e-10f << endr
      << 2.86058000000000e-10f << endr
      << 2.68926000000000e-10f << endr
      << 2.51794000000000e-10f << endr
      << 2.34662000000000e-10f << endr
      << 2.17530000000000e-10f << endr
      << 2.04976000000000e-10f << endr
      << 1.92422000000000e-10f << endr
      << 1.79868000000000e-10f << endr
      << 1.67314000000000e-10f << endr
      << 1.54760000000000e-10f << endr;
 zFcn = tzFcn;
}
#ifdef FULLCMF

/*  colorMatchFcn  Popular color matching functions.

    Returns the color matching functions XFCN, YFCN, and ZFCN at
  each wavelength in the vector LAMBDA.  FORMULARY is a string
  specifying which set of color matching functions to return.
  Supported color matching functions are given below:

    CIE_1931   CIE 1931 2-degree, XYZ
    1931_FULL  CIE 1931 2-degree, XYZ  (at 1nm resolution)
    CIE_1964   CIE 1964 10-degree, XYZ
    1964_FULL  CIE 1964 10-degree, XYZ (at 1nm resolution)
    Judd       CIE 1931 2-degree, XYZ modified by Judd (1951)
    Judd_Vos   CIE 1931 2-degree, XYZ modified by Judd (1951) and Vos (1978)
    Stiles_2   Stiles and Burch 2-degree, RGB (1955)
    Stiles_10  Stiles and Burch 10-degree, RGB (1959)

    Reference: http://cvrl.ioo.ucl.ac.uk/cmfs.htm

    See also illuminant.

  Copyright 1993-2005 The MathWorks, Inc.
*/


void MKCC::CMF_JUDD_VOS(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
    fmat::fixed<446,4> cmf;
  cmf << 380.f << 0.00268990000000000f  << 0.000200000000000000f << 0.0122600000000000f   << endr
      << 381.f << 0.00321402000000000f  << 0.000239112000000000f << 0.0146524000000000f   << endr
      << 382.f << 0.00373814000000000f  << 0.000278224000000000f << 0.0170448000000000f   << endr
      << 383.f << 0.00426226000000000f  << 0.000317336000000000f << 0.0194372000000000f   << endr
      << 384.f << 0.00478638000000000f  << 0.000356448000000000f << 0.0218296000000000f   << endr
      << 385.f << 0.00531050000000000f  << 0.000395560000000000f << 0.0242220000000000f   << endr
      << 386.f << 0.00640460000000000f  << 0.000476448000000000f << 0.0292276000000000f   << endr
      << 387.f << 0.00749870000000000f  << 0.000557336000000000f << 0.0342332000000000f   << endr
      << 388.f << 0.00859280000000000f  << 0.000638224000000000f << 0.0392388000000000f   << endr
      << 389.f << 0.00968690000000000f  << 0.000719112000000000f << 0.0442444000000000f   << endr
      << 390.f << 0.0107810000000000f   << 0.000800000000000000f << 0.0492500000000000f   << endr
      << 391.f << 0.0127832000000000f   << 0.000949140000000000f << 0.0584270000000000f   << endr
      << 392.f << 0.0147854000000000f   << 0.00109828000000000f  << 0.0676040000000000f   << endr
      << 393.f << 0.0167876000000000f   << 0.00124742000000000f  << 0.0767810000000000f   << endr
      << 394.f << 0.0187898000000000f   << 0.00139656000000000f  << 0.0859580000000000f   << endr
      << 395.f << 0.0207920000000000f   << 0.00154570000000000f  << 0.0951350000000000f   << endr
      << 396.f << 0.0242298000000000f   << 0.00179656000000000f  << 0.110926000000000f    << endr
      << 397.f << 0.0276676000000000f   << 0.00204742000000000f  << 0.126717000000000f    << endr
      << 398.f << 0.0311054000000000f   << 0.00229828000000000f  << 0.142508000000000f    << endr
      << 399.f << 0.0345432000000000f   << 0.00254914000000000f  << 0.158299000000000f    << endr
      << 400.f << 0.0379810000000000f   << 0.00280000000000000f  << 0.174090000000000f    << endr
      << 401.f << 0.0430162000000000f   << 0.00317124000000000f  << 0.197298000000000f    << endr
      << 402.f << 0.0480514000000000f   << 0.00354248000000000f  << 0.220506000000000f    << endr
      << 403.f << 0.0530866000000000f   << 0.00391372000000000f  << 0.243714000000000f    << endr
      << 404.f << 0.0581218000000000f   << 0.00428496000000000f  << 0.266922000000000f    << endr
      << 405.f << 0.0631570000000000f   << 0.00465620000000000f  << 0.290130000000000f    << endr
      << 406.f << 0.0705138000000000f   << 0.00520496000000000f  << 0.324210000000000f    << endr
      << 407.f << 0.0778706000000000f   << 0.00575372000000000f  << 0.358290000000000f    << endr
      << 408.f << 0.0852274000000000f   << 0.00630248000000000f  << 0.392370000000000f    << endr
      << 409.f << 0.0925842000000000f   << 0.00685124000000000f  << 0.426450000000000f    << endr
      << 410.f << 0.0999410000000000f   << 0.00740000000000000f  << 0.460530000000000f    << endr
      << 411.f << 0.111600800000000f    << 0.00827580000000000f  << 0.514756000000000f    << endr
      << 412.f << 0.123260600000000f    << 0.00915160000000000f  << 0.568982000000000f    << endr
      << 413.f << 0.134920400000000f    << 0.0100274000000000f   << 0.623208000000000f    << endr
      << 414.f << 0.146580200000000f    << 0.0109032000000000f   << 0.677434000000000f    << endr
      << 415.f << 0.158240000000000f    << 0.0117790000000000f   << 0.731660000000000f    << endr
      << 416.f << 0.172488000000000f    << 0.0129232000000000f   << 0.798488000000000f    << endr
      << 417.f << 0.186736000000000f    << 0.0140674000000000f   << 0.865316000000000f    << endr
      << 418.f << 0.200984000000000f    << 0.0152116000000000f   << 0.932144000000000f    << endr
      << 419.f << 0.215232000000000f    << 0.0163558000000000f   << 0.998972000000000f    << endr
      << 420.f << 0.229480000000000f    << 0.0175000000000000f   << 1.06580000000000f     << endr
      << 421.f << 0.239800000000000f    << 0.0185356000000000f   << 1.11556000000000f     << endr
      << 422.f << 0.250120000000000f    << 0.0195712000000000f   << 1.16532000000000f     << endr
      << 423.f << 0.260440000000000f    << 0.0206068000000000f   << 1.21508000000000f     << endr
      << 424.f << 0.270760000000000f    << 0.0216424000000000f   << 1.26484000000000f     << endr
      << 425.f << 0.281080000000000f    << 0.0226780000000000f   << 1.31460000000000f     << endr
      << 426.f << 0.287054000000000f    << 0.0236024000000000f   << 1.34512000000000f     << endr
      << 427.f << 0.293028000000000f    << 0.0245268000000000f   << 1.37564000000000f     << endr
      << 428.f << 0.299002000000000f    << 0.0254512000000000f   << 1.40616000000000f     << endr
      << 429.f << 0.304976000000000f    << 0.0263756000000000f   << 1.43668000000000f     << endr
      << 430.f << 0.310950000000000f    << 0.0273000000000000f   << 1.46720000000000f     << endr
      << 431.f << 0.314904000000000f    << 0.0283568000000000f   << 1.48968000000000f     << endr
      << 432.f << 0.318858000000000f    << 0.0294136000000000f   << 1.51216000000000f     << endr
      << 433.f << 0.322812000000000f    << 0.0304704000000000f   << 1.53464000000000f     << endr
      << 434.f << 0.326766000000000f    << 0.0315272000000000f   << 1.55712000000000f     << endr
      << 435.f << 0.330720000000000f    << 0.0325840000000000f   << 1.57960000000000f     << endr
      << 436.f << 0.331248000000000f    << 0.0336472000000000f   << 1.58700000000000f     << endr
      << 437.f << 0.331776000000000f    << 0.0347104000000000f   << 1.59440000000000f     << endr
      << 438.f << 0.332304000000000f    << 0.0357736000000000f   << 1.60180000000000f     << endr
      << 439.f << 0.332832000000000f    << 0.0368368000000000f   << 1.60920000000000f     << endr
      << 440.f << 0.333360000000000f    << 0.0379000000000000f   << 1.61660000000000f     << endr
      << 441.f << 0.330032000000000f    << 0.0387982000000000f   << 1.60692000000000f     << endr
      << 442.f << 0.326704000000000f    << 0.0396964000000000f   << 1.59724000000000f     << endr
      << 443.f << 0.323376000000000f    << 0.0405946000000000f   << 1.58756000000000f     << endr
      << 444.f << 0.320048000000000f    << 0.0414928000000000f   << 1.57788000000000f     << endr
      << 445.f << 0.316720000000000f    << 0.0423910000000000f   << 1.56820000000000f     << endr
      << 446.f << 0.311140000000000f    << 0.0432728000000000f   << 1.54890000000000f     << endr
      << 447.f << 0.305560000000000f    << 0.0441546000000000f   << 1.52960000000000f     << endr
      << 448.f << 0.299980000000000f    << 0.0450364000000000f   << 1.51030000000000f     << endr
      << 449.f << 0.294400000000000f    << 0.0459182000000000f   << 1.49100000000000f     << endr
      << 450.f << 0.288820000000000f    << 0.0468000000000000f   << 1.47170000000000f     << endr
      << 451.f << 0.282994000000000f    << 0.0478644000000000f   << 1.45216000000000f     << endr
      << 452.f << 0.277168000000000f    << 0.0489288000000000f   << 1.43262000000000f     << endr
      << 453.f << 0.271342000000000f    << 0.0499932000000000f   << 1.41308000000000f     << endr
      << 454.f << 0.265516000000000f    << 0.0510576000000000f   << 1.39354000000000f     << endr
      << 455.f << 0.259690000000000f    << 0.0521220000000000f   << 1.37400000000000f     << endr
      << 456.f << 0.254304000000000f    << 0.0536976000000000f   << 1.35754000000000f     << endr
      << 457.f << 0.248918000000000f    << 0.0552732000000000f   << 1.34108000000000f     << endr
      << 458.f << 0.243532000000000f    << 0.0568488000000000f   << 1.32462000000000f     << endr
      << 459.f << 0.238146000000000f    << 0.0584244000000000f   << 1.30816000000000f     << endr
      << 460.f << 0.232760000000000f    << 0.0600000000000000f   << 1.29170000000000f     << endr
      << 461.f << 0.228206000000000f    << 0.0625884000000000f   << 1.28048000000000f     << endr
      << 462.f << 0.223652000000000f    << 0.0651768000000000f   << 1.26926000000000f     << endr
      << 463.f << 0.219098000000000f    << 0.0677652000000000f   << 1.25804000000000f     << endr
      << 464.f << 0.214544000000000f    << 0.0703536000000000f   << 1.24682000000000f     << endr
      << 465.f << 0.209990000000000f    << 0.0729420000000000f   << 1.23560000000000f     << endr
      << 466.f << 0.202944000000000f    << 0.0765496000000000f   << 1.21124000000000f     << endr
      << 467.f << 0.195898000000000f    << 0.0801572000000000f   << 1.18688000000000f     << endr
      << 468.f << 0.188852000000000f    << 0.0837648000000000f   << 1.16252000000000f     << endr
      << 469.f << 0.181806000000000f    << 0.0873724000000000f   << 1.13816000000000f     << endr
      << 470.f << 0.174760000000000f    << 0.0909800000000000f   << 1.11380000000000f     << endr
      << 471.f << 0.166382000000000f    << 0.0953520000000000f   << 1.07948000000000f     << endr
      << 472.f << 0.158004000000000f    << 0.0997240000000000f   << 1.04516000000000f     << endr
      << 473.f << 0.149626000000000f    << 0.104096000000000f    << 1.01084000000000f     << endr
      << 474.f << 0.141248000000000f    << 0.108468000000000f    << 0.976520000000000f    << endr
      << 475.f << 0.132870000000000f    << 0.112840000000000f    << 0.942200000000000f    << endr
      << 476.f << 0.124684800000000f    << 0.118076000000000f    << 0.904952000000000f    << endr
      << 477.f << 0.116499600000000f    << 0.123312000000000f    << 0.867704000000000f    << endr
      << 478.f << 0.108314400000000f    << 0.128548000000000f    << 0.830456000000000f    << endr
      << 479.f << 0.100129200000000f    << 0.133784000000000f    << 0.793208000000000f    << endr
      << 480.f << 0.0919440000000000f   << 0.139020000000000f    << 0.755960000000000f    << endr
      << 481.f << 0.0849522000000000f   << 0.145190000000000f    << 0.722048000000000f    << endr
      << 482.f << 0.0779604000000000f   << 0.151360000000000f    << 0.688136000000000f    << endr
      << 483.f << 0.0709686000000000f   << 0.157530000000000f    << 0.654224000000000f    << endr
      << 484.f << 0.0639768000000000f   << 0.163700000000000f    << 0.620312000000000f    << endr
      << 485.f << 0.0569850000000000f   << 0.169870000000000f    << 0.586400000000000f    << endr
      << 486.f << 0.0519342000000000f   << 0.177500000000000f    << 0.558458000000000f    << endr
      << 487.f << 0.0468834000000000f   << 0.185130000000000f    << 0.530516000000000f    << endr
      << 488.f << 0.0418326000000000f   << 0.192760000000000f    << 0.502574000000000f    << endr
      << 489.f << 0.0367818000000000f   << 0.200390000000000f    << 0.474632000000000f    << endr
      << 490.f << 0.0317310000000000f   << 0.208020000000000f    << 0.446690000000000f    << endr
      << 491.f << 0.0283074000000000f   << 0.218032000000000f    << 0.425584000000000f    << endr
      << 492.f << 0.0248838000000000f   << 0.228044000000000f    << 0.404478000000000f    << endr
      << 493.f << 0.0214602000000000f   << 0.238056000000000f    << 0.383372000000000f    << endr
      << 494.f << 0.0180366000000000f   << 0.248068000000000f    << 0.362266000000000f    << endr
      << 495.f << 0.0146130000000000f   << 0.258080000000000f    << 0.341160000000000f    << endr
      << 496.f << 0.0126602200000000f   << 0.271064000000000f    << 0.325802000000000f    << endr
      << 497.f << 0.0107074400000000f   << 0.284048000000000f    << 0.310444000000000f    << endr
      << 498.f << 0.00875466000000000f  << 0.297032000000000f    << 0.295086000000000f    << endr
      << 499.f << 0.00680188000000000f  << 0.310016000000000f    << 0.279728000000000f    << endr
      << 500.f << 0.00484910000000000f  << 0.323000000000000f    << 0.264370000000000f    << endr
      << 501.f << 0.00434358000000000f  << 0.339480000000000f    << 0.252684000000000f    << endr
      << 502.f << 0.00383806000000000f  << 0.355960000000000f    << 0.240998000000000f    << endr
      << 503.f << 0.00333254000000000f  << 0.372440000000000f    << 0.229312000000000f    << endr
      << 504.f << 0.00282702000000000f  << 0.388920000000000f    << 0.217626000000000f    << endr
      << 505.f << 0.00232150000000000f  << 0.405400000000000f    << 0.205940000000000f    << endr
      << 506.f << 0.00371518000000000f  << 0.424920000000000f    << 0.195642000000000f    << endr
      << 507.f << 0.00510886000000000f  << 0.444440000000000f    << 0.185344000000000f    << endr
      << 508.f << 0.00650254000000000f  << 0.463960000000000f    << 0.175046000000000f    << endr
      << 509.f << 0.00789622000000000f  << 0.483480000000000f    << 0.164748000000000f    << endr
      << 510.f << 0.00928990000000000f  << 0.503000000000000f    << 0.154450000000000f    << endr
      << 511.f << 0.0132875200000000f   << 0.524022000000000f    << 0.145396000000000f    << endr
      << 512.f << 0.0172851400000000f   << 0.545044000000000f    << 0.136342000000000f    << endr
      << 513.f << 0.0212827600000000f   << 0.566066000000000f    << 0.127288000000000f    << endr
      << 514.f << 0.0252803800000000f   << 0.587088000000000f    << 0.118234000000000f    << endr
      << 515.f << 0.0292780000000000f   << 0.608110000000000f    << 0.109180000000000f    << endr
      << 516.f << 0.0361806000000000f   << 0.628488000000000f    << 0.102661000000000f    << endr
      << 517.f << 0.0430832000000000f   << 0.648866000000000f    << 0.0961420000000000f   << endr
      << 518.f << 0.0499858000000000f   << 0.669244000000000f    << 0.0896230000000000f   << endr
      << 519.f << 0.0568884000000000f   << 0.689622000000000f    << 0.0831040000000000f   << endr
      << 520.f << 0.0637910000000000f   << 0.710000000000000f    << 0.0765850000000000f   << endr
      << 521.f << 0.0731948000000000f   << 0.727020000000000f    << 0.0725134000000000f   << endr
      << 522.f << 0.0825986000000000f   << 0.744040000000000f    << 0.0684418000000000f   << endr
      << 523.f << 0.0920024000000000f   << 0.761060000000000f    << 0.0643702000000000f   << endr
      << 524.f << 0.101406200000000f    << 0.778080000000000f    << 0.0602986000000000f   << endr
      << 525.f << 0.110810000000000f    << 0.795100000000000f    << 0.0562270000000000f   << endr
      << 526.f << 0.122032000000000f    << 0.808480000000000f    << 0.0532548000000000f   << endr
      << 527.f << 0.133254000000000f    << 0.821860000000000f    << 0.0502826000000000f   << endr
      << 528.f << 0.144476000000000f    << 0.835240000000000f    << 0.0473104000000000f   << endr
      << 529.f << 0.155698000000000f    << 0.848620000000000f    << 0.0443382000000000f   << endr
      << 530.f << 0.166920000000000f    << 0.862000000000000f    << 0.0413660000000000f   << endr
      << 531.f << 0.179072000000000f    << 0.872610000000000f    << 0.0389634000000000f   << endr
      << 532.f << 0.191224000000000f    << 0.883220000000000f    << 0.0365608000000000f   << endr
      << 533.f << 0.203376000000000f    << 0.893830000000000f    << 0.0341582000000000f   << endr
      << 534.f << 0.215528000000000f    << 0.904440000000000f    << 0.0317556000000000f   << endr
      << 535.f << 0.227680000000000f    << 0.915050000000000f    << 0.0293530000000000f   << endr
      << 536.f << 0.240682000000000f    << 0.922840000000000f    << 0.0274908000000000f   << endr
      << 537.f << 0.253684000000000f    << 0.930630000000000f    << 0.0256286000000000f   << endr
      << 538.f << 0.266686000000000f    << 0.938420000000000f    << 0.0237664000000000f   << endr
      << 539.f << 0.279688000000000f    << 0.946210000000000f    << 0.0219042000000000f   << endr
      << 540.f << 0.292690000000000f    << 0.954000000000000f    << 0.0200420000000000f   << endr
      << 541.f << 0.306602000000000f    << 0.959208000000000f    << 0.0186960000000000f   << endr
      << 542.f << 0.320514000000000f    << 0.964416000000000f    << 0.0173500000000000f   << endr
      << 543.f << 0.334426000000000f    << 0.969624000000000f    << 0.0160040000000000f   << endr
      << 544.f << 0.348338000000000f    << 0.974832000000000f    << 0.0146580000000000f   << endr
      << 545.f << 0.362250000000000f    << 0.980040000000000f    << 0.0133120000000000f   << endr
      << 546.f << 0.377070000000000f    << 0.983022000000000f    << 0.0124060600000000f   << endr
      << 547.f << 0.391890000000000f    << 0.986004000000000f    << 0.0115001200000000f   << endr
      << 548.f << 0.406710000000000f    << 0.988986000000000f    << 0.0105941800000000f   << endr
      << 549.f << 0.421530000000000f    << 0.991968000000000f    << 0.00968824000000000f  << endr
      << 550.f << 0.436350000000000f    << 0.994950000000000f    << 0.00878230000000000f  << endr
      << 551.f << 0.452106000000000f    << 0.995980000000000f    << 0.00819730000000000f  << endr
      << 552.f << 0.467862000000000f    << 0.997010000000000f    << 0.00761230000000000f  << endr
      << 553.f << 0.483618000000000f    << 0.998040000000000f    << 0.00702730000000000f  << endr
      << 554.f << 0.499374000000000f    << 0.999070000000000f    << 0.00644230000000000f  << endr
      << 555.f << 0.515130000000000f    << 1.00010000000000f     << 0.00585730000000000f  << endr
      << 556.f << 0.531600000000000f    << 0.999080000000000f    << 0.00549570000000000f  << endr
      << 557.f << 0.548070000000000f    << 0.998060000000000f    << 0.00513410000000000f  << endr
      << 558.f << 0.564540000000000f    << 0.997040000000000f    << 0.00477250000000000f  << endr
      << 559.f << 0.581010000000000f    << 0.996020000000000f    << 0.00441090000000000f  << endr
      << 560.f << 0.597480000000000f    << 0.995000000000000f    << 0.00404930000000000f  << endr
      << 561.f << 0.614226000000000f    << 0.991750000000000f    << 0.00382378000000000f  << endr
      << 562.f << 0.630972000000000f    << 0.988500000000000f    << 0.00359826000000000f  << endr
      << 563.f << 0.647718000000000f    << 0.985250000000000f    << 0.00337274000000000f  << endr
      << 564.f << 0.664464000000000f    << 0.982000000000000f    << 0.00314722000000000f  << endr
      << 565.f << 0.681210000000000f    << 0.978750000000000f    << 0.00292170000000000f  << endr
      << 566.f << 0.697818000000000f    << 0.973400000000000f    << 0.00279278000000000f  << endr
      << 567.f << 0.714426000000000f    << 0.968050000000000f    << 0.00266386000000000f  << endr
      << 568.f << 0.731034000000000f    << 0.962700000000000f    << 0.00253494000000000f  << endr
      << 569.f << 0.747642000000000f    << 0.957350000000000f    << 0.00240602000000000f  << endr
      << 570.f << 0.764250000000000f    << 0.952000000000000f    << 0.00227710000000000f  << endr
      << 571.f << 0.780188000000000f    << 0.944716000000000f    << 0.00221580000000000f  << endr
      << 572.f << 0.796126000000000f    << 0.937432000000000f    << 0.00215450000000000f  << endr
      << 573.f << 0.812064000000000f    << 0.930148000000000f    << 0.00209320000000000f  << endr
      << 574.f << 0.828002000000000f    << 0.922864000000000f    << 0.00203190000000000f  << endr
      << 575.f << 0.843940000000000f    << 0.915580000000000f    << 0.00197060000000000f  << endr
      << 576.f << 0.858422000000000f    << 0.906464000000000f    << 0.00193780000000000f  << endr
      << 577.f << 0.872904000000000f    << 0.897348000000000f    << 0.00190500000000000f  << endr
      << 578.f << 0.887386000000000f    << 0.888232000000000f    << 0.00187220000000000f  << endr
      << 579.f << 0.901868000000000f    << 0.879116000000000f    << 0.00183940000000000f  << endr
      << 580.f << 0.916350000000000f    << 0.870000000000000f    << 0.00180660000000000f  << endr
      << 581.f << 0.928486000000000f    << 0.859246000000000f    << 0.00175426000000000f  << endr
      << 582.f << 0.940622000000000f    << 0.848492000000000f    << 0.00170192000000000f  << endr
      << 583.f << 0.952758000000000f    << 0.837738000000000f    << 0.00164958000000000f  << endr
      << 584.f << 0.964894000000000f    << 0.826984000000000f    << 0.00159724000000000f  << endr
      << 585.f << 0.977030000000000f    << 0.816230000000000f    << 0.00154490000000000f  << endr
      << 586.f << 0.986224000000000f    << 0.804384000000000f    << 0.00148288000000000f  << endr
      << 587.f << 0.995418000000000f    << 0.792538000000000f    << 0.00142086000000000f  << endr
      << 588.f << 1.00461200000000f     << 0.780692000000000f    << 0.00135884000000000f  << endr
      << 589.f << 1.01380600000000f     << 0.768846000000000f    << 0.00129682000000000f  << endr
      << 590.f << 1.02300000000000f     << 0.757000000000000f    << 0.00123480000000000f  << endr
      << 591.f << 1.02866000000000f     << 0.744566000000000f    << 0.00121138000000000f  << endr
      << 592.f << 1.03432000000000f     << 0.732132000000000f    << 0.00118796000000000f  << endr
      << 593.f << 1.03998000000000f     << 0.719698000000000f    << 0.00116454000000000f  << endr
      << 594.f << 1.04564000000000f     << 0.707264000000000f    << 0.00114112000000000f  << endr
      << 595.f << 1.05130000000000f     << 0.694830000000000f    << 0.00111770000000000f  << endr
      << 596.f << 1.05204000000000f     << 0.682064000000000f    << 0.00107528800000000f  << endr
      << 597.f << 1.05278000000000f     << 0.669298000000000f    << 0.00103287600000000f  << endr
      << 598.f << 1.05352000000000f     << 0.656532000000000f    << 0.000990464000000000f << endr
      << 599.f << 1.05426000000000f     << 0.643766000000000f    << 0.000948052000000000f << endr
      << 600.f << 1.05500000000000f     << 0.631000000000000f    << 0.000905640000000000f << endr
      << 601.f << 1.05124000000000f     << 0.618108000000000f    << 0.000863446000000000f << endr
      << 602.f << 1.04748000000000f     << 0.605216000000000f    << 0.000821252000000000f << endr
      << 603.f << 1.04372000000000f     << 0.592324000000000f    << 0.000779058000000000f << endr
      << 604.f << 1.03996000000000f     << 0.579432000000000f    << 0.000736864000000000f << endr
      << 605.f << 1.03620000000000f     << 0.566540000000000f    << 0.000694670000000000f << endr
      << 606.f << 1.02743800000000f     << 0.553832000000000f    << 0.000641506000000000f << endr
      << 607.f << 1.01867600000000f     << 0.541124000000000f    << 0.000588342000000000f << endr
      << 608.f << 1.00991400000000f     << 0.528416000000000f    << 0.000535178000000000f << endr
      << 609.f << 1.00115200000000f     << 0.515708000000000f    << 0.000482014000000000f << endr
      << 610.f << 0.992390000000000f    << 0.503000000000000f    << 0.000428850000000000f << endr
      << 611.f << 0.979634000000000f    << 0.490744000000000f    << 0.000406714000000000f << endr
      << 612.f << 0.966878000000000f    << 0.478488000000000f    << 0.000384578000000000f << endr
      << 613.f << 0.954122000000000f    << 0.466232000000000f    << 0.000362442000000000f << endr
      << 614.f << 0.941366000000000f    << 0.453976000000000f    << 0.000340306000000000f << endr
      << 615.f << 0.928610000000000f    << 0.441720000000000f    << 0.000318170000000000f << endr
      << 616.f << 0.911580000000000f    << 0.429576000000000f    << 0.000305732000000000f << endr
      << 617.f << 0.894550000000000f    << 0.417432000000000f    << 0.000293294000000000f << endr
      << 618.f << 0.877520000000000f    << 0.405288000000000f    << 0.000280856000000000f << endr
      << 619.f << 0.860490000000000f    << 0.393144000000000f    << 0.000268418000000000f << endr
      << 620.f << 0.843460000000000f    << 0.381000000000000f    << 0.000255980000000000f << endr
      << 621.f << 0.822734000000000f    << 0.368904000000000f    << 0.000236142000000000f << endr
      << 622.f << 0.802008000000000f    << 0.356808000000000f    << 0.000216304000000000f << endr
      << 623.f << 0.781282000000000f    << 0.344712000000000f    << 0.000196466000000000f << endr
      << 624.f << 0.760556000000000f    << 0.332616000000000f    << 0.000176628000000000f << endr
      << 625.f << 0.739830000000000f    << 0.320520000000000f    << 0.000156790000000000f << endr
      << 626.f << 0.718442000000000f    << 0.309416000000000f    << 0.000144970800000000f << endr
      << 627.f << 0.697054000000000f    << 0.298312000000000f    << 0.000133151600000000f << endr
      << 628.f << 0.675666000000000f    << 0.287208000000000f    << 0.000121332400000000f << endr
      << 629.f << 0.654278000000000f    << 0.276104000000000f    << 0.000109513200000000f << endr
      << 630.f << 0.632890000000000f    << 0.265000000000000f    << 9.76940000000000e-05f << endr
      << 631.f << 0.613014000000000f    << 0.255404000000000f    << 9.19440000000000e-05f << endr
      << 632.f << 0.593138000000000f    << 0.245808000000000f    << 8.61940000000000e-05f << endr
      << 633.f << 0.573262000000000f    << 0.236212000000000f    << 8.04440000000000e-05f << endr
      << 634.f << 0.553386000000000f    << 0.226616000000000f    << 7.46940000000000e-05f << endr
      << 635.f << 0.533510000000000f    << 0.217020000000000f    << 6.89440000000000e-05f << endr
      << 636.f << 0.514932000000000f    << 0.208616000000000f    << 6.53882000000000e-05f << endr
      << 637.f << 0.496354000000000f    << 0.200212000000000f    << 6.18324000000000e-05f << endr
      << 638.f << 0.477776000000000f    << 0.191808000000000f    << 5.82766000000000e-05f << endr
      << 639.f << 0.459198000000000f    << 0.183404000000000f    << 5.47208000000000e-05f << endr
      << 640.f << 0.440620000000000f    << 0.175000000000000f    << 5.11650000000000e-05f << endr
      << 641.f << 0.423402000000000f    << 0.167624000000000f    << 4.81352000000000e-05f << endr
      << 642.f << 0.406184000000000f    << 0.160248000000000f    << 4.51054000000000e-05f << endr
      << 643.f << 0.388966000000000f    << 0.152872000000000f    << 4.20756000000000e-05f << endr
      << 644.f << 0.371748000000000f    << 0.145496000000000f    << 3.90458000000000e-05f << endr
      << 645.f << 0.354530000000000f    << 0.138120000000000f    << 3.60160000000000e-05f << endr
      << 646.f << 0.339348000000000f    << 0.131896000000000f    << 3.36604000000000e-05f << endr
      << 647.f << 0.324166000000000f    << 0.125672000000000f    << 3.13048000000000e-05f << endr
      << 648.f << 0.308984000000000f    << 0.119448000000000f    << 2.89492000000000e-05f << endr
      << 649.f << 0.293802000000000f    << 0.113224000000000f    << 2.65936000000000e-05f << endr
      << 650.f << 0.278620000000000f    << 0.107000000000000f    << 2.42380000000000e-05f << endr
      << 651.f << 0.265866000000000f    << 0.101930400000000f    << 2.27734000000000e-05f << endr
      << 652.f << 0.253112000000000f    << 0.0968608000000000f   << 2.13088000000000e-05f << endr
      << 653.f << 0.240358000000000f    << 0.0917912000000000f   << 1.98442000000000e-05f << endr
      << 654.f << 0.227604000000000f    << 0.0867216000000000f   << 1.83796000000000e-05f << endr
      << 655.f << 0.214850000000000f    << 0.0816520000000000f   << 1.69150000000000e-05f << endr
      << 656.f << 0.204202000000000f    << 0.0775216000000000f   << 1.59132000000000e-05f << endr
      << 657.f << 0.193554000000000f    << 0.0733912000000000f   << 1.49114000000000e-05f << endr
      << 658.f << 0.182906000000000f    << 0.0692608000000000f   << 1.39096000000000e-05f << endr
      << 659.f << 0.172258000000000f    << 0.0651304000000000f   << 1.29078000000000e-05f << endr
      << 660.f << 0.161610000000000f    << 0.0610000000000000f   << 1.19060000000000e-05f << endr
      << 661.f << 0.152928000000000f    << 0.0576654000000000f   << 1.11545800000000e-05f << endr
      << 662.f << 0.144246000000000f    << 0.0543308000000000f   << 1.04031600000000e-05f << endr
      << 663.f << 0.135564000000000f    << 0.0509962000000000f   << 9.65174000000000e-06f << endr
      << 664.f << 0.126882000000000f    << 0.0476616000000000f   << 8.90032000000000e-06f << endr
      << 665.f << 0.118200000000000f    << 0.0443270000000000f   << 8.14890000000000e-06f << endr
      << 666.f << 0.111710600000000f    << 0.0418616000000000f   << 7.63924000000000e-06f << endr
      << 667.f << 0.105221200000000f    << 0.0393962000000000f   << 7.12958000000000e-06f << endr
      << 668.f << 0.0987318000000000f   << 0.0369308000000000f   << 6.61992000000000e-06f << endr
      << 669.f << 0.0922424000000000f   << 0.0344654000000000f   << 6.11026000000000e-06f << endr
      << 670.f << 0.0857530000000000f   << 0.0320000000000000f   << 5.60060000000000e-06f << endr
      << 671.f << 0.0812178000000000f   << 0.0302908000000000f   << 5.27136000000000e-06f << endr
      << 672.f << 0.0766826000000000f   << 0.0285816000000000f   << 4.94212000000000e-06f << endr
      << 673.f << 0.0721474000000000f   << 0.0268724000000000f   << 4.61288000000000e-06f << endr
      << 674.f << 0.0676122000000000f   << 0.0251632000000000f   << 4.28364000000000e-06f << endr
      << 675.f << 0.0630770000000000f   << 0.0234540000000000f   << 3.95440000000000e-06f << endr
      << 676.f << 0.0596284000000000f   << 0.0221632000000000f   << 3.72176000000000e-06f << endr
      << 677.f << 0.0561798000000000f   << 0.0208724000000000f   << 3.48912000000000e-06f << endr
      << 678.f << 0.0527312000000000f   << 0.0195816000000000f   << 3.25648000000000e-06f << endr
      << 679.f << 0.0492826000000000f   << 0.0182908000000000f   << 3.02384000000000e-06f << endr
      << 680.f << 0.0458340000000000f   << 0.0170000000000000f   << 2.79120000000000e-06f << endr
      << 681.f << 0.0430786000000000f   << 0.0159744000000000f   << 2.61648000000000e-06f << endr
      << 682.f << 0.0403232000000000f   << 0.0149488000000000f   << 2.44176000000000e-06f << endr
      << 683.f << 0.0375678000000000f   << 0.0139232000000000f   << 2.26704000000000e-06f << endr
      << 684.f << 0.0348124000000000f   << 0.0128976000000000f   << 2.09232000000000e-06f << endr
      << 685.f << 0.0320570000000000f   << 0.0118720000000000f   << 1.91760000000000e-06f << endr
      << 686.f << 0.0300830000000000f   << 0.0111396000000000f   << 1.79678000000000e-06f << endr
      << 687.f << 0.0281090000000000f   << 0.0104072000000000f   << 1.67596000000000e-06f << endr
      << 688.f << 0.0261350000000000f   << 0.00967480000000000f  << 1.55514000000000e-06f << endr
      << 689.f << 0.0241610000000000f   << 0.00894240000000000f  << 1.43432000000000e-06f << endr
      << 690.f << 0.0221870000000000f   << 0.00821000000000000f  << 1.31350000000000e-06f << endr
      << 691.f << 0.0208720000000000f   << 0.00772246000000000f  << 1.23383800000000e-06f << endr
      << 692.f << 0.0195570000000000f   << 0.00723492000000000f  << 1.15417600000000e-06f << endr
      << 693.f << 0.0182420000000000f   << 0.00674738000000000f  << 1.07451400000000e-06f << endr
      << 694.f << 0.0169270000000000f   << 0.00625984000000000f  << 9.94852000000000e-07f << endr
      << 695.f << 0.0156120000000000f   << 0.00577230000000000f  << 9.15190000000000e-07f << endr
      << 696.f << 0.0147092000000000f   << 0.00543824000000000f  << 8.61686000000000e-07f << endr
      << 697.f << 0.0138064000000000f   << 0.00510418000000000f  << 8.08182000000000e-07f << endr
      << 698.f << 0.0129036000000000f   << 0.00477012000000000f  << 7.54678000000000e-07f << endr
      << 699.f << 0.0120008000000000f   << 0.00443606000000000f  << 7.01174000000000e-07f << endr
      << 700.f << 0.0110980000000000f   << 0.00410200000000000f  << 6.47670000000000e-07f << endr
      << 701.f << 0.0104630600000000f   << 0.00386742000000000f  << 6.10840000000000e-07f << endr
      << 702.f << 0.00982812000000000f  << 0.00363284000000000f  << 5.74010000000000e-07f << endr
      << 703.f << 0.00919318000000000f  << 0.00339826000000000f  << 5.37180000000000e-07f << endr
      << 704.f << 0.00855824000000000f  << 0.00316368000000000f  << 5.00350000000000e-07f << endr
      << 705.f << 0.00792330000000000f  << 0.00292910000000000f  << 4.63520000000000e-07f << endr
      << 706.f << 0.00746926000000000f  << 0.00276148000000000f  << 4.37424000000000e-07f << endr
      << 707.f << 0.00701522000000000f  << 0.00259386000000000f  << 4.11328000000000e-07f << endr
      << 708.f << 0.00656118000000000f  << 0.00242624000000000f  << 3.85232000000000e-07f << endr
      << 709.f << 0.00610714000000000f  << 0.00225862000000000f  << 3.59136000000000e-07f << endr
      << 710.f << 0.00565310000000000f  << 0.00209100000000000f  << 3.33040000000000e-07f << endr
      << 711.f << 0.00532326000000000f  << 0.00196924000000000f  << 3.14078000000000e-07f << endr
      << 712.f << 0.00499342000000000f  << 0.00184748000000000f  << 2.95116000000000e-07f << endr
      << 713.f << 0.00466358000000000f  << 0.00172572000000000f  << 2.76154000000000e-07f << endr
      << 714.f << 0.00433374000000000f  << 0.00160396000000000f  << 2.57192000000000e-07f << endr
      << 715.f << 0.00400390000000000f  << 0.00148220000000000f  << 2.38230000000000e-07f << endr
      << 716.f << 0.00376818000000000f  << 0.00139516000000000f  << 2.24636000000000e-07f << endr
      << 717.f << 0.00353246000000000f  << 0.00130812000000000f  << 2.11042000000000e-07f << endr
      << 718.f << 0.00329674000000000f  << 0.00122108000000000f  << 1.97448000000000e-07f << endr
      << 719.f << 0.00306102000000000f  << 0.00113404000000000f  << 1.83854000000000e-07f << endr
      << 720.f << 0.00282530000000000f  << 0.00104700000000000f  << 1.70260000000000e-07f << endr
      << 721.f << 0.00265918000000000f  << 0.000985630000000000f << 1.60622000000000e-07f << endr
      << 722.f << 0.00249306000000000f  << 0.000924260000000000f << 1.50984000000000e-07f << endr
      << 723.f << 0.00232694000000000f  << 0.000862890000000000f << 1.41346000000000e-07f << endr
      << 724.f << 0.00216082000000000f  << 0.000801520000000000f << 1.31708000000000e-07f << endr
      << 725.f << 0.00199470000000000f  << 0.000740150000000000f << 1.22070000000000e-07f << endr
      << 726.f << 0.00187564000000000f  << 0.000696120000000000f << 1.15077400000000e-07f << endr
      << 727.f << 0.00175658000000000f  << 0.000652090000000000f << 1.08084800000000e-07f << endr
      << 728.f << 0.00163752000000000f  << 0.000608060000000000f << 1.01092200000000e-07f << endr
      << 729.f << 0.00151846000000000f  << 0.000564030000000000f << 9.40996000000000e-08f << endr
      << 730.f << 0.00139940000000000f  << 0.000520000000000000f << 8.71070000000000e-08f << endr
      << 731.f << 0.00131348000000000f  << 0.000488186000000000f << 8.19766000000000e-08f << endr
      << 732.f << 0.00122756000000000f  << 0.000456372000000000f << 7.68462000000000e-08f << endr
      << 733.f << 0.00114164000000000f  << 0.000424558000000000f << 7.17158000000000e-08f << endr
      << 734.f << 0.00105572000000000f  << 0.000392744000000000f << 6.65854000000000e-08f << endr
      << 735.f << 0.000969800000000000f << 0.000360930000000000f << 6.14550000000000e-08f << endr
      << 736.f << 0.000909534000000000f << 0.000338584000000000f << 5.77964000000000e-08f << endr
      << 737.f << 0.000849268000000000f << 0.000316238000000000f << 5.41378000000000e-08f << endr
      << 738.f << 0.000789002000000000f << 0.000293892000000000f << 5.04792000000000e-08f << endr
      << 739.f << 0.000728736000000000f << 0.000271546000000000f << 4.68206000000000e-08f << endr
      << 740.f << 0.000668470000000000f << 0.000249200000000000f << 4.31620000000000e-08f << endr
      << 741.f << 0.000627058000000000f << 0.000233822000000000f << 4.06054000000000e-08f << endr
      << 742.f << 0.000585646000000000f << 0.000218444000000000f << 3.80488000000000e-08f << endr
      << 743.f << 0.000544234000000000f << 0.000203066000000000f << 3.54922000000000e-08f << endr
      << 744.f << 0.000502822000000000f << 0.000187688000000000f << 3.29356000000000e-08f << endr
      << 745.f << 0.000461410000000000f << 0.000172310000000000f << 3.03790000000000e-08f << endr
      << 746.f << 0.000433274000000000f << 0.000161848000000000f << 2.86140000000000e-08f << endr
      << 747.f << 0.000405138000000000f << 0.000151386000000000f << 2.68490000000000e-08f << endr
      << 748.f << 0.000377002000000000f << 0.000140924000000000f << 2.50840000000000e-08f << endr
      << 749.f << 0.000348866000000000f << 0.000130462000000000f << 2.33190000000000e-08f << endr
      << 750.f << 0.000320730000000000f << 0.000120000000000000f << 2.15540000000000e-08f << endr
      << 751.f << 0.000301730000000000f << 0.000112924000000000f << 2.03418000000000e-08f << endr
      << 752.f << 0.000282730000000000f << 0.000105848000000000f << 1.91296000000000e-08f << endr
      << 753.f << 0.000263730000000000f << 9.87720000000000e-05f << 1.79174000000000e-08f << endr
      << 754.f << 0.000244730000000000f << 9.16960000000000e-05f << 1.67052000000000e-08f << endr
      << 755.f << 0.000225730000000000f << 8.46200000000000e-05f << 1.54930000000000e-08f << endr
      << 756.f << 0.000212530000000000f << 7.96960000000000e-05f << 1.46352000000000e-08f << endr
      << 757.f << 0.000199330000000000f << 7.47720000000000e-05f << 1.37774000000000e-08f << endr
      << 758.f << 0.000186130000000000f << 6.98480000000000e-05f << 1.29196000000000e-08f << endr
      << 759.f << 0.000172930000000000f << 6.49240000000000e-05f << 1.20618000000000e-08f << endr
      << 760.f << 0.000159730000000000f << 6.00000000000000e-05f << 1.12040000000000e-08f << endr
      << 761.f << 0.000150334000000000f << 5.64892000000000e-05f << 1.05806600000000e-08f << endr
      << 762.f << 0.000140938000000000f << 5.29784000000000e-05f << 9.95732000000000e-09f << endr
      << 763.f << 0.000131542000000000f << 4.94676000000000e-05f << 9.33398000000000e-09f << endr
      << 764.f << 0.000122146000000000f << 4.59568000000000e-05f << 8.71064000000000e-09f << endr
      << 765.f << 0.000112750000000000f << 4.24460000000000e-05f << 8.08730000000000e-09f << endr
      << 766.f << 0.000106102600000000f << 3.99568000000000e-05f << 7.63664000000000e-09f << endr
      << 767.f << 9.94552000000000e-05f << 3.74676000000000e-05f << 7.18598000000000e-09f << endr
      << 768.f << 9.28078000000000e-05f << 3.49784000000000e-05f << 6.73532000000000e-09f << endr
      << 769.f << 8.61604000000000e-05f << 3.24892000000000e-05f << 6.28466000000000e-09f << endr
      << 770.f << 7.95130000000000e-05f << 3.00000000000000e-05f << 5.83400000000000e-09f << endr
      << 771.f << 7.48278000000000e-05f << 2.82420000000000e-05f << 5.50940000000000e-09f << endr
      << 772.f << 7.01426000000000e-05f << 2.64840000000000e-05f << 5.18480000000000e-09f << endr
      << 773.f << 6.54574000000000e-05f << 2.47260000000000e-05f << 4.86020000000000e-09f << endr
      << 774.f << 6.07722000000000e-05f << 2.29680000000000e-05f << 4.53560000000000e-09f << endr
      << 775.f << 5.60870000000000e-05f << 2.12100000000000e-05f << 4.21100000000000e-09f << endr
      << 776.f << 5.27778000000000e-05f << 1.99658000000000e-05f << 3.97646000000000e-09f << endr
      << 777.f << 4.94686000000000e-05f << 1.87216000000000e-05f << 3.74192000000000e-09f << endr
      << 778.f << 4.61594000000000e-05f << 1.74774000000000e-05f << 3.50738000000000e-09f << endr
      << 779.f << 4.28502000000000e-05f << 1.62332000000000e-05f << 3.27284000000000e-09f << endr
      << 780.f << 3.95410000000000e-05f << 1.49890000000000e-05f << 3.03830000000000e-09f << endr
      << 781.f << 3.72032000000000e-05f << 1.41080000000000e-05f << 2.86878000000000e-09f << endr
      << 782.f << 3.48654000000000e-05f << 1.32270000000000e-05f << 2.69926000000000e-09f << endr
      << 783.f << 3.25276000000000e-05f << 1.23460000000000e-05f << 2.52974000000000e-09f << endr
      << 784.f << 3.01898000000000e-05f << 1.14650000000000e-05f << 2.36022000000000e-09f << endr
      << 785.f << 2.78520000000000e-05f << 1.05840000000000e-05f << 2.19070000000000e-09f << endr
      << 786.f << 2.62010000000000e-05f << 9.96032000000000e-06f << 2.06812000000000e-09f << endr
      << 787.f << 2.45500000000000e-05f << 9.33664000000000e-06f << 1.94554000000000e-09f << endr
      << 788.f << 2.28990000000000e-05f << 8.71296000000000e-06f << 1.82296000000000e-09f << endr
      << 789.f << 2.12480000000000e-05f << 8.08928000000000e-06f << 1.70038000000000e-09f << endr
      << 790.f << 1.95970000000000e-05f << 7.46560000000000e-06f << 1.57780000000000e-09f << endr
      << 791.f << 1.84316000000000e-05f << 7.02432000000000e-06f << 1.48920000000000e-09f << endr
      << 792.f << 1.72662000000000e-05f << 6.58304000000000e-06f << 1.40060000000000e-09f << endr
      << 793.f << 1.61008000000000e-05f << 6.14176000000000e-06f << 1.31200000000000e-09f << endr
      << 794.f << 1.49354000000000e-05f << 5.70048000000000e-06f << 1.22340000000000e-09f << endr
      << 795.f << 1.37700000000000e-05f << 5.25920000000000e-06f << 1.13480000000000e-09f << endr
      << 796.f << 1.29500000000000e-05f << 4.94792000000000e-06f << 1.07097000000000e-09f << endr
      << 797.f << 1.21300000000000e-05f << 4.63664000000000e-06f << 1.00714000000000e-09f << endr
      << 798.f << 1.13100000000000e-05f << 4.32536000000000e-06f << 9.43310000000000e-10f << endr
      << 799.f << 1.04900000000000e-05f << 4.01408000000000e-06f << 8.79480000000000e-10f << endr
      << 800.f << 9.67000000000000e-06f << 3.70280000000000e-06f << 8.15650000000000e-10f << endr
      << 801.f << 9.09436000000000e-06f << 3.48376000000000e-06f << 7.69772000000000e-10f << endr
      << 802.f << 8.51872000000000e-06f << 3.26472000000000e-06f << 7.23894000000000e-10f << endr
      << 803.f << 7.94308000000000e-06f << 3.04568000000000e-06f << 6.78016000000000e-10f << endr
      << 804.f << 7.36744000000000e-06f << 2.82664000000000e-06f << 6.32138000000000e-10f << endr
      << 805.f << 6.79180000000000e-06f << 2.60760000000000e-06f << 5.86260000000000e-10f << endr
      << 806.f << 6.38756000000000e-06f << 2.45338000000000e-06f << 5.53284000000000e-10f << endr
      << 807.f << 5.98332000000000e-06f << 2.29916000000000e-06f << 5.20308000000000e-10f << endr
      << 808.f << 5.57908000000000e-06f << 2.14494000000000e-06f << 4.87332000000000e-10f << endr
      << 809.f << 5.17484000000000e-06f << 1.99072000000000e-06f << 4.54356000000000e-10f << endr
      << 810.f << 4.77060000000000e-06f << 1.83650000000000e-06f << 4.21380000000000e-10f << endr
      << 811.f << 4.48748000000000e-06f << 1.72820000000000e-06f << 3.97742000000000e-10f << endr
      << 812.f << 4.20436000000000e-06f << 1.61990000000000e-06f << 3.74104000000000e-10f << endr
      << 813.f << 3.92124000000000e-06f << 1.51160000000000e-06f << 3.50466000000000e-10f << endr
      << 814.f << 3.63812000000000e-06f << 1.40330000000000e-06f << 3.26828000000000e-10f << endr
      << 815.f << 3.35500000000000e-06f << 1.29500000000000e-06f << 3.03190000000000e-10f << endr
      << 816.f << 3.15468000000000e-06f << 1.21818400000000e-06f << 2.86058000000000e-10f << endr
      << 817.f << 2.95436000000000e-06f << 1.14136800000000e-06f << 2.68926000000000e-10f << endr
      << 818.f << 2.75404000000000e-06f << 1.06455200000000e-06f << 2.51794000000000e-10f << endr
      << 819.f << 2.55372000000000e-06f << 9.87736000000000e-07f << 2.34662000000000e-10f << endr
      << 820.f << 2.35340000000000e-06f << 9.10920000000000e-07f << 2.17530000000000e-10f << endr
      << 821.f << 2.21026000000000e-06f << 8.55864000000000e-07f << 2.04976000000000e-10f << endr
      << 822.f << 2.06712000000000e-06f << 8.00808000000000e-07f << 1.92422000000000e-10f << endr
      << 823.f << 1.92398000000000e-06f << 7.45752000000000e-07f << 1.79868000000000e-10f << endr
      << 824.f << 1.78084000000000e-06f << 6.90696000000000e-07f << 1.67314000000000e-10f << endr
      << 825.f << 1.63770000000000e-06f << 6.35640000000000e-07f << 1.54760000000000e-10f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}


#define NUM_FNCS_8 (8)

void MKCC::colorMatchFcn(std::string formulary, fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
  lambda.reset();
  xFcn.reset();
  yFcn.reset();
  zFcn.reset();

  std::string names[] =
    {
      "cie_1931", "1931_full", "cie_1964", "1964_full", "judd", "judd_vos", "stiles_2", "stiles_10"
    };

  int i;
  for(i = 0; formulary[i] != '\0'; i++) {
    formulary[i] = tolower(formulary[i]);
  }

  size_t index;
  index = NUM_FNCS_8;
  for(i = 0; i < NUM_FNCS_8; ++i) {
    if(names[i].find(formulary) != std::string::npos) {
      index = i;
      break;
    }
  }

  switch(index)
  {
  case 0: // CIE_1931
      CMF_CIE_1931(lambda, xFcn, yFcn, zFcn);
      break;
  case 2: // CIE_1964
      CMF_CIE_1964(lambda, xFcn, yFcn, zFcn);
      break;
  case 4: // Judd
      CMF_JUDD(lambda, xFcn, yFcn, zFcn);
      break;
  case 5: // Judd_Vos
      // recommended (MIN: closer to the eye)
      // (Min) interpolating the cmf
      // xi = [380:825]';
      // cmfi = interp1(x,cmf,xi);
      CMF_JUDD_VOS(lambda, xFcn, yFcn, zFcn);
      break;
  case 6: // Stiles_2
      CMF_STILES_2(lambda, xFcn, yFcn, zFcn);
      break;
  case 7: // Stiles_10
      CMF_STILES_10(lambda, xFcn, yFcn, zFcn);
      break;
  case 1: // 1931_FULL
      CMF_CIE_1931_FULL(lambda, xFcn, yFcn, zFcn);
      break;
  case 3: // 1964_FULL
      CMF_CIE_1964_FULL(lambda, xFcn, yFcn, zFcn);
      break;
  default:
      {
        // only executes if function is unrecognized (shouldn't happen)
        cerr << "Error: colorMatchFcn:unrecognizedMatchFcn: Unrecognized color match function.\n";
        break;
      }
    }
return;
}


void MKCC::CMF_CIE_1931(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
    fmat::fixed<95,4> cmf;
    cmf << 360.f << 0.000129900000f << 0.000003917000f << 0.000606100000f << endr
      << 365.f << 0.000232100000f << 0.000006965000f << 0.001086000000f << endr
      << 370.f << 0.000414900000f << 0.000012390000f << 0.001946000000f << endr
      << 375.f << 0.000741600000f << 0.000022020000f << 0.003486000000f << endr
      << 380.f << 0.001368000000f << 0.000039000000f << 0.006450001000f << endr
      << 385.f << 0.002236000000f << 0.000064000000f << 0.010549990000f << endr
      << 390.f << 0.004243000000f << 0.000120000000f << 0.020050010000f << endr
      << 395.f << 0.007650000000f << 0.000217000000f << 0.036210000000f << endr
      << 400.f << 0.014310000000f << 0.000396000000f << 0.067850010000f << endr
      << 405.f << 0.023190000000f << 0.000640000000f << 0.110200000000f << endr
      << 410.f << 0.043510000000f << 0.001210000000f << 0.207400000000f << endr
      << 415.f << 0.077630000000f << 0.002180000000f << 0.371300000000f << endr
      << 420.f << 0.134380000000f << 0.004000000000f << 0.645600000000f << endr
      << 425.f << 0.214770000000f << 0.007300000000f << 1.039050100000f << endr
      << 430.f << 0.283900000000f << 0.011600000000f << 1.385600000000f << endr
      << 435.f << 0.328500000000f << 0.016840000000f << 1.622960000000f << endr
      << 440.f << 0.348280000000f << 0.023000000000f << 1.747060000000f << endr
      << 445.f << 0.348060000000f << 0.029800000000f << 1.782600000000f << endr
      << 450.f << 0.336200000000f << 0.038000000000f << 1.772110000000f << endr
      << 455.f << 0.318700000000f << 0.048000000000f << 1.744100000000f << endr
      << 460.f << 0.290800000000f << 0.060000000000f << 1.669200000000f << endr
      << 465.f << 0.251100000000f << 0.073900000000f << 1.528100000000f << endr
      << 470.f << 0.195360000000f << 0.090980000000f << 1.287640000000f << endr
      << 475.f << 0.142100000000f << 0.112600000000f << 1.041900000000f << endr
      << 480.f << 0.095640000000f << 0.139020000000f << 0.812950100000f << endr
      << 485.f << 0.057950010000f << 0.169300000000f << 0.616200000000f << endr
      << 490.f << 0.032010000000f << 0.208020000000f << 0.465180000000f << endr
      << 495.f << 0.014700000000f << 0.258600000000f << 0.353300000000f << endr
      << 500.f << 0.004900000000f << 0.323000000000f << 0.272000000000f << endr
      << 505.f << 0.002400000000f << 0.407300000000f << 0.212300000000f << endr
      << 510.f << 0.009300000000f << 0.503000000000f << 0.158200000000f << endr
      << 515.f << 0.029100000000f << 0.608200000000f << 0.111700000000f << endr
      << 520.f << 0.063270000000f << 0.710000000000f << 0.078249990000f << endr
      << 525.f << 0.109600000000f << 0.793200000000f << 0.057250010000f << endr
      << 530.f << 0.165500000000f << 0.862000000000f << 0.042160000000f << endr
      << 535.f << 0.225749900000f << 0.914850100000f << 0.029840000000f << endr
      << 540.f << 0.290400000000f << 0.954000000000f << 0.020300000000f << endr
      << 545.f << 0.359700000000f << 0.980300000000f << 0.013400000000f << endr
      << 550.f << 0.433449900000f << 0.994950100000f << 0.008749999000f << endr
      << 555.f << 0.512050100000f << 1.000000000000f << 0.005749999000f << endr
      << 560.f << 0.594500000000f << 0.995000000000f << 0.003900000000f << endr
      << 565.f << 0.678400000000f << 0.978600000000f << 0.002749999000f << endr
      << 570.f << 0.762100000000f << 0.952000000000f << 0.002100000000f << endr
      << 575.f << 0.842500000000f << 0.915400000000f << 0.001800000000f << endr
      << 580.f << 0.916300000000f << 0.870000000000f << 0.001650001000f << endr
      << 585.f << 0.978600000000f << 0.816300000000f << 0.001400000000f << endr
      << 590.f << 1.026300000000f << 0.757000000000f << 0.001100000000f << endr
      << 595.f << 1.056700000000f << 0.694900000000f << 0.001000000000f << endr
      << 600.f << 1.062200000000f << 0.631000000000f << 0.000800000000f << endr
              << 605.f << 1.045600000000f << 0.566800000000f << 0.000600000000f << endr
      << 610.f << 1.002600000000f << 0.503000000000f << 0.000340000000f << endr
      << 615.f << 0.938400000000f << 0.441200000000f << 0.000240000000f << endr
      << 620.f << 0.854449900000f << 0.381000000000f << 0.000190000000f << endr
      << 625.f << 0.751400000000f << 0.321000000000f << 0.000100000000f << endr
      << 630.f << 0.642400000000f << 0.265000000000f << 0.000049999990f << endr
      << 635.f << 0.541900000000f << 0.217000000000f << 0.000030000000f << endr
      << 640.f << 0.447900000000f << 0.175000000000f << 0.000020000000f << endr
      << 645.f << 0.360800000000f << 0.138200000000f << 0.000010000000f << endr
              << 650.f << 0.283500000000f << 0.107000000000f << 0.000000000000f << endr
      << 655.f << 0.218700000000f << 0.081600000000f << 0.000000000000f << endr
      << 660.f << 0.164900000000f << 0.061000000000f << 0.000000000000f << endr
      << 665.f << 0.121200000000f << 0.044580000000f << 0.000000000000f << endr
      << 670.f << 0.087400000000f << 0.032000000000f << 0.000000000000f << endr
              << 675.f << 0.063600000000f << 0.023200000000f << 0.000000000000f << endr
      << 680.f << 0.046770000000f << 0.017000000000f << 0.000000000000f << endr
      << 685.f << 0.032900000000f << 0.011920000000f << 0.000000000000f << endr
      << 690.f << 0.022700000000f << 0.008210000000f << 0.000000000000f << endr
      << 695.f << 0.015840000000f << 0.005723000000f << 0.000000000000f << endr
      << 700.f << 0.011359160000f << 0.004102000000f << 0.000000000000f << endr
      << 705.f << 0.008110916000f << 0.002929000000f << 0.000000000000f << endr
      << 710.f << 0.005790346000f << 0.002091000000f << 0.000000000000f << endr
      << 715.f << 0.004109457000f << 0.001484000000f << 0.000000000000f << endr
      << 720.f << 0.002899327000f << 0.001047000000f << 0.000000000000f << endr
      << 725.f << 0.002049190000f << 0.000740000000f << 0.000000000000f << endr
      << 730.f << 0.001439971000f << 0.000520000000f << 0.000000000000f << endr
      << 735.f << 0.000999949300f << 0.000361100000f << 0.000000000000f << endr
      << 740.f << 0.000690078600f << 0.000249200000f << 0.000000000000f << endr
      << 745.f << 0.000476021300f << 0.000171900000f << 0.000000000000f << endr
      << 750.f << 0.000332301100f << 0.000120000000f << 0.000000000000f << endr
      << 755.f << 0.000234826100f << 0.000084800000f << 0.000000000000f << endr
      << 760.f << 0.000166150500f << 0.000060000000f << 0.000000000000f << endr
      << 765.f << 0.000117413000f << 0.000042400000f << 0.000000000000f << endr
      << 770.f << 0.000083075270f << 0.000030000000f << 0.000000000000f << endr
      << 775.f << 0.000058706520f << 0.000021200000f << 0.000000000000f << endr
      << 780.f << 0.000041509940f << 0.000014990000f << 0.000000000000f << endr
      << 785.f << 0.000029353260f << 0.000010600000f << 0.000000000000f << endr
      << 790.f << 0.000020673830f << 0.000007465700f << 0.000000000000f << endr
      << 795.f << 0.000014559770f << 0.000005257800f << 0.000000000000f << endr
      << 800.f << 0.000010253980f << 0.000003702900f << 0.000000000000f << endr
      << 805.f << 0.000007221456f << 0.000002607800f << 0.000000000000f << endr
      << 810.f << 0.000005085868f << 0.000001836600f << 0.000000000000f << endr
      << 815.f << 0.000003581652f << 0.000001293400f << 0.000000000000f << endr
      << 820.f << 0.000002522525f << 0.000000910930f << 0.000000000000f << endr
      << 825.f << 0.000001776509f << 0.000000641530f << 0.000000000000f << endr
      << 830.f << 0.000001251141f << 0.000000451810f << 0.000000000000f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}

void MKCC::CMF_CIE_1964(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
    fmat::fixed<95,4> cmf;
    cmf << 360.f << 0.000000122200f << 0.000000013398f << 0.000000535027f << endr
      << 365.f << 0.000000919270f << 0.000000100650f << 0.000004028300f << endr
      << 370.f << 0.000005958600f << 0.000000651100f << 0.000026143700f << endr
      << 375.f << 0.000033266000f << 0.000003625000f << 0.000146220000f << endr
      << 380.f << 0.000159952000f << 0.000017364000f << 0.000704776000f << endr
      << 385.f << 0.000662440000f << 0.000071560000f << 0.002927800000f << endr
      << 390.f << 0.002361600000f << 0.000253400000f << 0.010482200000f << endr
      << 395.f << 0.007242300000f << 0.000768500000f << 0.032344000000f << endr
      << 400.f << 0.019109700000f << 0.002004400000f << 0.086010900000f << endr
      << 405.f << 0.043400000000f << 0.004509000000f << 0.197120000000f << endr
      << 410.f << 0.084736000000f << 0.008756000000f << 0.389366000000f << endr
      << 415.f << 0.140638000000f << 0.014456000000f << 0.656760000000f << endr
      << 420.f << 0.204492000000f << 0.021391000000f << 0.972542000000f << endr
      << 425.f << 0.264737000000f << 0.029497000000f << 1.282500000000f << endr
      << 430.f << 0.314679000000f << 0.038676000000f << 1.553480000000f << endr
      << 435.f << 0.357719000000f << 0.049602000000f << 1.798500000000f << endr
      << 440.f << 0.383734000000f << 0.062077000000f << 1.967280000000f << endr
      << 445.f << 0.386726000000f << 0.074704000000f << 2.027300000000f << endr
      << 450.f << 0.370702000000f << 0.089456000000f << 1.994800000000f << endr
      << 455.f << 0.342957000000f << 0.106256000000f << 1.900700000000f << endr
      << 460.f << 0.302273000000f << 0.128201000000f << 1.745370000000f << endr
      << 465.f << 0.254085000000f << 0.152761000000f << 1.554900000000f << endr
      << 470.f << 0.195618000000f << 0.185190000000f << 1.317560000000f << endr
      << 475.f << 0.132349000000f << 0.219940000000f << 1.030200000000f << endr
      << 480.f << 0.080507000000f << 0.253589000000f << 0.772125000000f << endr
      << 485.f << 0.041072000000f << 0.297665000000f << 0.570060000000f << endr
      << 490.f << 0.016172000000f << 0.339133000000f << 0.415254000000f << endr
      << 495.f << 0.005132000000f << 0.395379000000f << 0.302356000000f << endr
      << 500.f << 0.003816000000f << 0.460777000000f << 0.218502000000f << endr
      << 505.f << 0.015444000000f << 0.531360000000f << 0.159249000000f << endr
      << 510.f << 0.037465000000f << 0.606741000000f << 0.112044000000f << endr
      << 515.f << 0.071358000000f << 0.685660000000f << 0.082248000000f << endr
      << 520.f << 0.117749000000f << 0.761757000000f << 0.060709000000f << endr
      << 525.f << 0.172953000000f << 0.823330000000f << 0.043050000000f << endr
      << 530.f << 0.236491000000f << 0.875211000000f << 0.030451000000f << endr
      << 535.f << 0.304213000000f << 0.923810000000f << 0.020584000000f << endr
      << 540.f << 0.376772000000f << 0.961988000000f << 0.013676000000f << endr
      << 545.f << 0.451584000000f << 0.982200000000f << 0.007918000000f << endr
      << 550.f << 0.529826000000f << 0.991761000000f << 0.003988000000f << endr
      << 555.f << 0.616053000000f << 0.999110000000f << 0.001091000000f << endr
      << 560.f << 0.705224000000f << 0.997340000000f << 0.f << endr
      << 565.f << 0.793832000000f << 0.982380000000f << 0.f << endr
      << 570.f << 0.878655000000f << 0.955552000000f << 0.f << endr
      << 575.f << 0.951162000000f << 0.915175000000f << 0.f << endr
      << 580.f << 1.014160000000f << 0.868934000000f << 0.f << endr
      << 585.f << 1.074300000000f << 0.825623000000f << 0.f << endr
      << 590.f << 1.118520000000f << 0.777405000000f << 0.f << endr
      << 595.f << 1.134300000000f << 0.720353000000f << 0.f << endr
      << 600.f << 1.123990000000f << 0.658341000000f << 0.f << endr
      << 605.f << 1.089100000000f << 0.593878000000f << 0.f << endr
      << 610.f << 1.030480000000f << 0.527963000000f << 0.f << endr
      << 615.f << 0.950740000000f << 0.461834000000f << 0.f << endr
      << 620.f << 0.856297000000f << 0.398057000000f << 0.f << endr
      << 625.f << 0.754930000000f << 0.339554000000f << 0.f << endr
      << 630.f << 0.647467000000f << 0.283493000000f << 0.f << endr
      << 635.f << 0.535110000000f << 0.228254000000f << 0.f << endr
      << 640.f << 0.431567000000f << 0.179828000000f << 0.f << endr
      << 645.f << 0.343690000000f << 0.140211000000f << 0.f << endr
      << 650.f << 0.268329000000f << 0.107633000000f << 0.f << endr
      << 655.f << 0.204300000000f << 0.081187000000f << 0.f << endr
      << 660.f << 0.152568000000f << 0.060281000000f << 0.f << endr
      << 665.f << 0.112210000000f << 0.044096000000f << 0.f << endr
      << 670.f << 0.081260600000f << 0.031800400000f << 0.f << endr
      << 675.f << 0.057930000000f << 0.022601700000f << 0.f << endr
      << 680.f << 0.040850800000f << 0.015905100000f << 0.f << endr
      << 685.f << 0.028623000000f << 0.011130300000f << 0.f << endr
      << 690.f << 0.019941300000f << 0.007748800000f << 0.f << endr
      << 695.f << 0.013842000000f << 0.005375100000f << 0.f << endr
      << 700.f << 0.009576880000f << 0.003717740000f << 0.f << endr
      << 705.f << 0.006605200000f << 0.002564560000f << 0.f << endr
      << 710.f << 0.004552630000f << 0.001768470000f << 0.f << endr
      << 715.f << 0.003144700000f << 0.001222390000f << 0.f << endr
      << 720.f << 0.002174960000f << 0.000846190000f << 0.f << endr
      << 725.f << 0.001505700000f << 0.000586440000f << 0.f << endr
      << 730.f << 0.001044760000f << 0.000407410000f << 0.f << endr
      << 735.f << 0.000727450000f << 0.000284041000f << 0.f << endr
      << 740.f << 0.000508258000f << 0.000198730000f << 0.f << endr
      << 745.f << 0.000356380000f << 0.000139550000f << 0.f << endr
      << 750.f << 0.000250969000f << 0.000098428000f << 0.f << endr
      << 755.f << 0.000177730000f << 0.000069819000f << 0.f << endr
      << 760.f << 0.000126390000f << 0.000049737000f << 0.f << endr
      << 765.f << 0.000090151000f << 0.000035540500f << 0.f << endr
      << 770.f << 0.000064525800f << 0.000025486000f << 0.f << endr
      << 775.f << 0.000046339000f << 0.000018338400f << 0.f << endr
      << 780.f << 0.000033411700f << 0.000013249000f << 0.f << endr
      << 785.f << 0.000024209000f << 0.000009619600f << 0.f << endr
      << 790.f << 0.000017611500f << 0.000007012800f << 0.f << endr
      << 795.f << 0.000012855000f << 0.000005129800f << 0.f << endr
      << 800.f << 0.000009413630f << 0.000003764730f << 0.f << endr
      << 805.f << 0.000006913000f << 0.000002770810f << 0.f << endr
      << 810.f << 0.000005093470f << 0.000002046130f << 0.f << endr
      << 815.f << 0.000003767100f << 0.000001516770f << 0.f << endr
      << 820.f << 0.000002795310f << 0.000001128090f << 0.f << endr
      << 825.f << 0.000002082000f << 0.000000842160f << 0.f << endr
      << 830.f << 0.000001553140f << 0.000000629700f << 0.f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}






void MKCC::CMF_JUDD(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
    fmat::fixed<41,4> cmf;
    cmf << 370.f << 0.0008f << 0.0001f << 0.0046f << endr
      << 380.f << 0.0045f << 0.0004f << 0.0224f << endr
      << 390.f << 0.0201f << 0.0015f << 0.0925f << endr
          << 400.f << 0.0611f << 0.0045f << 0.2799f << endr
          << 410.f << 0.1267f << 0.0093f << 0.5835f << endr
          << 420.f << 0.2285f << 0.0175f << 1.0622f << endr
          << 430.f << 0.3081f << 0.0273f << 1.4526f << endr
          << 440.f << 0.3312f << 0.0379f << 1.6064f << endr
          << 450.f << 0.2888f << 0.0468f << 1.4717f << endr
          << 460.f << 0.2323f << 0.0600f << 1.2880f << endr
          << 470.f << 0.1745f << 0.0910f << 1.1133f << endr
          << 480.f << 0.0920f << 0.1390f << 0.7552f << endr
          << 490.f << 0.0318f << 0.2080f << 0.4461f << endr
          << 500.f << 0.0048f << 0.3230f << 0.2644f << endr
          << 510.f << 0.0093f << 0.5030f << 0.1541f << endr
          << 520.f << 0.0636f << 0.7100f << 0.0763f << endr
          << 530.f << 0.1668f << 0.8620f << 0.0412f << endr
          << 540.f << 0.2926f << 0.9540f << 0.0200f << endr
          << 550.f << 0.4364f << 0.9950f << 0.0088f << endr
          << 560.f << 0.5970f << 0.9950f << 0.0039f << endr
          << 570.f << 0.7642f << 0.9520f << 0.0020f << endr
          << 580.f << 0.9159f << 0.8700f << 0.0016f << endr
          << 590.f << 1.0225f << 0.7570f << 0.0011f << endr
          << 600.f << 1.0544f << 0.6310f << 0.0007f << endr
          << 610.f << 0.9922f << 0.5030f << 0.0003f << endr
          << 620.f << 0.8432f << 0.3810f << 0.0002f << endr
          << 630.f << 0.6327f << 0.2650f << 0.0001f << endr
          << 640.f << 0.4404f << 0.1750f << 0.0000f << endr
          << 650.f << 0.2787f << 0.1070f << 0.0000f << endr
          << 660.f << 0.1619f << 0.0610f << 0.0000f << endr
          << 670.f << 0.0858f << 0.0320f << 0.0000f << endr
          << 680.f << 0.0459f << 0.0170f << 0.0000f << endr
          << 690.f << 0.0222f << 0.0082f << 0.0000f << endr
          << 700.f << 0.0113f << 0.0041f << 0.0000f << endr
          << 710.f << 0.0057f << 0.0021f << 0.0000f << endr
          << 720.f << 0.0028f << 0.0011f << 0.0000f << endr
          << 730.f << 0.0015f << 0.0005f << 0.0000f << endr
          << 740.f << 0.0005f << 0.0002f << 0.0000f << endr
          << 750.f << 0.0003f << 0.0001f << 0.0000f << endr
          << 760.f << 0.0002f << 0.0001f << 0.0000f << endr
          << 770.f << 0.0001f << 0.0000f << 0.0000f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}




void MKCC::CMF_STILES_2(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
    fmat::fixed<69,4> cmf;
    cmf << 390.f << 1.83970e-003f << -4.53930e-004f << 1.21520e-002f << endr
      << 395.f << 4.61530e-003f << -1.04640e-003f << 3.11100e-002f << endr
      << 400.f << 9.62640e-003f << -2.16890e-003f << 6.23710e-002f << endr
      << 405.f << 1.89790e-002f << -4.43040e-003f << 1.31610e-001f << endr
      << 410.f << 3.08030e-002f << -7.20480e-003f << 2.27500e-001f << endr
      << 415.f << 4.24590e-002f << -1.25790e-002f << 3.58970e-001f << endr
          << 420.f << 5.16620e-002f << -1.66510e-002f << 5.23960e-001f << endr
          << 425.f << 5.28370e-002f << -2.12400e-002f << 6.85860e-001f << endr
          << 430.f << 4.42870e-002f << -1.99360e-002f << 7.96040e-001f << endr
          << 435.f << 3.22200e-002f << -1.60970e-002f << 8.94590e-001f << endr
          << 440.f << 1.47630e-002f << -7.34570e-003f << 9.63950e-001f << endr
          << 445.f << -2.33920e-003f << 1.36900e-003f << 9.98140e-001f << endr
          << 450.f << -2.91300e-002f << 1.96100e-002f << 9.18750e-001f << endr
          << 455.f << -6.06770e-002f << 4.34640e-002f << 8.24870e-001f << endr
          << 460.f << -9.62240e-002f << 7.09540e-002f << 7.85540e-001f << endr
          << 465.f << -1.37590e-001f << 1.10220e-001f << 6.67230e-001f << endr
          << 470.f << -1.74860e-001f << 1.50880e-001f << 6.10980e-001f << endr
          << 475.f << -2.12600e-001f << 1.97940e-001f << 4.88290e-001f << endr
          << 480.f << -2.37800e-001f << 2.40420e-001f << 3.61950e-001f << endr
          << 485.f << -2.56740e-001f << 2.79930e-001f << 2.66340e-001f << endr
          << 490.f << -2.77270e-001f << 3.33530e-001f << 1.95930e-001f << endr
          << 495.f << -2.91250e-001f << 4.05210e-001f << 1.47300e-001f << endr
          << 500.f << -2.95000e-001f << 4.90600e-001f << 1.07490e-001f << endr
          << 505.f << -2.97060e-001f << 5.96730e-001f << 7.67140e-002f << endr
          << 510.f << -2.67590e-001f << 7.01840e-001f << 5.02480e-002f << endr
          << 515.f << -2.17250e-001f << 8.08520e-001f << 2.87810e-002f << endr
          << 520.f << -1.47680e-001f << 9.10760e-001f << 1.33090e-002f << endr
          << 525.f << -3.51840e-002f << 9.84820e-001f << 2.11700e-003f << endr
          << 530.f << 1.06140e-001f << 1.03390e+000f << -4.15740e-003f << endr
          << 535.f << 2.59810e-001f << 1.05380e+000f << -8.30320e-003f << endr
          << 540.f << 4.19760e-001f << 1.05120e+000f << -1.21910e-002f << endr
          << 545.f << 5.92590e-001f << 1.04980e+000f << -1.40390e-002f << endr
          << 550.f << 7.90040e-001f << 1.03680e+000f << -1.46810e-002f << endr
          << 555.f << 1.00780e+000f << 9.98260e-001f << -1.49470e-002f << endr
          << 560.f << 1.22830e+000f << 9.37830e-001f << -1.46130e-002f << endr
          << 565.f << 1.47270e+000f << 8.80390e-001f << -1.37820e-002f << endr
          << 570.f << 1.74760e+000f << 8.28350e-001f << -1.26500e-002f << endr
          << 575.f << 2.02140e+000f << 7.46860e-001f << -1.13560e-002f << endr
          << 580.f << 2.27240e+000f << 6.49300e-001f << -9.93170e-003f << endr
          << 585.f << 2.48960e+000f << 5.63170e-001f << -8.41480e-003f << endr
          << 590.f << 2.67250e+000f << 4.76750e-001f << -7.02100e-003f << endr
          << 595.f << 2.80930e+000f << 3.84840e-001f << -5.74370e-003f << endr
          << 600.f << 2.87170e+000f << 3.00690e-001f << -4.27430e-003f << endr
          << 605.f << 2.85250e+000f << 2.28530e-001f << -2.91320e-003f << endr
          << 610.f << 2.76010e+000f << 1.65750e-001f << -2.26930e-003f << endr
          << 615.f << 2.59890e+000f << 1.13730e-001f << -1.99660e-003f << endr
          << 620.f << 2.37430e+000f << 7.46820e-002f << -1.50690e-003f << endr
          << 625.f << 2.10540e+000f << 4.65040e-002f << -9.38220e-004f << endr
          << 630.f << 1.81450e+000f << 2.63330e-002f << -5.53160e-004f << endr
          << 635.f << 1.52470e+000f << 1.27240e-002f << -3.16680e-004f << endr
          << 640.f << 1.25430e+000f << 4.50330e-003f << -1.43190e-004f << endr
          << 645.f << 1.00760e+000f << 9.66110e-005f << -4.08310e-006f << endr
          << 650.f << 7.86420e-001f << -1.96450e-003f << 1.10810e-004f << endr
          << 655.f << 5.96590e-001f << -2.63270e-003f << 1.91750e-004f << endr
          << 660.f << 4.43200e-001f << -2.62620e-003f << 2.26560e-004f << endr
          << 665.f << 3.24100e-001f << -2.30270e-003f << 2.15200e-004f << endr
          << 670.f << 2.34550e-001f << -1.87000e-003f << 1.63610e-004f << endr
          << 675.f << 1.68840e-001f << -1.44240e-003f << 9.71640e-005f << endr
          << 680.f << 1.20860e-001f << -1.07550e-003f << 5.10330e-005f << endr
          << 685.f << 8.58110e-002f << -7.90040e-004f << 3.52710e-005f << endr
          << 690.f << 6.02600e-002f << -5.67650e-004f << 3.12110e-005f << endr
          << 695.f << 4.14800e-002f << -3.92740e-004f << 2.45080e-005f << endr
          << 700.f << 2.81140e-002f << -2.62310e-004f << 1.65210e-005f << endr
          << 705.f << 1.91170e-002f << -1.75120e-004f << 1.11240e-005f << endr
          << 710.f << 1.33050e-002f << -1.21400e-004f << 8.69650e-006f << endr
          << 715.f << 9.40920e-003f << -8.57600e-005f << 7.43510e-006f << endr
          << 720.f << 6.51770e-003f << -5.76770e-005f << 6.10570e-006f << endr
          << 725.f << 4.53770e-003f << -3.90030e-005f << 5.02770e-006f << endr
          << 730.f << 3.17420e-003f << -2.65110e-005f << 4.12510e-006f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}


void MKCC::CMF_STILES_10(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
    fmat::fixed<89,4> cmf;
    cmf << 390.f << 1.5000e-03f << -4.0000e-04f << 6.2000e-03f << endr
      << 395.f << 3.8000e-03f << -1.0000e-03f << 1.6100e-02f << endr
      << 400.f << 8.9000e-03f << -2.5000e-03f << 4.0000e-02f << endr
      << 405.f << 1.8800e-02f << -5.9000e-03f << 9.0600e-02f << endr
      << 410.f << 3.5000e-02f << -1.1900e-02f << 1.8020e-01f << endr
      << 415.f << 5.3100e-02f << -2.0100e-02f << 3.0880e-01f << endr
      << 420.f << 7.0200e-02f << -2.8900e-02f << 4.6700e-01f << endr
      << 425.f << 7.6300e-02f << -3.3800e-02f << 6.1520e-01f << endr
      << 430.f << 7.4500e-02f << -3.4900e-02f << 7.6380e-01f << endr
      << 435.f << 5.6100e-02f << -2.7600e-02f << 8.7780e-01f << endr
      << 440.f << 3.2300e-02f << -1.6900e-02f << 9.7550e-01f << endr
      << 445.f << -4.4000e-03f << 2.4000e-03f << 1.0019e+00f << endr
      << 450.f << -4.7800e-02f << 2.8300e-02f << 9.9960e-01f << endr
      << 455.f << -9.7000e-02f << 6.3600e-02f << 9.1390e-01f << endr
      << 460.f << -1.5860e-01f << 1.0820e-01f << 8.2970e-01f << endr
      << 465.f << -2.2350e-01f << 1.6170e-01f << 7.4170e-01f << endr
      << 470.f << -2.8480e-01f << 2.2010e-01f << 6.1340e-01f << endr
      << 475.f << -3.3460e-01f << 2.7960e-01f << 4.7200e-01f << endr
      << 480.f << -3.7760e-01f << 3.4280e-01f << 3.4950e-01f << endr
      << 485.f << -4.1360e-01f << 4.0860e-01f << 2.5640e-01f << endr
      << 490.f << -4.3170e-01f << 4.7160e-01f << 1.8190e-01f << endr
      << 495.f << -4.4520e-01f << 5.4910e-01f << 1.3070e-01f << endr
      << 500.f << -4.3500e-01f << 6.2600e-01f << 9.1000e-02f << endr
      << 505.f << -4.1400e-01f << 7.0970e-01f << 5.8000e-02f << endr
      << 510.f << -3.6730e-01f << 7.9350e-01f << 3.5700e-02f << endr
      << 515.f << -2.8450e-01f << 8.7150e-01f << 2.0000e-02f << endr
      << 520.f << -1.8550e-01f << 9.4770e-01f << 9.5000e-03f << endr
      << 525.f << -4.3500e-02f << 9.9450e-01f << 7.0000e-04f << endr
      << 530.f << 1.2700e-01f << 1.0203e+00f << -4.3000e-03f << endr
      << 535.f << 3.1290e-01f << 1.0375e+00f << -6.4000e-03f << endr
      << 540.f << 5.3620e-01f << 1.0517e+00f << -8.2000e-03f << endr
      << 545.f << 7.7220e-01f << 1.0390e+00f << -9.4000e-03f << endr
      << 550.f << 1.0059e+00f << 1.0029e+00f << -9.7000e-03f << endr
      << 555.f << 1.2710e+00f << 9.6980e-01f << -9.7000e-03f << endr
      << 560.f << 1.5574e+00f << 9.1620e-01f << -9.3000e-03f << endr
      << 565.f << 1.8465e+00f << 8.5710e-01f << -8.7000e-03f << endr
      << 570.f << 2.1511e+00f << 7.8230e-01f << -8.0000e-03f << endr
      << 575.f << 2.4250e+00f << 6.9530e-01f << -7.3000e-03f << endr
      << 580.f << 2.6574e+00f << 5.9660e-01f << -6.3000e-03f << endr
      << 585.f << 2.9151e+00f << 5.0630e-01f << -5.3700e-03f << endr
      << 590.f << 3.0779e+00f << 4.2030e-01f << -4.4500e-03f << endr
      << 595.f << 3.1613e+00f << 3.3600e-01f << -3.5700e-03f << endr
      << 600.f << 3.1673e+00f << 2.5910e-01f << -2.7700e-03f << endr
      << 605.f << 3.1048e+00f << 1.9170e-01f << -2.0800e-03f << endr
      << 610.f << 2.9462e+00f << 1.3670e-01f << -1.5000e-03f << endr
      << 615.f << 2.7194e+00f << 9.3800e-02f << -1.0300e-03f << endr
      << 620.f << 2.4526e+00f << 6.1100e-02f << -6.8000e-04f << endr
      << 625.f << 2.1700e+00f << 3.7100e-02f << -4.4200e-04f << endr
      << 630.f << 1.8358e+00f << 2.1500e-02f << -2.7200e-04f << endr
      << 635.f << 1.5179e+00f << 1.1200e-02f << -1.4100e-04f << endr
      << 640.f << 1.2428e+00f << 4.4000e-03f << -5.4900e-05f << endr
      << 645.f << 1.0070e+00f << 7.8000e-05f << -2.2000e-06f << endr
      << 650.f << 7.8270e-01f << -1.3680e-03f << 2.3700e-05f << endr
      << 655.f << 5.9340e-01f << -1.9880e-03f << 2.8600e-05f << endr
      << 660.f << 4.4420e-01f << -2.1680e-03f << 2.6100e-05f << endr
      << 665.f << 3.2830e-01f << -2.0060e-03f << 2.2500e-05f << endr
      << 670.f << 2.3940e-01f << -1.6420e-03f << 1.8200e-05f << endr
      << 675.f << 1.7220e-01f << -1.2720e-03f << 1.3900e-05f << endr
      << 680.f << 1.2210e-01f << -9.4700e-04f << 1.0300e-05f << endr
      << 685.f << 8.5300e-02f << -6.8300e-04f << 7.3800e-06f << endr
      << 690.f << 5.8600e-02f << -4.7800e-04f << 5.2200e-06f << endr
      << 695.f << 4.0800e-02f << -3.3700e-04f << 3.6700e-06f << endr
      << 700.f << 2.8400e-02f << -2.3500e-04f << 2.5600e-06f << endr
      << 705.f << 1.9700e-02f << -1.6300e-04f << 1.7600e-06f << endr
      << 710.f << 1.3500e-02f << -1.1100e-04f << 1.2000e-06f << endr
      << 715.f << 9.2400e-03f << -7.4800e-05f << 8.1700e-07f << endr
      << 720.f << 6.3800e-03f << -5.0800e-05f << 5.5500e-07f << endr
      << 725.f << 4.4100e-03f << -3.4400e-05f << 3.7500e-07f << endr
      << 730.f << 3.0700e-03f << -2.3400e-05f << 2.5400e-07f << endr
      << 735.f << 2.1400e-03f << -1.5900e-05f << 1.7100e-07f << endr
      << 740.f << 1.4900e-03f << -1.0700e-05f << 1.1600e-07f << endr
      << 745.f << 1.0500e-03f << -7.2300e-06f << 7.8500e-08f << endr
      << 750.f << 7.3900e-04f << -4.8700e-06f << 5.3100e-08f << endr
      << 755.f << 5.2300e-04f << -3.2900e-06f << 3.6000e-08f << endr
      << 760.f << 3.7200e-04f << -2.2200e-06f << 2.4400e-08f << endr
      << 765.f << 2.6500e-04f << -1.5000e-06f << 1.6500e-08f << endr
      << 770.f << 1.9000e-04f << -1.0200e-06f << 1.1200e-08f << endr
      << 775.f << 1.3600e-04f << -6.8800e-07f << 7.5300e-09f << endr
      << 780.f << 9.8400e-05f << -4.6500e-07f << 5.0700e-09f << endr
      << 785.f << 7.1300e-05f << -3.1200e-07f << 3.4000e-09f << endr
      << 790.f << 5.1800e-05f << -2.0800e-07f << 2.2700e-09f << endr
      << 795.f << 3.7700e-05f << -1.3700e-07f << 1.5000e-09f << endr
      << 800.f << 2.7600e-05f << -8.8000e-08f << 9.8600e-10f << endr
      << 805.f << 2.0300e-05f << -5.5300e-08f << 6.3900e-10f << endr
      << 810.f << 1.4900e-05f << -3.3600e-08f << 4.0700e-10f << endr
      << 815.f << 1.1000e-05f << -1.9600e-08f << 2.5300e-10f << endr
      << 820.f << 8.1800e-06f << -1.0900e-08f << 1.5200e-10f << endr
      << 825.f << 6.0900e-06f << -5.7000e-09f << 8.6400e-11f << endr
      << 830.f << 4.5500e-06f << -2.7700e-09f << 4.4200e-11f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}
//============================================================================================

void MKCC::CMF_CIE_1931_FULL(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
fmat::fixed<471,4> cmf;
cmf << 360.f << 0.000129900000f << 0.000003917000f << 0.000606100000f << endr
  << 361.f << 0.000145847000f << 0.000004393581f << 0.000680879200f << endr
  << 362.f << 0.000163802100f << 0.000004929604f << 0.000765145600f << endr
    << 363.f << 0.000184003700f << 0.000005532136f << 0.000860012400f << endr
    << 364.f << 0.000206690200f << 0.000006208245f << 0.000966592800f << endr
    << 365.f << 0.000232100000f << 0.000006965000f << 0.001086000000f << endr
    << 366.f << 0.000260728000f << 0.000007813219f << 0.001220586000f << endr
    << 367.f << 0.000293075000f << 0.000008767336f << 0.001372729000f << endr
    << 368.f << 0.000329388000f << 0.000009839844f << 0.001543579000f << endr
    << 369.f << 0.000369914000f << 0.000011043230f << 0.001734286000f << endr
    << 370.f << 0.000414900000f << 0.000012390000f << 0.001946000000f << endr
    << 371.f << 0.000464158700f << 0.000013886410f << 0.002177777000f << endr
    << 372.f << 0.000518986000f << 0.000015557280f << 0.002435809000f << endr
    << 373.f << 0.000581854000f << 0.000017442960f << 0.002731953000f << endr
    << 374.f << 0.000655234700f << 0.000019583750f << 0.003078064000f << endr
    << 375.f << 0.000741600000f << 0.000022020000f << 0.003486000000f << endr
    << 376.f << 0.000845029600f << 0.000024839650f << 0.003975227000f << endr
    << 377.f << 0.000964526800f << 0.000028041260f << 0.004540880000f << endr
    << 378.f << 0.001094949000f << 0.000031531040f << 0.005158320000f << endr
    << 379.f << 0.001231154000f << 0.000035215210f << 0.005802907000f << endr
    << 380.f << 0.001368000000f << 0.000039000000f << 0.006450001000f << endr
    << 381.f << 0.001502050000f << 0.000042826400f << 0.007083216000f << endr
    << 382.f << 0.001642328000f << 0.000046914600f << 0.007745488000f << endr
    << 383.f << 0.001802382000f << 0.000051589600f << 0.008501152000f << endr
    << 384.f << 0.001995757000f << 0.000057176400f << 0.009414544000f << endr
    << 385.f << 0.002236000000f << 0.000064000000f << 0.010549990000f << endr
    << 386.f << 0.002535385000f << 0.000072344210f << 0.011965800000f << endr
    << 387.f << 0.002892603000f << 0.000082212240f << 0.013655870000f << endr
    << 388.f << 0.003300829000f << 0.000093508160f << 0.015588050000f << endr
    << 389.f << 0.003753236000f << 0.000106136100f << 0.017730150000f << endr
    << 390.f << 0.004243000000f << 0.000120000000f << 0.020050010000f << endr
    << 391.f << 0.004762389000f << 0.000134984000f << 0.022511360000f << endr
    << 392.f << 0.005330048000f << 0.000151492000f << 0.025202880000f << endr
    << 393.f << 0.005978712000f << 0.000170208000f << 0.028279720000f << endr
    << 394.f << 0.006741117000f << 0.000191816000f << 0.031897040000f << endr
    << 395.f << 0.007650000000f << 0.000217000000f << 0.036210000000f << endr
    << 396.f << 0.008751373000f << 0.000246906700f << 0.041437710000f << endr
    << 397.f << 0.010028880000f << 0.000281240000f << 0.047503720000f << endr
    << 398.f << 0.011421700000f << 0.000318520000f << 0.054119880000f << endr
    << 399.f << 0.012869010000f << 0.000357266700f << 0.060998030000f << endr
    << 400.f << 0.014310000000f << 0.000396000000f << 0.067850010000f << endr
    << 401.f << 0.015704430000f << 0.000433714700f << 0.074486320000f << endr
    << 402.f << 0.017147440000f << 0.000473024000f << 0.081361560000f << endr
    << 403.f << 0.018781220000f << 0.000517876000f << 0.089153640000f << endr
    << 404.f << 0.020748010000f << 0.000572218700f << 0.098540480000f << endr
    << 405.f << 0.023190000000f << 0.000640000000f << 0.110200000000f << endr
    << 406.f << 0.026207360000f << 0.000724560000f << 0.124613300000f << endr
    << 407.f << 0.029782480000f << 0.000825500000f << 0.141701700000f << endr
    << 408.f << 0.033880920000f << 0.000941160000f << 0.161303500000f << endr
    << 409.f << 0.038468240000f << 0.001069880000f << 0.183256800000f << endr
    << 410.f << 0.043510000000f << 0.001210000000f << 0.207400000000f << endr
    << 411.f << 0.048995600000f << 0.001362091000f << 0.233692100000f << endr
    << 412.f << 0.055022600000f << 0.001530752000f << 0.262611400000f << endr
    << 413.f << 0.061718800000f << 0.001720368000f << 0.294774600000f << endr
    << 414.f << 0.069212000000f << 0.001935323000f << 0.330798500000f << endr
    << 415.f << 0.077630000000f << 0.002180000000f << 0.371300000000f << endr
    << 416.f << 0.086958110000f << 0.002454800000f << 0.416209100000f << endr
    << 417.f << 0.097176720000f << 0.002764000000f << 0.465464200000f << endr
    << 418.f << 0.108406300000f << 0.003117800000f << 0.519694800000f << endr
    << 419.f << 0.120767200000f << 0.003526400000f << 0.579530300000f << endr
    << 420.f << 0.134380000000f << 0.004000000000f << 0.645600000000f << endr
    << 421.f << 0.149358200000f << 0.004546240000f << 0.718483800000f << endr
    << 422.f << 0.165395700000f << 0.005159320000f << 0.796713300000f << endr
    << 423.f << 0.181983100000f << 0.005829280000f << 0.877845900000f << endr
    << 424.f << 0.198611000000f << 0.006546160000f << 0.959439000000f << endr
    << 425.f << 0.214770000000f << 0.007300000000f << 1.039050100000f << endr
    << 426.f << 0.230186800000f << 0.008086507000f << 1.115367300000f << endr
    << 427.f << 0.244879700000f << 0.008908720000f << 1.188497100000f << endr
    << 428.f << 0.258777300000f << 0.009767680000f << 1.258123300000f << endr
    << 429.f << 0.271807900000f << 0.010664430000f << 1.323929600000f << endr
    << 430.f << 0.283900000000f << 0.011600000000f << 1.385600000000f << endr
    << 431.f << 0.294943800000f << 0.012573170000f << 1.442635200000f << endr
    << 432.f << 0.304896500000f << 0.013582720000f << 1.494803500000f << endr
    << 433.f << 0.313787300000f << 0.014629680000f << 1.542190300000f << endr
    << 434.f << 0.321645400000f << 0.015715090000f << 1.584880700000f << endr
    << 435.f << 0.328500000000f << 0.016840000000f << 1.622960000000f << endr
    << 436.f << 0.334351300000f << 0.018007360000f << 1.656404800000f << endr
    << 437.f << 0.339210100000f << 0.019214480000f << 1.685295900000f << endr
    << 438.f << 0.343121300000f << 0.020453920000f << 1.709874500000f << endr
    << 439.f << 0.346129600000f << 0.021718240000f << 1.730382100000f << endr
    << 440.f << 0.348280000000f << 0.023000000000f << 1.747060000000f << endr
    << 441.f << 0.349599900000f << 0.024294610000f << 1.760044600000f << endr
    << 442.f << 0.350147400000f << 0.025610240000f << 1.769623300000f << endr
    << 443.f << 0.350013000000f << 0.026958570000f << 1.776263700000f << endr
    << 444.f << 0.349287000000f << 0.028351250000f << 1.780433400000f << endr
    << 445.f << 0.348060000000f << 0.029800000000f << 1.782600000000f << endr
    << 446.f << 0.346373300000f << 0.031310830000f << 1.782968200000f << endr
    << 447.f << 0.344262400000f << 0.032883680000f << 1.781699800000f << endr
    << 448.f << 0.341808800000f << 0.034521120000f << 1.779198200000f << endr
    << 449.f << 0.339094100000f << 0.036225710000f << 1.775867100000f << endr
    << 450.f << 0.336200000000f << 0.038000000000f << 1.772110000000f << endr
    << 451.f << 0.333197700000f << 0.039846670000f << 1.768258900000f << endr
    << 452.f << 0.330041100000f << 0.041768000000f << 1.764039000000f << endr
    << 453.f << 0.326635700000f << 0.043766000000f << 1.758943800000f << endr
    << 454.f << 0.322886800000f << 0.045842670000f << 1.752466300000f << endr
    << 455.f << 0.318700000000f << 0.048000000000f << 1.744100000000f << endr
    << 456.f << 0.314025100000f << 0.050243680000f << 1.733559500000f << endr
    << 457.f << 0.308884000000f << 0.052573040000f << 1.720858100000f << endr
    << 458.f << 0.303290400000f << 0.054980560000f << 1.705936900000f << endr
    << 459.f << 0.297257900000f << 0.057458720000f << 1.688737200000f << endr
    << 460.f << 0.290800000000f << 0.060000000000f << 1.669200000000f << endr
    << 461.f << 0.283970100000f << 0.062601970000f << 1.647528700000f << endr
    << 462.f << 0.276721400000f << 0.065277520000f << 1.623412700000f << endr
    << 463.f << 0.268917800000f << 0.068042080000f << 1.596022300000f << endr
    << 464.f << 0.260422700000f << 0.070911090000f << 1.564528000000f << endr
    << 465.f << 0.251100000000f << 0.073900000000f << 1.528100000000f << endr
    << 466.f << 0.240847500000f << 0.077016000000f << 1.486111400000f << endr
    << 467.f << 0.229851200000f << 0.080266400000f << 1.439521500000f << endr
    << 468.f << 0.218407200000f << 0.083666800000f << 1.389879900000f << endr
    << 469.f << 0.206811500000f << 0.087232800000f << 1.338736200000f << endr
    << 470.f << 0.195360000000f << 0.090980000000f << 1.287640000000f << endr
    << 471.f << 0.184213600000f << 0.094917550000f << 1.237422300000f << endr
    << 472.f << 0.173327300000f << 0.099045840000f << 1.187824300000f << endr
    << 473.f << 0.162688100000f << 0.103367400000f << 1.138761100000f << endr
    << 474.f << 0.152283300000f << 0.107884600000f << 1.090148000000f << endr
    << 475.f << 0.142100000000f << 0.112600000000f << 1.041900000000f << endr
    << 476.f << 0.132178600000f << 0.117532000000f << 0.994197600000f << endr
    << 477.f << 0.122569600000f << 0.122674400000f << 0.947347300000f << endr
    << 478.f << 0.113275200000f << 0.127992800000f << 0.901453100000f << endr
    << 479.f << 0.104297900000f << 0.133452800000f << 0.856619300000f << endr
    << 480.f << 0.095640000000f << 0.139020000000f << 0.812950100000f << endr
    << 481.f << 0.087299550000f << 0.144676400000f << 0.770517300000f << endr
    << 482.f << 0.079308040000f << 0.150469300000f << 0.729444800000f << endr
    << 483.f << 0.071717760000f << 0.156461900000f << 0.689913600000f << endr
    << 484.f << 0.064580990000f << 0.162717700000f << 0.652104900000f << endr
    << 485.f << 0.057950010000f << 0.169300000000f << 0.616200000000f << endr
    << 486.f << 0.051862110000f << 0.176243100000f << 0.582328600000f << endr
    << 487.f << 0.046281520000f << 0.183558100000f << 0.550416200000f << endr
    << 488.f << 0.041150880000f << 0.191273500000f << 0.520337600000f << endr
    << 489.f << 0.036412830000f << 0.199418000000f << 0.491967300000f << endr
    << 490.f << 0.032010000000f << 0.208020000000f << 0.465180000000f << endr
    << 491.f << 0.027917200000f << 0.217119900000f << 0.439924600000f << endr
    << 492.f << 0.024144400000f << 0.226734500000f << 0.416183600000f << endr
    << 493.f << 0.020687000000f << 0.236857100000f << 0.393882200000f << endr
    << 494.f << 0.017540400000f << 0.247481200000f << 0.372945900000f << endr
    << 495.f << 0.014700000000f << 0.258600000000f << 0.353300000000f << endr
    << 496.f << 0.012161790000f << 0.270184900000f << 0.334857800000f << endr
    << 497.f << 0.009919960000f << 0.282293900000f << 0.317552100000f << endr
    << 498.f << 0.007967240000f << 0.295050500000f << 0.301337500000f << endr
    << 499.f << 0.006296346000f << 0.308578000000f << 0.286168600000f << endr
    << 500.f << 0.004900000000f << 0.323000000000f << 0.272000000000f << endr
    << 501.f << 0.003777173000f << 0.338402100000f << 0.258817100000f << endr
    << 502.f << 0.002945320000f << 0.354685800000f << 0.246483800000f << endr
    << 503.f << 0.002424880000f << 0.371698600000f << 0.234771800000f << endr
    << 504.f << 0.002236293000f << 0.389287500000f << 0.223453300000f << endr
    << 505.f << 0.002400000000f << 0.407300000000f << 0.212300000000f << endr
    << 506.f << 0.002925520000f << 0.425629900000f << 0.201169200000f << endr
    << 507.f << 0.003836560000f << 0.444309600000f << 0.190119600000f << endr
    << 508.f << 0.005174840000f << 0.463394400000f << 0.179225400000f << endr
    << 509.f << 0.006982080000f << 0.482939500000f << 0.168560800000f << endr
    << 510.f << 0.009300000000f << 0.503000000000f << 0.158200000000f << endr
    << 511.f << 0.012149490000f << 0.523569300000f << 0.148138300000f << endr
    << 512.f << 0.015535880000f << 0.544512000000f << 0.138375800000f << endr
    << 513.f << 0.019477520000f << 0.565690000000f << 0.128994200000f << endr
    << 514.f << 0.023992770000f << 0.586965300000f << 0.120075100000f << endr
    << 515.f << 0.029100000000f << 0.608200000000f << 0.111700000000f << endr
    << 516.f << 0.034814850000f << 0.629345600000f << 0.103904800000f << endr
    << 517.f << 0.041120160000f << 0.650306800000f << 0.096667480000f << endr
    << 518.f << 0.047985040000f << 0.670875200000f << 0.089982720000f << endr
    << 519.f << 0.055378610000f << 0.690842400000f << 0.083845310000f << endr
    << 520.f << 0.063270000000f << 0.710000000000f << 0.078249990000f << endr
    << 521.f << 0.071635010000f << 0.728185200000f << 0.073208990000f << endr
    << 522.f << 0.080462240000f << 0.745463600000f << 0.068678160000f << endr
    << 523.f << 0.089739960000f << 0.761969400000f << 0.064567840000f << endr
    << 524.f << 0.099456450000f << 0.777836800000f << 0.060788350000f << endr
    << 525.f << 0.109600000000f << 0.793200000000f << 0.057250010000f << endr
    << 526.f << 0.120167400000f << 0.808110400000f << 0.053904350000f << endr
    << 527.f << 0.131114500000f << 0.822496200000f << 0.050746640000f << endr
    << 528.f << 0.142367900000f << 0.836306800000f << 0.047752760000f << endr
    << 529.f << 0.153854200000f << 0.849491600000f << 0.044898590000f << endr
    << 530.f << 0.165500000000f << 0.862000000000f << 0.042160000000f << endr
    << 531.f << 0.177257100000f << 0.873810800000f << 0.039507280000f << endr
    << 532.f << 0.189140000000f << 0.884962400000f << 0.036935640000f << endr
    << 533.f << 0.201169400000f << 0.895493600000f << 0.034458360000f << endr
    << 534.f << 0.213365800000f << 0.905443200000f << 0.032088720000f << endr
    << 535.f << 0.225749900000f << 0.914850100000f << 0.029840000000f << endr
    << 536.f << 0.238320900000f << 0.923734800000f << 0.027711810000f << endr
    << 537.f << 0.251066800000f << 0.932092400000f << 0.025694440000f << endr
    << 538.f << 0.263992200000f << 0.939922600000f << 0.023787160000f << endr
    << 539.f << 0.277101700000f << 0.947225200000f << 0.021989250000f << endr
    << 540.f << 0.290400000000f << 0.954000000000f << 0.020300000000f << endr
    << 541.f << 0.303891200000f << 0.960256100000f << 0.018718050000f << endr
    << 542.f << 0.317572600000f << 0.966007400000f << 0.017240360000f << endr
    << 543.f << 0.331438400000f << 0.971260600000f << 0.015863640000f << endr
    << 544.f << 0.345482800000f << 0.976022500000f << 0.014584610000f << endr
    << 545.f << 0.359700000000f << 0.980300000000f << 0.013400000000f << endr
    << 546.f << 0.374083900000f << 0.984092400000f << 0.012307230000f << endr
    << 547.f << 0.388639600000f << 0.987418200000f << 0.011301880000f << endr
    << 548.f << 0.403378400000f << 0.990312800000f << 0.010377920000f << endr
    << 549.f << 0.418311500000f << 0.992811600000f << 0.009529306000f << endr
    << 550.f << 0.433449900000f << 0.994950100000f << 0.008749999000f << endr
    << 551.f << 0.448795300000f << 0.996710800000f << 0.008035200000f << endr
    << 552.f << 0.464336000000f << 0.998098300000f << 0.007381600000f << endr
    << 553.f << 0.480064000000f << 0.999112000000f << 0.006785400000f << endr
    << 554.f << 0.495971300000f << 0.999748200000f << 0.006242800000f << endr
    << 555.f << 0.512050100000f << 1.000000000000f << 0.005749999000f << endr
    << 556.f << 0.528295900000f << 0.999856700000f << 0.005303600000f << endr
    << 557.f << 0.544691600000f << 0.999304600000f << 0.004899800000f << endr
    << 558.f << 0.561209400000f << 0.998325500000f << 0.004534200000f << endr
    << 559.f << 0.577821500000f << 0.996898700000f << 0.004202400000f << endr
    << 560.f << 0.594500000000f << 0.995000000000f << 0.003900000000f << endr
    << 561.f << 0.611220900000f << 0.992600500000f << 0.003623200000f << endr
    << 562.f << 0.627975800000f << 0.989742600000f << 0.003370600000f << endr
    << 563.f << 0.644760200000f << 0.986444400000f << 0.003141400000f << endr
    << 564.f << 0.661569700000f << 0.982724100000f << 0.002934800000f << endr
    << 565.f << 0.678400000000f << 0.978600000000f << 0.002749999000f << endr
    << 566.f << 0.695239200000f << 0.974083700000f << 0.002585200000f << endr
    << 567.f << 0.712058600000f << 0.969171200000f << 0.002438600000f << endr
    << 568.f << 0.728828400000f << 0.963856800000f << 0.002309400000f << endr
    << 569.f << 0.745518800000f << 0.958134900000f << 0.002196800000f << endr
    << 570.f << 0.762100000000f << 0.952000000000f << 0.002100000000f << endr
    << 571.f << 0.778543200000f << 0.945450400000f << 0.002017733000f << endr
    << 572.f << 0.794825600000f << 0.938499200000f << 0.001948200000f << endr
    << 573.f << 0.810926400000f << 0.931162800000f << 0.001889800000f << endr
    << 574.f << 0.826824800000f << 0.923457600000f << 0.001840933000f << endr
    << 575.f << 0.842500000000f << 0.915400000000f << 0.001800000000f << endr
    << 576.f << 0.857932500000f << 0.907006400000f << 0.001766267000f << endr
    << 577.f << 0.873081600000f << 0.898277200000f << 0.001737800000f << endr
    << 578.f << 0.887894400000f << 0.889204800000f << 0.001711200000f << endr
    << 579.f << 0.902318100000f << 0.879781600000f << 0.001683067000f << endr
    << 580.f << 0.916300000000f << 0.870000000000f << 0.001650001000f << endr
    << 581.f << 0.929799500000f << 0.859861300000f << 0.001610133000f << endr
    << 582.f << 0.942798400000f << 0.849392000000f << 0.001564400000f << endr
    << 583.f << 0.955277600000f << 0.838622000000f << 0.001513600000f << endr
    << 584.f << 0.967217900000f << 0.827581300000f << 0.001458533000f << endr
    << 585.f << 0.978600000000f << 0.816300000000f << 0.001400000000f << endr
    << 586.f << 0.989385600000f << 0.804794700000f << 0.001336667000f << endr
    << 587.f << 0.999548800000f << 0.793082000000f << 0.001270000000f << endr
    << 588.f << 1.009089200000f << 0.781192000000f << 0.001205000000f << endr
    << 589.f << 1.018006400000f << 0.769154700000f << 0.001146667000f << endr
    << 590.f << 1.026300000000f << 0.757000000000f << 0.001100000000f << endr
    << 591.f << 1.033982700000f << 0.744754100000f << 0.001068800000f << endr
    << 592.f << 1.040986000000f << 0.732422400000f << 0.001049400000f << endr
    << 593.f << 1.047188000000f << 0.720003600000f << 0.001035600000f << endr
    << 594.f << 1.052466700000f << 0.707496500000f << 0.001021200000f << endr
    << 595.f << 1.056700000000f << 0.694900000000f << 0.001000000000f << endr
    << 596.f << 1.059794400000f << 0.682219200000f << 0.000968640000f << endr
    << 597.f << 1.061799200000f << 0.669471600000f << 0.000929920000f << endr
    << 598.f << 1.062806800000f << 0.656674400000f << 0.000886880000f << endr
    << 599.f << 1.062909600000f << 0.643844800000f << 0.000842560000f << endr
    << 600.f << 1.062200000000f << 0.631000000000f << 0.000800000000f << endr
    << 601.f << 1.060735200000f << 0.618155500000f << 0.000760960000f << endr
    << 602.f << 1.058443600000f << 0.605314400000f << 0.000723680000f << endr
    << 603.f << 1.055224400000f << 0.592475600000f << 0.000685920000f << endr
    << 604.f << 1.050976800000f << 0.579637900000f << 0.000645440000f << endr
    << 605.f << 1.045600000000f << 0.566800000000f << 0.000600000000f << endr
    << 606.f << 1.039036900000f << 0.553961100000f << 0.000547866700f << endr
    << 607.f << 1.031360800000f << 0.541137200000f << 0.000491600000f << endr
    << 608.f << 1.022666200000f << 0.528352800000f << 0.000435400000f << endr
    << 609.f << 1.013047700000f << 0.515632300000f << 0.000383466700f << endr
    << 610.f << 1.002600000000f << 0.503000000000f << 0.000340000000f << endr
    << 611.f << 0.991367500000f << 0.490468800000f << 0.000307253300f << endr
    << 612.f << 0.979331400000f << 0.478030400000f << 0.000283160000f << endr
    << 613.f << 0.966491600000f << 0.465677600000f << 0.000265440000f << endr
    << 614.f << 0.952847900000f << 0.453403200000f << 0.000251813300f << endr
    << 615.f << 0.938400000000f << 0.441200000000f << 0.000240000000f << endr
    << 616.f << 0.923194000000f << 0.429080000000f << 0.000229546700f << endr
    << 617.f << 0.907244000000f << 0.417036000000f << 0.000220640000f << endr
    << 618.f << 0.890502000000f << 0.405032000000f << 0.000211960000f << endr
    << 619.f << 0.872920000000f << 0.393032000000f << 0.000202186700f << endr
    << 620.f << 0.854449900000f << 0.381000000000f << 0.000190000000f << endr
    << 621.f << 0.835084000000f << 0.368918400000f << 0.000174213300f << endr
    << 622.f << 0.814946000000f << 0.356827200000f << 0.000155640000f << endr
    << 623.f << 0.794186000000f << 0.344776800000f << 0.000135960000f << endr
    << 624.f << 0.772954000000f << 0.332817600000f << 0.000116853300f << endr
    << 625.f << 0.751400000000f << 0.321000000000f << 0.000100000000f << endr
    << 626.f << 0.729583600000f << 0.309338100000f << 0.000086133330f << endr
    << 627.f << 0.707588800000f << 0.297850400000f << 0.000074600000f << endr
    << 628.f << 0.685602200000f << 0.286593600000f << 0.000065000000f << endr
    << 629.f << 0.663810400000f << 0.275624500000f << 0.000056933330f << endr
    << 630.f << 0.642400000000f << 0.265000000000f << 0.000049999990f << endr
    << 631.f << 0.621514900000f << 0.254763200000f << 0.000044160000f << endr
    << 632.f << 0.601113800000f << 0.244889600000f << 0.000039480000f << endr
    << 633.f << 0.581105200000f << 0.235334400000f << 0.000035720000f << endr
    << 634.f << 0.561397700000f << 0.226052800000f << 0.000032640000f << endr
    << 635.f << 0.541900000000f << 0.217000000000f << 0.000030000000f << endr
    << 636.f << 0.522599500000f << 0.208161600000f << 0.000027653330f << endr
    << 637.f << 0.503546400000f << 0.199548800000f << 0.000025560000f << endr
    << 638.f << 0.484743600000f << 0.191155200000f << 0.000023640000f << endr
    << 639.f << 0.466193900000f << 0.182974400000f << 0.000021813330f << endr
    << 640.f << 0.447900000000f << 0.175000000000f << 0.000020000000f << endr
    << 641.f << 0.429861300000f << 0.167223500000f << 0.000018133330f << endr
    << 642.f << 0.412098000000f << 0.159646400000f << 0.000016200000f << endr
    << 643.f << 0.394644000000f << 0.152277600000f << 0.000014200000f << endr
    << 644.f << 0.377533300000f << 0.145125900000f << 0.000012133330f << endr
    << 645.f << 0.360800000000f << 0.138200000000f << 0.000010000000f << endr
    << 646.f << 0.344456300000f << 0.131500300000f << 0.000007733333f << endr
    << 647.f << 0.328516800000f << 0.125024800000f << 0.000005400000f << endr
    << 648.f << 0.313019200000f << 0.118779200000f << 0.000003200000f << endr
    << 649.f << 0.298001100000f << 0.112769100000f << 0.000001333333f << endr
    << 650.f << 0.283500000000f << 0.107000000000f << 0.000000000000f << endr
    << 651.f << 0.269544800000f << 0.101476200000f << 0.000000000000f << endr
    << 652.f << 0.256118400000f << 0.096188640000f << 0.000000000000f << endr
    << 653.f << 0.243189600000f << 0.091122960000f << 0.000000000000f << endr
    << 654.f << 0.230727200000f << 0.086264850000f << 0.000000000000f << endr
    << 655.f << 0.218700000000f << 0.081600000000f << 0.000000000000f << endr
    << 656.f << 0.207097100000f << 0.077120640000f << 0.000000000000f << endr
    << 657.f << 0.195923200000f << 0.072825520000f << 0.000000000000f << endr
    << 658.f << 0.185170800000f << 0.068710080000f << 0.000000000000f << endr
    << 659.f << 0.174832300000f << 0.064769760000f << 0.000000000000f << endr
    << 660.f << 0.164900000000f << 0.061000000000f << 0.000000000000f << endr
    << 661.f << 0.155366700000f << 0.057396210000f << 0.000000000000f << endr
    << 662.f << 0.146230000000f << 0.053955040000f << 0.000000000000f << endr
    << 663.f << 0.137490000000f << 0.050673760000f << 0.000000000000f << endr
    << 664.f << 0.129146700000f << 0.047549650000f << 0.000000000000f << endr
    << 665.f << 0.121200000000f << 0.044580000000f << 0.000000000000f << endr
    << 666.f << 0.113639700000f << 0.041758720000f << 0.000000000000f << endr
    << 667.f << 0.106465000000f << 0.039084960000f << 0.000000000000f << endr
    << 668.f << 0.099690440000f << 0.036563840000f << 0.000000000000f << endr
    << 669.f << 0.093330610000f << 0.034200480000f << 0.000000000000f << endr
    << 670.f << 0.087400000000f << 0.032000000000f << 0.000000000000f << endr
    << 671.f << 0.081900960000f << 0.029962610000f << 0.000000000000f << endr
    << 672.f << 0.076804280000f << 0.028076640000f << 0.000000000000f << endr
    << 673.f << 0.072077120000f << 0.026329360000f << 0.000000000000f << endr
    << 674.f << 0.067686640000f << 0.024708050000f << 0.000000000000f << endr
    << 675.f << 0.063600000000f << 0.023200000000f << 0.000000000000f << endr
    << 676.f << 0.059806850000f << 0.021800770000f << 0.000000000000f << endr
    << 677.f << 0.056282160000f << 0.020501120000f << 0.000000000000f << endr
    << 678.f << 0.052971040000f << 0.019281080000f << 0.000000000000f << endr
    << 679.f << 0.049818610000f << 0.018120690000f << 0.000000000000f << endr
    << 680.f << 0.046770000000f << 0.017000000000f << 0.000000000000f << endr
    << 681.f << 0.043784050000f << 0.015903790000f << 0.000000000000f << endr
    << 682.f << 0.040875360000f << 0.014837180000f << 0.000000000000f << endr
    << 683.f << 0.038072640000f << 0.013810680000f << 0.000000000000f << endr
    << 684.f << 0.035404610000f << 0.012834780000f << 0.000000000000f << endr
    << 685.f << 0.032900000000f << 0.011920000000f << 0.000000000000f << endr
    << 686.f << 0.030564190000f << 0.011068310000f << 0.000000000000f << endr
    << 687.f << 0.028380560000f << 0.010273390000f << 0.000000000000f << endr
    << 688.f << 0.026344840000f << 0.009533311000f << 0.000000000000f << endr
    << 689.f << 0.024452750000f << 0.008846157000f << 0.000000000000f << endr
    << 690.f << 0.022700000000f << 0.008210000000f << 0.000000000000f << endr
    << 691.f << 0.021084290000f << 0.007623781000f << 0.000000000000f << endr
    << 692.f << 0.019599880000f << 0.007085424000f << 0.000000000000f << endr
    << 693.f << 0.018237320000f << 0.006591476000f << 0.000000000000f << endr
    << 694.f << 0.016987170000f << 0.006138485000f << 0.000000000000f << endr
    << 695.f << 0.015840000000f << 0.005723000000f << 0.000000000000f << endr
    << 696.f << 0.014790640000f << 0.005343059000f << 0.000000000000f << endr
    << 697.f << 0.013831320000f << 0.004995796000f << 0.000000000000f << endr
    << 698.f << 0.012948680000f << 0.004676404000f << 0.000000000000f << endr
    << 699.f << 0.012129200000f << 0.004380075000f << 0.000000000000f << endr
    << 700.f << 0.011359160000f << 0.004102000000f << 0.000000000000f << endr
    << 701.f << 0.010629350000f << 0.003838453000f << 0.000000000000f << endr
    << 702.f << 0.009938846000f << 0.003589099000f << 0.000000000000f << endr
    << 703.f << 0.009288422000f << 0.003354219000f << 0.000000000000f << endr
    << 704.f << 0.008678854000f << 0.003134093000f << 0.000000000000f << endr
    << 705.f << 0.008110916000f << 0.002929000000f << 0.000000000000f << endr
    << 706.f << 0.007582388000f << 0.002738139000f << 0.000000000000f << endr
    << 707.f << 0.007088746000f << 0.002559876000f << 0.000000000000f << endr
    << 708.f << 0.006627313000f << 0.002393244000f << 0.000000000000f << endr
    << 709.f << 0.006195408000f << 0.002237275000f << 0.000000000000f << endr
    << 710.f << 0.005790346000f << 0.002091000000f << 0.000000000000f << endr
    << 711.f << 0.005409826000f << 0.001953587000f << 0.000000000000f << endr
    << 712.f << 0.005052583000f << 0.001824580000f << 0.000000000000f << endr
    << 713.f << 0.004717512000f << 0.001703580000f << 0.000000000000f << endr
    << 714.f << 0.004403507000f << 0.001590187000f << 0.000000000000f << endr
    << 715.f << 0.004109457000f << 0.001484000000f << 0.000000000000f << endr
    << 716.f << 0.003833913000f << 0.001384496000f << 0.000000000000f << endr
    << 717.f << 0.003575748000f << 0.001291268000f << 0.000000000000f << endr
    << 718.f << 0.003334342000f << 0.001204092000f << 0.000000000000f << endr
    << 719.f << 0.003109075000f << 0.001122744000f << 0.000000000000f << endr
    << 720.f << 0.002899327000f << 0.001047000000f << 0.000000000000f << endr
    << 721.f << 0.002704348000f << 0.000976589600f << 0.000000000000f << endr
    << 722.f << 0.002523020000f << 0.000911108800f << 0.000000000000f << endr
    << 723.f << 0.002354168000f << 0.000850133200f << 0.000000000000f << endr
    << 724.f << 0.002196616000f << 0.000793238400f << 0.000000000000f << endr
    << 725.f << 0.002049190000f << 0.000740000000f << 0.000000000000f << endr
    << 726.f << 0.001910960000f << 0.000690082700f << 0.000000000000f << endr
    << 727.f << 0.001781438000f << 0.000643310000f << 0.000000000000f << endr
    << 728.f << 0.001660110000f << 0.000599496000f << 0.000000000000f << endr
    << 729.f << 0.001546459000f << 0.000558454700f << 0.000000000000f << endr
    << 730.f << 0.001439971000f << 0.000520000000f << 0.000000000000f << endr
    << 731.f << 0.001340042000f << 0.000483913600f << 0.000000000000f << endr
    << 732.f << 0.001246275000f << 0.000450052800f << 0.000000000000f << endr
    << 733.f << 0.001158471000f << 0.000418345200f << 0.000000000000f << endr
    << 734.f << 0.001076430000f << 0.000388718400f << 0.000000000000f << endr
    << 735.f << 0.000999949300f << 0.000361100000f << 0.000000000000f << endr
    << 736.f << 0.000928735800f << 0.000335383500f << 0.000000000000f << endr
    << 737.f << 0.000862433200f << 0.000311440400f << 0.000000000000f << endr
    << 738.f << 0.000800750300f << 0.000289165600f << 0.000000000000f << endr
    << 739.f << 0.000743396000f << 0.000268453900f << 0.000000000000f << endr
    << 740.f << 0.000690078600f << 0.000249200000f << 0.000000000000f << endr
    << 741.f << 0.000640515600f << 0.000231301900f << 0.000000000000f << endr
    << 742.f << 0.000594502100f << 0.000214685600f << 0.000000000000f << endr
    << 743.f << 0.000551864600f << 0.000199288400f << 0.000000000000f << endr
    << 744.f << 0.000512429000f << 0.000185047500f << 0.000000000000f << endr
    << 745.f << 0.000476021300f << 0.000171900000f << 0.000000000000f << endr
    << 746.f << 0.000442453600f << 0.000159778100f << 0.000000000000f << endr
    << 747.f << 0.000411511700f << 0.000148604400f << 0.000000000000f << endr
    << 748.f << 0.000382981400f << 0.000138301600f << 0.000000000000f << endr
    << 749.f << 0.000356649100f << 0.000128792500f << 0.000000000000f << endr
    << 750.f << 0.000332301100f << 0.000120000000f << 0.000000000000f << endr
    << 751.f << 0.000309758600f << 0.000111859500f << 0.000000000000f << endr
    << 752.f << 0.000288887100f << 0.000104322400f << 0.000000000000f << endr
    << 753.f << 0.000269539400f << 0.000097335600f << 0.000000000000f << endr
    << 754.f << 0.000251568200f << 0.000090845870f << 0.000000000000f << endr
    << 755.f << 0.000234826100f << 0.000084800000f << 0.000000000000f << endr
    << 756.f << 0.000219171000f << 0.000079146670f << 0.000000000000f << endr
    << 757.f << 0.000204525800f << 0.000073858000f << 0.000000000000f << endr
    << 758.f << 0.000190840500f << 0.000068916000f << 0.000000000000f << endr
    << 759.f << 0.000178065400f << 0.000064302670f << 0.000000000000f << endr
    << 760.f << 0.000166150500f << 0.000060000000f << 0.000000000000f << endr
    << 761.f << 0.000155023600f << 0.000055981870f << 0.000000000000f << endr
    << 762.f << 0.000144621900f << 0.000052225600f << 0.000000000000f << endr
    << 763.f << 0.000134909800f << 0.000048718400f << 0.000000000000f << endr
    << 764.f << 0.000125852000f << 0.000045447470f << 0.000000000000f << endr
    << 765.f << 0.000117413000f << 0.000042400000f << 0.000000000000f << endr
    << 766.f << 0.000109551500f << 0.000039561040f << 0.000000000000f << endr
    << 767.f << 0.000102224500f << 0.000036915120f << 0.000000000000f << endr
    << 768.f << 0.000095394450f << 0.000034448680f << 0.000000000000f << endr
    << 769.f << 0.000089023900f << 0.000032148160f << 0.000000000000f << endr
    << 770.f << 0.000083075270f << 0.000030000000f << 0.000000000000f << endr
    << 771.f << 0.000077512690f << 0.000027991250f << 0.000000000000f << endr
    << 772.f << 0.000072313040f << 0.000026113560f << 0.000000000000f << endr
    << 773.f << 0.000067457780f << 0.000024360240f << 0.000000000000f << endr
    << 774.f << 0.000062928440f << 0.000022724610f << 0.000000000000f << endr
    << 775.f << 0.000058706520f << 0.000021200000f << 0.000000000000f << endr
    << 776.f << 0.000054770280f << 0.000019778550f << 0.000000000000f << endr
    << 777.f << 0.000051099180f << 0.000018452850f << 0.000000000000f << endr
    << 778.f << 0.000047676540f << 0.000017216870f << 0.000000000000f << endr
    << 779.f << 0.000044485670f << 0.000016064590f << 0.000000000000f << endr
    << 780.f << 0.000041509940f << 0.000014990000f << 0.000000000000f << endr
    << 781.f << 0.000038733240f << 0.000013987280f << 0.000000000000f << endr
    << 782.f << 0.000036142030f << 0.000013051550f << 0.000000000000f << endr
    << 783.f << 0.000033723520f << 0.000012178180f << 0.000000000000f << endr
    << 784.f << 0.000031464870f << 0.000011362540f << 0.000000000000f << endr
    << 785.f << 0.000029353260f << 0.000010600000f << 0.000000000000f << endr
    << 786.f << 0.000027375730f << 0.000009885877f << 0.000000000000f << endr
    << 787.f << 0.000025524330f << 0.000009217304f << 0.000000000000f << endr
    << 788.f << 0.000023793760f << 0.000008592362f << 0.000000000000f << endr
    << 789.f << 0.000022178700f << 0.000008009133f << 0.000000000000f << endr
    << 790.f << 0.000020673830f << 0.000007465700f << 0.000000000000f << endr
    << 791.f << 0.000019272260f << 0.000006959567f << 0.000000000000f << endr
    << 792.f << 0.000017966400f << 0.000006487995f << 0.000000000000f << endr
    << 793.f << 0.000016749910f << 0.000006048699f << 0.000000000000f << endr
    << 794.f << 0.000015616480f << 0.000005639396f << 0.000000000000f << endr
    << 795.f << 0.000014559770f << 0.000005257800f << 0.000000000000f << endr
    << 796.f << 0.000013573870f << 0.000004901771f << 0.000000000000f << endr
    << 797.f << 0.000012654360f << 0.000004569720f << 0.000000000000f << endr
    << 798.f << 0.000011797230f << 0.000004260194f << 0.000000000000f << endr
    << 799.f << 0.000010998440f << 0.000003971739f << 0.000000000000f << endr
    << 800.f << 0.000010253980f << 0.000003702900f << 0.000000000000f << endr
    << 801.f << 0.000009559646f << 0.000003452163f << 0.000000000000f << endr
    << 802.f << 0.000008912044f << 0.000003218302f << 0.000000000000f << endr
    << 803.f << 0.000008308358f << 0.000003000300f << 0.000000000000f << endr
    << 804.f << 0.000007745769f << 0.000002797139f << 0.000000000000f << endr
    << 805.f << 0.000007221456f << 0.000002607800f << 0.000000000000f << endr
    << 806.f << 0.000006732475f << 0.000002431220f << 0.000000000000f << endr
    << 807.f << 0.000006276423f << 0.000002266531f << 0.000000000000f << endr
    << 808.f << 0.000005851304f << 0.000002113013f << 0.000000000000f << endr
    << 809.f << 0.000005455118f << 0.000001969943f << 0.000000000000f << endr
    << 810.f << 0.000005085868f << 0.000001836600f << 0.000000000000f << endr
    << 811.f << 0.000004741466f << 0.000001712230f << 0.000000000000f << endr
    << 812.f << 0.000004420236f << 0.000001596228f << 0.000000000000f << endr
    << 813.f << 0.000004120783f << 0.000001488090f << 0.000000000000f << endr
    << 814.f << 0.000003841716f << 0.000001387314f << 0.000000000000f << endr
    << 815.f << 0.000003581652f << 0.000001293400f << 0.000000000000f << endr
    << 816.f << 0.000003339127f << 0.000001205820f << 0.000000000000f << endr
    << 817.f << 0.000003112949f << 0.000001124143f << 0.000000000000f << endr
    << 818.f << 0.000002902121f << 0.000001048009f << 0.000000000000f << endr
    << 819.f << 0.000002705645f << 0.000000977058f << 0.000000000000f << endr
    << 820.f << 0.000002522525f << 0.000000910930f << 0.000000000000f << endr
    << 821.f << 0.000002351726f << 0.000000849251f << 0.000000000000f << endr
    << 822.f << 0.000002192415f << 0.000000791721f << 0.000000000000f << endr
    << 823.f << 0.000002043902f << 0.000000738090f << 0.000000000000f << endr
    << 824.f << 0.000001905497f << 0.000000688110f << 0.000000000000f << endr
    << 825.f << 0.000001776509f << 0.000000641530f << 0.000000000000f << endr
    << 826.f << 0.000001656215f << 0.000000598090f << 0.000000000000f << endr
    << 827.f << 0.000001544022f << 0.000000557575f << 0.000000000000f << endr
    << 828.f << 0.000001439440f << 0.000000519808f << 0.000000000000f << endr
    << 829.f << 0.000001341977f << 0.000000484612f << 0.000000000000f << endr
    << 830.f << 0.000001251141f << 0.000000451810f << 0.000000000000f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}
void MKCC::CMF_CIE_1964_FULL(fvec &lambda, fvec &xFcn, fvec &yFcn, fvec &zFcn)
{
    fmat::fixed<471,4> cmf;
    cmf << 360.f << 0.000000122200f << 0.000000013398f << 0.000000535027f << endr
      << 361.f << 0.000000185138f << 0.000000020294f << 0.000000810720f << endr
        << 362.f << 0.000000278830f << 0.000000030560f << 0.000001221200f << endr
        << 363.f << 0.000000417470f << 0.000000045740f << 0.000001828700f << endr
        << 364.f << 0.000000621330f << 0.000000068050f << 0.000002722200f << endr
        << 365.f << 0.000000919270f << 0.000000100650f << 0.000004028300f << endr
        << 366.f << 0.000001351980f << 0.000000147980f << 0.000005925700f << endr
        << 367.f << 0.000001976540f << 0.000000216270f << 0.000008665100f << endr
        << 368.f << 0.000002872500f << 0.000000314200f << 0.000012596000f << endr
        << 369.f << 0.000004149500f << 0.000000453700f << 0.000018201000f << endr
        << 370.f << 0.000005958600f << 0.000000651100f << 0.000026143700f << endr
        << 371.f << 0.000008505600f << 0.000000928800f << 0.000037330000f << endr
        << 372.f << 0.000012068600f << 0.000001317500f << 0.000052987000f << endr
        << 373.f << 0.000017022600f << 0.000001857200f << 0.000074764000f << endr
        << 374.f << 0.000023868000f << 0.000002602000f << 0.000104870000f << endr
        << 375.f << 0.000033266000f << 0.000003625000f << 0.000146220000f << endr
        << 376.f << 0.000046087000f << 0.000005019000f << 0.000202660000f << endr
        << 377.f << 0.000063472000f << 0.000006907000f << 0.000279230000f << endr
        << 378.f << 0.000086892000f << 0.000009449000f << 0.000382450000f << endr
        << 379.f << 0.000118246000f << 0.000012848000f << 0.000520720000f << endr
        << 380.f << 0.000159952000f << 0.000017364000f << 0.000704776000f << endr
        << 381.f << 0.000215080000f << 0.000023327000f << 0.000948230000f << endr
        << 382.f << 0.000287490000f << 0.000031150000f << 0.001268200000f << endr
        << 383.f << 0.000381990000f << 0.000041350000f << 0.001686100000f << endr
        << 384.f << 0.000504550000f << 0.000054560000f << 0.002228500000f << endr
        << 385.f << 0.000662440000f << 0.000071560000f << 0.002927800000f << endr
        << 386.f << 0.000864500000f << 0.000093300000f << 0.003823700000f << endr
        << 387.f << 0.001121500000f << 0.000120870000f << 0.004964200000f << endr
        << 388.f << 0.001446160000f << 0.000155640000f << 0.006406700000f << endr
        << 389.f << 0.001853590000f << 0.000199200000f << 0.008219300000f << endr
        << 390.f << 0.002361600000f << 0.000253400000f << 0.010482200000f << endr
        << 391.f << 0.002990600000f << 0.000320200000f << 0.013289000000f << endr
        << 392.f << 0.003764500000f << 0.000402400000f << 0.016747000000f << endr
        << 393.f << 0.004710200000f << 0.000502300000f << 0.020980000000f << endr
        << 394.f << 0.005858100000f << 0.000623200000f << 0.026127000000f << endr
        << 395.f << 0.007242300000f << 0.000768500000f << 0.032344000000f << endr
        << 396.f << 0.008899600000f << 0.000941700000f << 0.039802000000f << endr
        << 397.f << 0.010870900000f << 0.001147800000f << 0.048691000000f << endr
        << 398.f << 0.013198900000f << 0.001390300000f << 0.059210000000f << endr
        << 399.f << 0.015929200000f << 0.001674000000f << 0.071576000000f << endr
        << 400.f << 0.019109700000f << 0.002004400000f << 0.086010900000f << endr
        << 401.f << 0.022788000000f << 0.002386000000f << 0.102740000000f << endr
        << 402.f << 0.027011000000f << 0.002822000000f << 0.122000000000f << endr
        << 403.f << 0.031829000000f << 0.003319000000f << 0.144020000000f << endr
        << 404.f << 0.037278000000f << 0.003880000000f << 0.168990000000f << endr
        << 405.f << 0.043400000000f << 0.004509000000f << 0.197120000000f << endr
        << 406.f << 0.050223000000f << 0.005209000000f << 0.228570000000f << endr
        << 407.f << 0.057764000000f << 0.005985000000f << 0.263470000000f << endr
        << 408.f << 0.066038000000f << 0.006833000000f << 0.301900000000f << endr
        << 409.f << 0.075033000000f << 0.007757000000f << 0.343870000000f << endr
        << 410.f << 0.084736000000f << 0.008756000000f << 0.389366000000f << endr
        << 411.f << 0.095041000000f << 0.009816000000f << 0.437970000000f << endr
        << 412.f << 0.105836000000f << 0.010918000000f << 0.489220000000f << endr
        << 413.f << 0.117066000000f << 0.012058000000f << 0.542900000000f << endr
        << 414.f << 0.128682000000f << 0.013237000000f << 0.598810000000f << endr
        << 415.f << 0.140638000000f << 0.014456000000f << 0.656760000000f << endr
        << 416.f << 0.152893000000f << 0.015717000000f << 0.716580000000f << endr
        << 417.f << 0.165416000000f << 0.017025000000f << 0.778120000000f << endr
        << 418.f << 0.178191000000f << 0.018399000000f << 0.841310000000f << endr
        << 419.f << 0.191214000000f << 0.019848000000f << 0.906110000000f << endr
        << 420.f << 0.204492000000f << 0.021391000000f << 0.972542000000f << endr
        << 421.f << 0.217650000000f << 0.022992000000f << 1.038900000000f << endr
        << 422.f << 0.230267000000f << 0.024598000000f << 1.103100000000f << endr
        << 423.f << 0.242311000000f << 0.026213000000f << 1.165100000000f << endr
        << 424.f << 0.253793000000f << 0.027841000000f << 1.224900000000f << endr
        << 425.f << 0.264737000000f << 0.029497000000f << 1.282500000000f << endr
        << 426.f << 0.275195000000f << 0.031195000000f << 1.338200000000f << endr
        << 427.f << 0.285301000000f << 0.032927000000f << 1.392600000000f << endr
        << 428.f << 0.295143000000f << 0.034738000000f << 1.446100000000f << endr
        << 429.f << 0.304869000000f << 0.036654000000f << 1.499400000000f << endr
        << 430.f << 0.314679000000f << 0.038676000000f << 1.553480000000f << endr
        << 431.f << 0.324355000000f << 0.040792000000f << 1.607200000000f << endr
        << 432.f << 0.333570000000f << 0.042946000000f << 1.658900000000f << endr
        << 433.f << 0.342243000000f << 0.045114000000f << 1.708200000000f << endr
        << 434.f << 0.350312000000f << 0.047333000000f << 1.754800000000f << endr
        << 435.f << 0.357719000000f << 0.049602000000f << 1.798500000000f << endr
        << 436.f << 0.364482000000f << 0.051934000000f << 1.839200000000f << endr
        << 437.f << 0.370493000000f << 0.054337000000f << 1.876600000000f << endr
        << 438.f << 0.375727000000f << 0.056822000000f << 1.910500000000f << endr
        << 439.f << 0.380158000000f << 0.059399000000f << 1.940800000000f << endr
        << 440.f << 0.383734000000f << 0.062077000000f << 1.967280000000f << endr
        << 441.f << 0.386327000000f << 0.064737000000f << 1.989100000000f << endr
        << 442.f << 0.387858000000f << 0.067285000000f << 2.005700000000f << endr
        << 443.f << 0.388396000000f << 0.069764000000f << 2.017400000000f << endr
        << 444.f << 0.387978000000f << 0.072218000000f << 2.024400000000f << endr
        << 445.f << 0.386726000000f << 0.074704000000f << 2.027300000000f << endr
        << 446.f << 0.384696000000f << 0.077272000000f << 2.026400000000f << endr
        << 447.f << 0.382006000000f << 0.079979000000f << 2.022300000000f << endr
        << 448.f << 0.378709000000f << 0.082874000000f << 2.015300000000f << endr
        << 449.f << 0.374915000000f << 0.086000000000f << 2.006000000000f << endr
        << 450.f << 0.370702000000f << 0.089456000000f << 1.994800000000f << endr
        << 451.f << 0.366089000000f << 0.092947000000f << 1.981400000000f << endr
        << 452.f << 0.361045000000f << 0.096275000000f << 1.965300000000f << endr
        << 453.f << 0.355518000000f << 0.099535000000f << 1.946400000000f << endr
        << 454.f << 0.349486000000f << 0.102829000000f << 1.924800000000f << endr
        << 455.f << 0.342957000000f << 0.106256000000f << 1.900700000000f << endr
        << 456.f << 0.335893000000f << 0.109901000000f << 1.874100000000f << endr
        << 457.f << 0.328284000000f << 0.113835000000f << 1.845100000000f << endr
        << 458.f << 0.320150000000f << 0.118167000000f << 1.813900000000f << endr
        << 459.f << 0.311475000000f << 0.122932000000f << 1.780600000000f << endr
        << 460.f << 0.302273000000f << 0.128201000000f << 1.745370000000f << endr
        << 461.f << 0.292858000000f << 0.133457000000f << 1.709100000000f << endr
        << 462.f << 0.283502000000f << 0.138323000000f << 1.672300000000f << endr
        << 463.f << 0.274044000000f << 0.143042000000f << 1.634700000000f << endr
        << 464.f << 0.264263000000f << 0.147787000000f << 1.595600000000f << endr
        << 465.f << 0.254085000000f << 0.152761000000f << 1.554900000000f << endr
        << 466.f << 0.243392000000f << 0.158102000000f << 1.512200000000f << endr
        << 467.f << 0.232187000000f << 0.163941000000f << 1.467300000000f << endr
        << 468.f << 0.220488000000f << 0.170362000000f << 1.419900000000f << endr
        << 469.f << 0.208198000000f << 0.177425000000f << 1.370000000000f << endr
        << 470.f << 0.195618000000f << 0.185190000000f << 1.317560000000f << endr
        << 471.f << 0.183034000000f << 0.193025000000f << 1.262400000000f << endr
        << 472.f << 0.170222000000f << 0.200313000000f << 1.205000000000f << endr
        << 473.f << 0.157348000000f << 0.207156000000f << 1.146600000000f << endr
        << 474.f << 0.144650000000f << 0.213644000000f << 1.088000000000f << endr
        << 475.f << 0.132349000000f << 0.219940000000f << 1.030200000000f << endr
        << 476.f << 0.120584000000f << 0.226170000000f << 0.973830000000f << endr
        << 477.f << 0.109456000000f << 0.232467000000f << 0.919430000000f << endr
        << 478.f << 0.099042000000f << 0.239025000000f << 0.867460000000f << endr
        << 479.f << 0.089388000000f << 0.245997000000f << 0.818280000000f << endr
        << 480.f << 0.080507000000f << 0.253589000000f << 0.772125000000f << endr
        << 481.f << 0.072034000000f << 0.261876000000f << 0.728290000000f << endr
        << 482.f << 0.063710000000f << 0.270643000000f << 0.686040000000f << endr
        << 483.f << 0.055694000000f << 0.279645000000f << 0.645530000000f << endr
        << 484.f << 0.048117000000f << 0.288694000000f << 0.606850000000f << endr
        << 485.f << 0.041072000000f << 0.297665000000f << 0.570060000000f << endr
        << 486.f << 0.034642000000f << 0.306469000000f << 0.535220000000f << endr
        << 487.f << 0.028896000000f << 0.315035000000f << 0.502340000000f << endr
        << 488.f << 0.023876000000f << 0.323335000000f << 0.471400000000f << endr
        << 489.f << 0.019628000000f << 0.331366000000f << 0.442390000000f << endr
        << 490.f << 0.016172000000f << 0.339133000000f << 0.415254000000f << endr
        << 491.f << 0.013300000000f << 0.347860000000f << 0.390024000000f << endr
        << 492.f << 0.010759000000f << 0.358326000000f << 0.366399000000f << endr
        << 493.f << 0.008542000000f << 0.370001000000f << 0.344015000000f << endr
        << 494.f << 0.006661000000f << 0.382464000000f << 0.322689000000f << endr
        << 495.f << 0.005132000000f << 0.395379000000f << 0.302356000000f << endr
        << 496.f << 0.003982000000f << 0.408482000000f << 0.283036000000f << endr
        << 497.f << 0.003239000000f << 0.421588000000f << 0.264816000000f << endr
        << 498.f << 0.002934000000f << 0.434619000000f << 0.247848000000f << endr
        << 499.f << 0.003114000000f << 0.447601000000f << 0.232318000000f << endr
        << 500.f << 0.003816000000f << 0.460777000000f << 0.218502000000f << endr
        << 501.f << 0.005095000000f << 0.474340000000f << 0.205851000000f << endr
        << 502.f << 0.006936000000f << 0.488200000000f << 0.193596000000f << endr
        << 503.f << 0.009299000000f << 0.502340000000f << 0.181736000000f << endr
        << 504.f << 0.012147000000f << 0.516740000000f << 0.170281000000f << endr
        << 505.f << 0.015444000000f << 0.531360000000f << 0.159249000000f << endr
        << 506.f << 0.019156000000f << 0.546190000000f << 0.148673000000f << endr
        << 507.f << 0.023250000000f << 0.561180000000f << 0.138609000000f << endr
        << 508.f << 0.027690000000f << 0.576290000000f << 0.129096000000f << endr
        << 509.f << 0.032444000000f << 0.591500000000f << 0.120215000000f << endr
        << 510.f << 0.037465000000f << 0.606741000000f << 0.112044000000f << endr
        << 511.f << 0.042956000000f << 0.622150000000f << 0.104710000000f << endr
        << 512.f << 0.049114000000f << 0.637830000000f << 0.098196000000f << endr
        << 513.f << 0.055920000000f << 0.653710000000f << 0.092361000000f << endr
        << 514.f << 0.063349000000f << 0.669680000000f << 0.087088000000f << endr
        << 515.f << 0.071358000000f << 0.685660000000f << 0.082248000000f << endr
        << 516.f << 0.079901000000f << 0.701550000000f << 0.077744000000f << endr
        << 517.f << 0.088909000000f << 0.717230000000f << 0.073456000000f << endr
        << 518.f << 0.098293000000f << 0.732570000000f << 0.069268000000f << endr
        << 519.f << 0.107949000000f << 0.747460000000f << 0.065060000000f << endr
        << 520.f << 0.117749000000f << 0.761757000000f << 0.060709000000f << endr
        << 521.f << 0.127839000000f << 0.775340000000f << 0.056457000000f << endr
        << 522.f << 0.138450000000f << 0.788220000000f << 0.052609000000f << endr
        << 523.f << 0.149516000000f << 0.800460000000f << 0.049122000000f << endr
        << 524.f << 0.161041000000f << 0.812140000000f << 0.045954000000f << endr
        << 525.f << 0.172953000000f << 0.823330000000f << 0.043050000000f << endr
        << 526.f << 0.185209000000f << 0.834120000000f << 0.040368000000f << endr
        << 527.f << 0.197755000000f << 0.844600000000f << 0.037839000000f << endr
        << 528.f << 0.210538000000f << 0.854870000000f << 0.035384000000f << endr
        << 529.f << 0.223460000000f << 0.865040000000f << 0.032949000000f << endr
        << 530.f << 0.236491000000f << 0.875211000000f << 0.030451000000f << endr
        << 531.f << 0.249633000000f << 0.885370000000f << 0.028029000000f << endr
        << 532.f << 0.262972000000f << 0.895370000000f << 0.025862000000f << endr
        << 533.f << 0.276515000000f << 0.905150000000f << 0.023920000000f << endr
        << 534.f << 0.290269000000f << 0.914650000000f << 0.022174000000f << endr
        << 535.f << 0.304213000000f << 0.923810000000f << 0.020584000000f << endr
        << 536.f << 0.318361000000f << 0.932550000000f << 0.019127000000f << endr
        << 537.f << 0.332705000000f << 0.940810000000f << 0.017740000000f << endr
        << 538.f << 0.347232000000f << 0.948520000000f << 0.016403000000f << endr
        << 539.f << 0.361926000000f << 0.955600000000f << 0.015064000000f << endr
        << 540.f << 0.376772000000f << 0.961988000000f << 0.013676000000f << endr
        << 541.f << 0.391683000000f << 0.967540000000f << 0.012308000000f << endr
        << 542.f << 0.406594000000f << 0.972230000000f << 0.011056000000f << endr
        << 543.f << 0.421539000000f << 0.976170000000f << 0.009915000000f << endr
        << 544.f << 0.436517000000f << 0.979460000000f << 0.008872000000f << endr
        << 545.f << 0.451584000000f << 0.982200000000f << 0.007918000000f << endr
        << 546.f << 0.466782000000f << 0.984520000000f << 0.007030000000f << endr
        << 547.f << 0.482147000000f << 0.986520000000f << 0.006223000000f << endr
        << 548.f << 0.497738000000f << 0.988320000000f << 0.005453000000f << endr
        << 549.f << 0.513606000000f << 0.990020000000f << 0.004714000000f << endr
        << 550.f << 0.529826000000f << 0.991761000000f << 0.003988000000f << endr
        << 551.f << 0.546440000000f << 0.993530000000f << 0.003289000000f << endr
        << 552.f << 0.563426000000f << 0.995230000000f << 0.002646000000f << endr
        << 553.f << 0.580726000000f << 0.996770000000f << 0.002063000000f << endr
        << 554.f << 0.598290000000f << 0.998090000000f << 0.001533000000f << endr
        << 555.f << 0.616053000000f << 0.999110000000f << 0.001091000000f << endr
        << 556.f << 0.633948000000f << 0.999770000000f << 0.000711000000f << endr
        << 557.f << 0.651901000000f << 1.000000000000f << 0.000407000000f << endr
        << 558.f << 0.669824000000f << 0.999710000000f << 0.000184000000f << endr
        << 559.f << 0.687632000000f << 0.998850000000f << 0.000047000000f << endr
        << 560.f << 0.705224000000f << 0.997340000000f << 0.000000000000f << endr
        << 561.f << 0.722773000000f << 0.995260000000f << 0.000000000000f << endr
        << 562.f << 0.740483000000f << 0.992740000000f << 0.000000000000f << endr
        << 563.f << 0.758273000000f << 0.989750000000f << 0.000000000000f << endr
        << 564.f << 0.776083000000f << 0.986300000000f << 0.000000000000f << endr
        << 565.f << 0.793832000000f << 0.982380000000f << 0.000000000000f << endr
        << 566.f << 0.811436000000f << 0.977980000000f << 0.000000000000f << endr
        << 567.f << 0.828822000000f << 0.973110000000f << 0.000000000000f << endr
        << 568.f << 0.845879000000f << 0.967740000000f << 0.000000000000f << endr
        << 569.f << 0.862525000000f << 0.961890000000f << 0.000000000000f << endr
        << 570.f << 0.878655000000f << 0.955552000000f << 0.000000000000f << endr
        << 571.f << 0.894208000000f << 0.948601000000f << 0.000000000000f << endr
        << 572.f << 0.909206000000f << 0.940981000000f << 0.000000000000f << endr
        << 573.f << 0.923672000000f << 0.932798000000f << 0.000000000000f << endr
        << 574.f << 0.937638000000f << 0.924158000000f << 0.000000000000f << endr
        << 575.f << 0.951162000000f << 0.915175000000f << 0.000000000000f << endr
        << 576.f << 0.964283000000f << 0.905954000000f << 0.000000000000f << endr
        << 577.f << 0.977068000000f << 0.896608000000f << 0.000000000000f << endr
        << 578.f << 0.989590000000f << 0.887249000000f << 0.000000000000f << endr
        << 579.f << 1.001910000000f << 0.877986000000f << 0.000000000000f << endr
        << 580.f << 1.014160000000f << 0.868934000000f << 0.000000000000f << endr
        << 581.f << 1.026500000000f << 0.860164000000f << 0.000000000000f << endr
        << 582.f << 1.038800000000f << 0.851519000000f << 0.000000000000f << endr
        << 583.f << 1.051000000000f << 0.842963000000f << 0.000000000000f << endr
        << 584.f << 1.062900000000f << 0.834393000000f << 0.000000000000f << endr
        << 585.f << 1.074300000000f << 0.825623000000f << 0.000000000000f << endr
        << 586.f << 1.085200000000f << 0.816764000000f << 0.000000000000f << endr
        << 587.f << 1.095200000000f << 0.807544000000f << 0.000000000000f << endr
        << 588.f << 1.104200000000f << 0.797947000000f << 0.000000000000f << endr
        << 589.f << 1.112000000000f << 0.787893000000f << 0.000000000000f << endr
        << 590.f << 1.118520000000f << 0.777405000000f << 0.000000000000f << endr
        << 591.f << 1.123800000000f << 0.766490000000f << 0.000000000000f << endr
        << 592.f << 1.128000000000f << 0.755309000000f << 0.000000000000f << endr
        << 593.f << 1.131100000000f << 0.743845000000f << 0.000000000000f << endr
        << 594.f << 1.133200000000f << 0.732190000000f << 0.000000000000f << endr
        << 595.f << 1.134300000000f << 0.720353000000f << 0.000000000000f << endr
        << 596.f << 1.134300000000f << 0.708281000000f << 0.000000000000f << endr
        << 597.f << 1.133300000000f << 0.696055000000f << 0.000000000000f << endr
        << 598.f << 1.131200000000f << 0.683621000000f << 0.000000000000f << endr
        << 599.f << 1.128100000000f << 0.671048000000f << 0.000000000000f << endr
        << 600.f << 1.123990000000f << 0.658341000000f << 0.000000000000f << endr
        << 601.f << 1.118900000000f << 0.645545000000f << 0.000000000000f << endr
        << 602.f << 1.112900000000f << 0.632718000000f << 0.000000000000f << endr
        << 603.f << 1.105900000000f << 0.619815000000f << 0.000000000000f << endr
        << 604.f << 1.098000000000f << 0.606887000000f << 0.000000000000f << endr
        << 605.f << 1.089100000000f << 0.593878000000f << 0.000000000000f << endr
        << 606.f << 1.079200000000f << 0.580781000000f << 0.000000000000f << endr
        << 607.f << 1.068400000000f << 0.567653000000f << 0.000000000000f << endr
        << 608.f << 1.056700000000f << 0.554490000000f << 0.000000000000f << endr
        << 609.f << 1.044000000000f << 0.541228000000f << 0.000000000000f << endr
        << 610.f << 1.030480000000f << 0.527963000000f << 0.000000000000f << endr
        << 611.f << 1.016000000000f << 0.514634000000f << 0.000000000000f << endr
        << 612.f << 1.000800000000f << 0.501363000000f << 0.000000000000f << endr
        << 613.f << 0.984790000000f << 0.488124000000f << 0.000000000000f << endr
        << 614.f << 0.968080000000f << 0.474935000000f << 0.000000000000f << endr
        << 615.f << 0.950740000000f << 0.461834000000f << 0.000000000000f << endr
        << 616.f << 0.932800000000f << 0.448823000000f << 0.000000000000f << endr
        << 617.f << 0.914340000000f << 0.435917000000f << 0.000000000000f << endr
        << 618.f << 0.895390000000f << 0.423153000000f << 0.000000000000f << endr
        << 619.f << 0.876030000000f << 0.410526000000f << 0.000000000000f << endr
        << 620.f << 0.856297000000f << 0.398057000000f << 0.000000000000f << endr
        << 621.f << 0.836350000000f << 0.385835000000f << 0.000000000000f << endr
        << 622.f << 0.816290000000f << 0.373951000000f << 0.000000000000f << endr
        << 623.f << 0.796050000000f << 0.362311000000f << 0.000000000000f << endr
        << 624.f << 0.775610000000f << 0.350863000000f << 0.000000000000f << endr
        << 625.f << 0.754930000000f << 0.339554000000f << 0.000000000000f << endr
        << 626.f << 0.733990000000f << 0.328309000000f << 0.000000000000f << endr
        << 627.f << 0.712780000000f << 0.317118000000f << 0.000000000000f << endr
        << 628.f << 0.691290000000f << 0.305936000000f << 0.000000000000f << endr
        << 629.f << 0.669520000000f << 0.294737000000f << 0.000000000000f << endr
        << 630.f << 0.647467000000f << 0.283493000000f << 0.000000000000f << endr
        << 631.f << 0.625110000000f << 0.272222000000f << 0.000000000000f << endr
        << 632.f << 0.602520000000f << 0.260990000000f << 0.000000000000f << endr
        << 633.f << 0.579890000000f << 0.249877000000f << 0.000000000000f << endr
        << 634.f << 0.557370000000f << 0.238946000000f << 0.000000000000f << endr
        << 635.f << 0.535110000000f << 0.228254000000f << 0.000000000000f << endr
        << 636.f << 0.513240000000f << 0.217853000000f << 0.000000000000f << endr
        << 637.f << 0.491860000000f << 0.207780000000f << 0.000000000000f << endr
        << 638.f << 0.471080000000f << 0.198072000000f << 0.000000000000f << endr
        << 639.f << 0.450960000000f << 0.188748000000f << 0.000000000000f << endr
        << 640.f << 0.431567000000f << 0.179828000000f << 0.000000000000f << endr
        << 641.f << 0.412870000000f << 0.171285000000f << 0.000000000000f << endr
        << 642.f << 0.394750000000f << 0.163059000000f << 0.000000000000f << endr
        << 643.f << 0.377210000000f << 0.155151000000f << 0.000000000000f << endr
        << 644.f << 0.360190000000f << 0.147535000000f << 0.000000000000f << endr
        << 645.f << 0.343690000000f << 0.140211000000f << 0.000000000000f << endr
        << 646.f << 0.327690000000f << 0.133170000000f << 0.000000000000f << endr
        << 647.f << 0.312170000000f << 0.126400000000f << 0.000000000000f << endr
        << 648.f << 0.297110000000f << 0.119892000000f << 0.000000000000f << endr
        << 649.f << 0.282500000000f << 0.113640000000f << 0.000000000000f << endr
        << 650.f << 0.268329000000f << 0.107633000000f << 0.000000000000f << endr
        << 651.f << 0.254590000000f << 0.101870000000f << 0.000000000000f << endr
        << 652.f << 0.241300000000f << 0.096347000000f << 0.000000000000f << endr
        << 653.f << 0.228480000000f << 0.091063000000f << 0.000000000000f << endr
        << 654.f << 0.216140000000f << 0.086010000000f << 0.000000000000f << endr
        << 655.f << 0.204300000000f << 0.081187000000f << 0.000000000000f << endr
        << 656.f << 0.192950000000f << 0.076583000000f << 0.000000000000f << endr
        << 657.f << 0.182110000000f << 0.072198000000f << 0.000000000000f << endr
        << 658.f << 0.171770000000f << 0.068024000000f << 0.000000000000f << endr
        << 659.f << 0.161920000000f << 0.064052000000f << 0.000000000000f << endr
        << 660.f << 0.152568000000f << 0.060281000000f << 0.000000000000f << endr
        << 661.f << 0.143670000000f << 0.056697000000f << 0.000000000000f << endr
        << 662.f << 0.135200000000f << 0.053292000000f << 0.000000000000f << endr
        << 663.f << 0.127130000000f << 0.050059000000f << 0.000000000000f << endr
        << 664.f << 0.119480000000f << 0.046998000000f << 0.000000000000f << endr
        << 665.f << 0.112210000000f << 0.044096000000f << 0.000000000000f << endr
        << 666.f << 0.105310000000f << 0.041345000000f << 0.000000000000f << endr
        << 667.f << 0.098786000000f << 0.038750700000f << 0.000000000000f << endr
        << 668.f << 0.092610000000f << 0.036297800000f << 0.000000000000f << endr
        << 669.f << 0.086773000000f << 0.033983200000f << 0.000000000000f << endr
        << 670.f << 0.081260600000f << 0.031800400000f << 0.000000000000f << endr
        << 671.f << 0.076048000000f << 0.029739500000f << 0.000000000000f << endr
        << 672.f << 0.071114000000f << 0.027791800000f << 0.000000000000f << endr
        << 673.f << 0.066454000000f << 0.025955100000f << 0.000000000000f << endr
        << 674.f << 0.062062000000f << 0.024226300000f << 0.000000000000f << endr
        << 675.f << 0.057930000000f << 0.022601700000f << 0.000000000000f << endr
        << 676.f << 0.054050000000f << 0.021077900000f << 0.000000000000f << endr
        << 677.f << 0.050412000000f << 0.019650500000f << 0.000000000000f << endr
        << 678.f << 0.047006000000f << 0.018315300000f << 0.000000000000f << endr
        << 679.f << 0.043823000000f << 0.017068600000f << 0.000000000000f << endr
        << 680.f << 0.040850800000f << 0.015905100000f << 0.000000000000f << endr
        << 681.f << 0.038072000000f << 0.014818300000f << 0.000000000000f << endr
        << 682.f << 0.035468000000f << 0.013800800000f << 0.000000000000f << endr
        << 683.f << 0.033031000000f << 0.012849500000f << 0.000000000000f << endr
        << 684.f << 0.030753000000f << 0.011960700000f << 0.000000000000f << endr
        << 685.f << 0.028623000000f << 0.011130300000f << 0.000000000000f << endr
        << 686.f << 0.026635000000f << 0.010355500000f << 0.000000000000f << endr
        << 687.f << 0.024781000000f << 0.009633200000f << 0.000000000000f << endr
        << 688.f << 0.023052000000f << 0.008959900000f << 0.000000000000f << endr
        << 689.f << 0.021441000000f << 0.008332400000f << 0.000000000000f << endr
        << 690.f << 0.019941300000f << 0.007748800000f << 0.000000000000f << endr
        << 691.f << 0.018544000000f << 0.007204600000f << 0.000000000000f << endr
        << 692.f << 0.017241000000f << 0.006697500000f << 0.000000000000f << endr
        << 693.f << 0.016027000000f << 0.006225100000f << 0.000000000000f << endr
        << 694.f << 0.014896000000f << 0.005785000000f << 0.000000000000f << endr
        << 695.f << 0.013842000000f << 0.005375100000f << 0.000000000000f << endr
        << 696.f << 0.012862000000f << 0.004994100000f << 0.000000000000f << endr
        << 697.f << 0.011949000000f << 0.004639200000f << 0.000000000000f << endr
        << 698.f << 0.011100000000f << 0.004309300000f << 0.000000000000f << endr
        << 699.f << 0.010311000000f << 0.004002800000f << 0.000000000000f << endr
        << 700.f << 0.009576880000f << 0.003717740000f << 0.000000000000f << endr
        << 701.f << 0.008894000000f << 0.003452620000f << 0.000000000000f << endr
        << 702.f << 0.008258100000f << 0.003205830000f << 0.000000000000f << endr
        << 703.f << 0.007666400000f << 0.002976230000f << 0.000000000000f << endr
        << 704.f << 0.007116300000f << 0.002762810000f << 0.000000000000f << endr
        << 705.f << 0.006605200000f << 0.002564560000f << 0.000000000000f << endr
        << 706.f << 0.006130600000f << 0.002380480000f << 0.000000000000f << endr
        << 707.f << 0.005690300000f << 0.002209710000f << 0.000000000000f << endr
        << 708.f << 0.005281900000f << 0.002051320000f << 0.000000000000f << endr
        << 709.f << 0.004903300000f << 0.001904490000f << 0.000000000000f << endr
        << 710.f << 0.004552630000f << 0.001768470000f << 0.000000000000f << endr
        << 711.f << 0.004227500000f << 0.001642360000f << 0.000000000000f << endr
        << 712.f << 0.003925800000f << 0.001525350000f << 0.000000000000f << endr
        << 713.f << 0.003645700000f << 0.001416720000f << 0.000000000000f << endr
        << 714.f << 0.003385900000f << 0.001315950000f << 0.000000000000f << endr
        << 715.f << 0.003144700000f << 0.001222390000f << 0.000000000000f << endr
        << 716.f << 0.002920800000f << 0.001135550000f << 0.000000000000f << endr
        << 717.f << 0.002713000000f << 0.001054940000f << 0.000000000000f << endr
        << 718.f << 0.002520200000f << 0.000980140000f << 0.000000000000f << endr
        << 719.f << 0.002341100000f << 0.000910660000f << 0.000000000000f << endr
        << 720.f << 0.002174960000f << 0.000846190000f << 0.000000000000f << endr
        << 721.f << 0.002020600000f << 0.000786290000f << 0.000000000000f << endr
        << 722.f << 0.001877300000f << 0.000730680000f << 0.000000000000f << endr
        << 723.f << 0.001744100000f << 0.000678990000f << 0.000000000000f << endr
        << 724.f << 0.001620500000f << 0.000631010000f << 0.000000000000f << endr
        << 725.f << 0.001505700000f << 0.000586440000f << 0.000000000000f << endr
        << 726.f << 0.001399200000f << 0.000545110000f << 0.000000000000f << endr
        << 727.f << 0.001300400000f << 0.000506720000f << 0.000000000000f << endr
        << 728.f << 0.001208700000f << 0.000471110000f << 0.000000000000f << endr
        << 729.f << 0.001123600000f << 0.000438050000f << 0.000000000000f << endr
        << 730.f << 0.001044760000f << 0.000407410000f << 0.000000000000f << endr
        << 731.f << 0.000971560000f << 0.000378962000f << 0.000000000000f << endr
        << 732.f << 0.000903600000f << 0.000352543000f << 0.000000000000f << endr
        << 733.f << 0.000840480000f << 0.000328001000f << 0.000000000000f << endr
        << 734.f << 0.000781870000f << 0.000305208000f << 0.000000000000f << endr
        << 735.f << 0.000727450000f << 0.000284041000f << 0.000000000000f << endr
        << 736.f << 0.000676900000f << 0.000264375000f << 0.000000000000f << endr
        << 737.f << 0.000629960000f << 0.000246109000f << 0.000000000000f << endr
        << 738.f << 0.000586370000f << 0.000229143000f << 0.000000000000f << endr
        << 739.f << 0.000545870000f << 0.000213376000f << 0.000000000000f << endr
        << 740.f << 0.000508258000f << 0.000198730000f << 0.000000000000f << endr
        << 741.f << 0.000473300000f << 0.000185115000f << 0.000000000000f << endr
        << 742.f << 0.000440800000f << 0.000172454000f << 0.000000000000f << endr
        << 743.f << 0.000410580000f << 0.000160678000f << 0.000000000000f << endr
        << 744.f << 0.000382490000f << 0.000149730000f << 0.000000000000f << endr
        << 745.f << 0.000356380000f << 0.000139550000f << 0.000000000000f << endr
        << 746.f << 0.000332110000f << 0.000130086000f << 0.000000000000f << endr
        << 747.f << 0.000309550000f << 0.000121290000f << 0.000000000000f << endr
        << 748.f << 0.000288580000f << 0.000113106000f << 0.000000000000f << endr
        << 749.f << 0.000269090000f << 0.000105501000f << 0.000000000000f << endr
        << 750.f << 0.000250969000f << 0.000098428000f << 0.000000000000f << endr
        << 751.f << 0.000234130000f << 0.000091853000f << 0.000000000000f << endr
        << 752.f << 0.000218470000f << 0.000085738000f << 0.000000000000f << endr
        << 753.f << 0.000203910000f << 0.000080048000f << 0.000000000000f << endr
        << 754.f << 0.000190350000f << 0.000074751000f << 0.000000000000f << endr
        << 755.f << 0.000177730000f << 0.000069819000f << 0.000000000000f << endr
        << 756.f << 0.000165970000f << 0.000065222000f << 0.000000000000f << endr
        << 757.f << 0.000155020000f << 0.000060939000f << 0.000000000000f << endr
        << 758.f << 0.000144800000f << 0.000056942000f << 0.000000000000f << endr
        << 759.f << 0.000135280000f << 0.000053217000f << 0.000000000000f << endr
        << 760.f << 0.000126390000f << 0.000049737000f << 0.000000000000f << endr
        << 761.f << 0.000118100000f << 0.000046491000f << 0.000000000000f << endr
        << 762.f << 0.000110370000f << 0.000043464000f << 0.000000000000f << endr
        << 763.f << 0.000103150000f << 0.000040635000f << 0.000000000000f << endr
        << 764.f << 0.000096427000f << 0.000038000000f << 0.000000000000f << endr
        << 765.f << 0.000090151000f << 0.000035540500f << 0.000000000000f << endr
        << 766.f << 0.000084294000f << 0.000033244800f << 0.000000000000f << endr
        << 767.f << 0.000078830000f << 0.000031100600f << 0.000000000000f << endr
        << 768.f << 0.000073729000f << 0.000029099000f << 0.000000000000f << endr
        << 769.f << 0.000068969000f << 0.000027230700f << 0.000000000000f << endr
        << 770.f << 0.000064525800f << 0.000025486000f << 0.000000000000f << endr
        << 771.f << 0.000060376000f << 0.000023856100f << 0.000000000000f << endr
        << 772.f << 0.000056500000f << 0.000022333200f << 0.000000000000f << endr
        << 773.f << 0.000052880000f << 0.000020910400f << 0.000000000000f << endr
        << 774.f << 0.000049498000f << 0.000019580800f << 0.000000000000f << endr
        << 775.f << 0.000046339000f << 0.000018338400f << 0.000000000000f << endr
        << 776.f << 0.000043389000f << 0.000017177700f << 0.000000000000f << endr
        << 777.f << 0.000040634000f << 0.000016093400f << 0.000000000000f << endr
        << 778.f << 0.000038060000f << 0.000015080000f << 0.000000000000f << endr
        << 779.f << 0.000035657000f << 0.000014133600f << 0.000000000000f << endr
        << 780.f << 0.000033411700f << 0.000013249000f << 0.000000000000f << endr
        << 781.f << 0.000031315000f << 0.000012422600f << 0.000000000000f << endr
        << 782.f << 0.000029355000f << 0.000011649900f << 0.000000000000f << endr
        << 783.f << 0.000027524000f << 0.000010927700f << 0.000000000000f << endr
        << 784.f << 0.000025811000f << 0.000010251900f << 0.000000000000f << endr
        << 785.f << 0.000024209000f << 0.000009619600f << 0.000000000000f << endr
        << 786.f << 0.000022711000f << 0.000009028100f << 0.000000000000f << endr
        << 787.f << 0.000021308000f << 0.000008474000f << 0.000000000000f << endr
        << 788.f << 0.000019994000f << 0.000007954800f << 0.000000000000f << endr
        << 789.f << 0.000018764000f << 0.000007468600f << 0.000000000000f << endr
        << 790.f << 0.000017611500f << 0.000007012800f << 0.000000000000f << endr
        << 791.f << 0.000016532000f << 0.000006585800f << 0.000000000000f << endr
        << 792.f << 0.000015521000f << 0.000006185700f << 0.000000000000f << endr
        << 793.f << 0.000014574000f << 0.000005810700f << 0.000000000000f << endr
        << 794.f << 0.000013686000f << 0.000005459000f << 0.000000000000f << endr
        << 795.f << 0.000012855000f << 0.000005129800f << 0.000000000000f << endr
        << 796.f << 0.000012075000f << 0.000004820600f << 0.000000000000f << endr
        << 797.f << 0.000011345000f << 0.000004531200f << 0.000000000000f << endr
        << 798.f << 0.000010659000f << 0.000004259100f << 0.000000000000f << endr
        << 799.f << 0.000010017000f << 0.000004004200f << 0.000000000000f << endr
        << 800.f << 0.000009413630f << 0.000003764730f << 0.000000000000f << endr
        << 801.f << 0.000008847900f << 0.000003539950f << 0.000000000000f << endr
        << 802.f << 0.000008317100f << 0.000003329140f << 0.000000000000f << endr
        << 803.f << 0.000007819000f << 0.000003131150f << 0.000000000000f << endr
        << 804.f << 0.000007351600f << 0.000002945290f << 0.000000000000f << endr
        << 805.f << 0.000006913000f << 0.000002770810f << 0.000000000000f << endr
        << 806.f << 0.000006501500f << 0.000002607050f << 0.000000000000f << endr
        << 807.f << 0.000006115300f << 0.000002453290f << 0.000000000000f << endr
        << 808.f << 0.000005752900f << 0.000002308940f << 0.000000000000f << endr
        << 809.f << 0.000005412700f << 0.000002173380f << 0.000000000000f << endr
        << 810.f << 0.000005093470f << 0.000002046130f << 0.000000000000f << endr
        << 811.f << 0.000004793800f << 0.000001926620f << 0.000000000000f << endr
        << 812.f << 0.000004512500f << 0.000001814400f << 0.000000000000f << endr
        << 813.f << 0.000004248300f << 0.000001708950f << 0.000000000000f << endr
        << 814.f << 0.000004000200f << 0.000001609880f << 0.000000000000f << endr
        << 815.f << 0.000003767100f << 0.000001516770f << 0.000000000000f << endr
        << 816.f << 0.000003548000f << 0.000001429210f << 0.000000000000f << endr
        << 817.f << 0.000003342100f << 0.000001346860f << 0.000000000000f << endr
        << 818.f << 0.000003148500f << 0.000001269450f << 0.000000000000f << endr
        << 819.f << 0.000002966500f << 0.000001196620f << 0.000000000000f << endr
        << 820.f << 0.000002795310f << 0.000001128090f << 0.000000000000f << endr
        << 821.f << 0.000002634500f << 0.000001063680f << 0.000000000000f << endr
        << 822.f << 0.000002483400f << 0.000001003130f << 0.000000000000f << endr
        << 823.f << 0.000002341400f << 0.000000946220f << 0.000000000000f << endr
        << 824.f << 0.000002207800f << 0.000000892630f << 0.000000000000f << endr
        << 825.f << 0.000002082000f << 0.000000842160f << 0.000000000000f << endr
        << 826.f << 0.000001963600f << 0.000000794640f << 0.000000000000f << endr
        << 827.f << 0.000001851900f << 0.000000749780f << 0.000000000000f << endr
        << 828.f << 0.000001746500f << 0.000000707440f << 0.000000000000f << endr
        << 829.f << 0.000001647100f << 0.000000667480f << 0.000000000000f << endr
        << 830.f << 0.000001553140f << 0.000000629700f << 0.000000000000f << endr;

    lambda = cmf.col(0);
    xFcn = cmf.col(1);
    yFcn = cmf.col(2);
    zFcn = cmf.col(3);
}
#endif

// currently unused
/* float cmf_5[][4];
cmf =
{
  { 380, 2.689900e-003, 2.000000e-004, 1.226000e-002 },
    { 385, 5.310500e-003, 3.955600e-004, 2.422200e-002 },
    { 390, 1.078100e-002, 8.000000e-004, 4.925000e-002 },
    { 395, 2.079200e-002, 1.545700e-003, 9.513500e-002 },
    { 400, 3.798100e-002, 2.800000e-003, 1.740900e-001 },
    { 405, 6.315700e-002, 4.656200e-003, 2.901300e-001 },
    { 410, 9.994100e-002, 7.400000e-003, 4.605300e-001 },
    { 415, 1.582400e-001, 1.177900e-002, 7.316600e-001 },
    { 420, 2.294800e-001, 1.750000e-002, 1.065800e+000 },
    { 425, 2.810800e-001, 2.267800e-002, 1.314600e+000 },
    { 430, 3.109500e-001, 2.730000e-002, 1.467200e+000 },
    { 435, 3.307200e-001, 3.258400e-002, 1.579600e+000 },
    { 440, 3.333600e-001, 3.790000e-002, 1.616600e+000 },
    { 445, 3.167200e-001, 4.239100e-002, 1.568200e+000 },
    { 450, 2.888200e-001, 4.680000e-002, 1.471700e+000 },
    { 455, 2.596900e-001, 5.212200e-002, 1.374000e+000 },
    { 460, 2.327600e-001, 6.000000e-002, 1.291700e+000 },
    { 465, 2.099900e-001, 7.294200e-002, 1.235600e+000 },
    { 470, 1.747600e-001, 9.098000e-002, 1.113800e+000 },
    { 475, 1.328700e-001, 1.128400e-001, 9.422000e-001 },
    { 480, 9.194400e-002, 1.390200e-001, 7.559600e-001 },
    { 485, 5.698500e-002, 1.698700e-001, 5.864000e-001 },
    { 490, 3.173100e-002, 2.080200e-001, 4.466900e-001 },
    { 495, 1.461300e-002, 2.580800e-001, 3.411600e-001 },
    { 500, 4.849100e-003, 3.230000e-001, 2.643700e-001 },
    { 505, 2.321500e-003, 4.054000e-001, 2.059400e-001 },
    { 510, 9.289900e-003, 5.030000e-001, 1.544500e-001 },
    { 515, 2.927800e-002, 6.081100e-001, 1.091800e-001 },
    { 520, 6.379100e-002, 7.100000e-001, 7.658500e-002 },
    { 525, 1.108100e-001, 7.951000e-001, 5.622700e-002 },
    { 530, 1.669200e-001, 8.620000e-001, 4.136600e-002 },
    { 535, 2.276800e-001, 9.150500e-001, 2.935300e-002 },
    { 540, 2.926900e-001, 9.540000e-001, 2.004200e-002 },
    { 545, 3.622500e-001, 9.800400e-001, 1.331200e-002 },
    { 550, 4.363500e-001, 9.949500e-001, 8.782300e-003 },
    { 555, 5.151300e-001, 1.000100e+000, 5.857300e-003 },
    { 560, 5.974800e-001, 9.950000e-001, 4.049300e-003 },
    { 565, 6.812100e-001, 9.787500e-001, 2.921700e-003 },
    { 570, 7.642500e-001, 9.520000e-001, 2.277100e-003 },
    { 575, 8.439400e-001, 9.155800e-001, 1.970600e-003 },
    { 580, 9.163500e-001, 8.700000e-001, 1.806600e-003 },
    { 585, 9.770300e-001, 8.162300e-001, 1.544900e-003 },
    { 590, 1.023000e+000, 7.570000e-001, 1.234800e-003 },
    { 595, 1.051300e+000, 6.948300e-001, 1.117700e-003 },
    { 600, 1.055000e+000, 6.310000e-001, 9.056400e-004 },
    { 605, 1.036200e+000, 5.665400e-001, 6.946700e-004 },
    { 610, 9.923900e-001, 5.030000e-001, 4.288500e-004 },
    { 615, 9.286100e-001, 4.417200e-001, 3.181700e-004 },
    { 620, 8.434600e-001, 3.810000e-001, 2.559800e-004 },
    { 625, 7.398300e-001, 3.205200e-001, 1.567900e-004 },
    { 630, 6.328900e-001, 2.650000e-001, 9.769400e-005 },
    { 635, 5.335100e-001, 2.170200e-001, 6.894400e-005 },
    { 640, 4.406200e-001, 1.750000e-001, 5.116500e-005 },
    { 645, 3.545300e-001, 1.381200e-001, 3.601600e-005 },
    { 650, 2.786200e-001, 1.070000e-001, 2.423800e-005 },
    { 655, 2.148500e-001, 8.165200e-002, 1.691500e-005 },
    { 660, 1.616100e-001, 6.100000e-002, 1.190600e-005 },
    { 665, 1.182000e-001, 4.432700e-002, 8.148900e-006 },
    { 670, 8.575300e-002, 3.200000e-002, 5.600600e-006 },
    { 675, 6.307700e-002, 2.345400e-002, 3.954400e-006 },
    { 680, 4.583400e-002, 1.700000e-002, 2.791200e-006 },
    { 685, 3.205700e-002, 1.187200e-002, 1.917600e-006 },
    { 690, 2.218700e-002, 8.210000e-003, 1.313500e-006 },
    { 695, 1.561200e-002, 5.772300e-003, 9.151900e-007 },
    { 700, 1.109800e-002, 4.102000e-003, 6.476700e-007 },
    { 705, 7.923300e-003, 2.929100e-003, 4.635200e-007 },
    { 710, 5.653100e-003, 2.091000e-003, 3.330400e-007 },
    { 715, 4.003900e-003, 1.482200e-003, 2.382300e-007 },
    { 720, 2.825300e-003, 1.047000e-003, 1.702600e-007 },
    { 725, 1.994700e-003, 7.401500e-004, 1.220700e-007 },
    { 730, 1.399400e-003, 5.200000e-004, 8.710700e-008 },
    { 735, 9.698000e-004, 3.609300e-004, 6.145500e-008 },
    { 740, 6.684700e-004, 2.492000e-004, 4.316200e-008 },
    { 745, 4.614100e-004, 1.723100e-004, 3.037900e-008 },
    { 750, 3.207300e-004, 1.200000e-004, 2.155400e-008 },
    { 755, 2.257300e-004, 8.462000e-005, 1.549300e-008 },
    { 760, 1.597300e-004, 6.000000e-005, 1.120400e-008 },
    { 765, 1.127500e-004, 4.244600e-005, 8.087300e-009 },
    { 770, 7.951300e-005, 3.000000e-005, 5.834000e-009 },
    { 775, 5.608700e-005, 2.121000e-005, 4.211000e-009 },
    { 780, 3.954100e-005, 1.498900e-005, 3.038300e-009 },
    { 785, 2.785200e-005, 1.058400e-005, 2.190700e-009 },
    { 790, 1.959700e-005, 7.465600e-006, 1.577800e-009 },
    { 795, 1.377000e-005, 5.259200e-006, 1.134800e-009 },
    { 800, 9.670000e-006, 3.702800e-006, 8.156500e-010 },
    { 805, 6.791800e-006, 2.607600e-006, 5.862600e-010 },
    { 810, 4.770600e-006, 1.836500e-006, 4.213800e-010 },
    { 815, 3.355000e-006, 1.295000e-006, 3.031900e-010 },
    { 820, 2.353400e-006, 9.109200e-007, 2.175300e-010 },
    { 825, 1.637700e-006, 6.356400e-007, 1.547600e-010 }
};*/
