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

 - Acknowledgments: Some portions of this file are based on the example codes
   of ITK/VTK library from Kitware, QT API from Nokia. I would like to thank
   annonymous help by various software engineering communities.

*****************************************************************************/

#if defined(_WIN32) || defined(_WIN64)
  #define snprintf _snprintf
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
#endif

#include <string>

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QWidget>

#include <vtkStringArray.h>
#include <vtkJPEGReader.h>
#include <vtkTIFFReader.h>
#include <vtkPNGReader.h>
#include <vtkBMPReader.h>

#include <vtkPolyData.h>
#include <vtkDataArray.h>
#include <vtkTexture.h>
#include <vtkOBJReader.h>
//#include <vtkPLYReader.h> // vtk orignal (no reading texture coordinates)
#include "vtkPLYReader2.h" // mk revision with reading texture coordinates
#include "vtkVRMLSource2.h"
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkStringArray.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkActorCollection.h>
#include <vtkRenderer.h>
#include <vtkMapper.h>
#include "vtkOpenEXR.h"

#include "../mainWindow.h"

#include "readCHEROb.h"
#include "../function/mkColorConvert.h"
#include "../function/mkTools.hpp"

#define GAMMA (2.2f)

#define DEFAULTKA (0.2)
#define DEFAULTKD (1.0)
#define DEFAULTKS (1.0)
#define DEFAULTD (1.0)
#define DEFAULTTR (1.0)
#define DEFAULTILLUM (2)
#define DEFAULTNS (0.0)

#define CLAMP8(pixel)	( (( pixel > 255 ))		? 255	: (( pixel < 0 )	? 0		: pixel ) )
#define CLAMP16(pixel)	( (( pixel > 65535 ))	? 65535	: (( pixel < 0 )	? 0		: pixel ) )
#define CLAMPN(pixel)	( (( pixel > 1.0f ))	? 1.0f	: (( pixel < 0.0f )	? 0.0f	: pixel ) )
#define CLAMPND(pixel)	( (( pixel > 1.0 ))		? 1.0	: (( pixel < 0.0 )	? 0.0	: pixel ) )

#include <armadillo>  // Beware Armadillo should not be in the header => conflict with ITK and VTK
using namespace arma;


bool ReadCHEROb::read3D(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData *(&polyData), vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn)
{
  QFileInfo fi(filename);
  QString extension = fi.suffix().toLower();

  //qDebug() << "file name: " << filename;

  if (extension==tr("obj")) {
    if (!readOBJ(filename, channelnames, wavelengths, texturefilenameout, materials, polyData, rgbTexture, hyperImageData, isTextureOn)) return false;
  }
  else if (extension==tr("ply")) {
    if (!readPLY(filename, channelnames, wavelengths, texturefilenameout, materials, polyData, rgbTexture, hyperImageData, isTextureOn)) return false;
  }
  else if (extension==tr("wrl")) {
    if (!readWRL(filename, channelnames, wavelengths, texturefilenameout, materials, polyData, rgbTexture, hyperImageData, isTextureOn)) return false;
  }
  else {
    cout << "file is not supported";
    return false;
  }

//#define _DEBUG // not oK
//#ifdef _DEBUG
//vtkSmartPointer<vtkImageData> outputrgb = vtkImageData::SafeDownCast(rgbTexture->GetInput());
//  mkDebug md;  md.qDebugImageData(outputrgb); // fine
////imagedata1:  512   512   1 :  1
////imagedata1 (10,10,0,1):  19
//#endif

//  if (polyData->GetPointData()) {
//      if (polyData->GetPointData()->GetTCoords()) {
//          qDebug() << "number of components (2) " << polyData->GetPointData()->GetTCoords()->GetNumberOfComponents();
//          qDebug() << "number of texture coordinates (2) " << polyData->GetPointData()->GetTCoords()->GetNumberOfTuples();
//      }
//  }
  return true;
}


bool ReadCHEROb::readWRL(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData *(&polyData), vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn)
{
  // parse filename
  QFileInfo fi(filename);

  QString extension = fi.suffix().toLower();
  if (extension!=tr("wrl")) {
//    qDebug() << "the wrl file was not found";
    return false;
  }

  std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String
//  const char *filenamesc = fnstr.c_str();

  vtkSmartPointer<vtkVRMLSource2> vtkWRLImport = vtkSmartPointer<vtkVRMLSource2>::New();
  vtkWRLImport->SetFileName(fnstr.c_str());
  if (vtkWRLImport)
      vtkWRLImport->Update();
  else
  {
      cerr << "no WRL imported!";
      return false;
  }

  int i=0;// temporally one object

  vtkSmartPointer<vtkPolyData> tempPoly = vtkSmartPointer<vtkPolyData>::New();
  tempPoly = vtkSmartPointer<vtkPolyData>::New();
  tempPoly = vtkWRLImport->GetOutput();
  if (tempPoly)
      tempPoly->Update();
  else
  {
      cerr << "no polygon detected!";
      return false;
  }

  polyData = vtkPolyData::New();
  polyData->DeepCopy(tempPoly);
#ifdef _DEBUG
  if (polyData->GetPointData()) {
      if (polyData->GetPointData()->GetTCoords()) {
          qDebug() << "number of components (2) " << polyData->GetPointData()->GetTCoords()->GetNumberOfComponents();
          qDebug() << "number of texture coordinates (2) " << polyData->GetPointData()->GetTCoords()->GetNumberOfTuples();
      }
  }
#endif
  if ( parseWRL2(filename, materials) )
  {
    std::string texturefn = materials[i].textureFilename;

    materials[i].fileType = WRLFILE;
    QString path =  fi.dir().path() + "/";
    QString texturefilename;
    texturefilename = path + texturefn.c_str();
    QFileInfo texturefi(texturefilename);

    if (texturefi.exists())
    {
      readATexture(texturefilename, channelnames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    } else {
      readAnyTexture(filename, channelnames, wavelengths, texturefilename, rgbTexture, hyperImageData, isTextureOn);
    }
    if(isTextureOn)
    {
        texturefilenameout = texturefilename;
    }
  } else {
      Material *currentMaterial = createMaterial();
      materials.push_back(*currentMaterial);
      materials[i].fileType = WRLFILE;
      isTextureOn = false;
  }

  return true;
} // end of class


// note that the original vtkOBJReader library reads texture coordinates.
bool ReadCHEROb::readOBJ(QString objfilename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData *(&polyData), vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn)
{
  // parse filename
  QFileInfo fi(objfilename);

  QString extension = fi.suffix().toLower();
  if (extension!=tr("obj")) {
//    qDebug() << "the obj file was not found";
    return false;
  }

  std::string fnstr = objfilename.toLocal8Bit().constData(); // QString -> Std. String
//  const char *filenamesc = fnstr.c_str();

  // read the obj file (only for vertex and index) - default support from Vtk
  vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
  reader->SetFileName(fnstr.c_str());
  reader->Update();

  // sending vertex and face to the polydata
  polyData = vtkPolyData::New();
  polyData->DeepCopy(reader->GetOutput());

  // making mtl path
  // parse filename
  QString path =  fi.dir().path() + "/";
  QString mtlfilenamein;

  if (parseOBJ(objfilename, mtlfilenamein))
  {
    QFileInfo mtlfn1(mtlfilenamein);
    QString mtlfilename = path + mtlfn1.fileName();
    QFileInfo mtlfn(mtlfilename);

	/*qDebug() << "mtlfilenamein = " << mtlfilenamein;
	qDebug() << "mtlfilename = " << mtlfilename;*/

    QString texturefilename;
    int i=0;
    if (mtlfn.exists())
    {
      // read mtl file.
      if (parseMTL(mtlfilename, materials))
      {
        std::string texturefn = materials[i].textureFilename;
        texturefilename = path + texturefn.c_str();
		//qDebug() << "texturefilename A = " << texturefilename;
        readATexture(texturefilename, channelnames, wavelengths, rgbTexture, hyperImageData, isTextureOn);

        // DT: fixing texture bug
        if(isTextureOn)
            texturefilenameout = texturefilename;

        materials[i].fileType = OBJFILE;
      } else {
        isTextureOn = false;
      }
    } else {
      readAnyTexture(objfilename, channelnames, wavelengths, texturefilename, rgbTexture, hyperImageData, isTextureOn);

      if(isTextureOn)
        texturefilenameout = texturefilename;

      Material *currentMaterial = createMaterial();
      materials.push_back(*currentMaterial);
      materials[i].fileType = OBJFILE;
    }
  //  qDebug() << "isTextureOn: " << isTextureOn;
  } else {
    isTextureOn = false;
  }
  return true;
} // end of class


bool ReadCHEROb::readPLY(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData *(&polyData), vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn)
{
  // parse filename
  QFileInfo fi(filename);

  QString extension = fi.suffix().toLower();
  if (extension!=tr("ply")) {
//    qDebug() << "the ply file was not found";
    return false;
  }

  std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String
//  const char *filenamesc = fnstr.c_str();

  //MK: the original vtk file only supports reading vertex and indices.
  //MK: this vtkPLYReader2 reads texture coordinates additionally.
  vtkSmartPointer<vtkPLYReader2> reader = vtkSmartPointer<vtkPLYReader2>::New();
  reader->SetFileName(fnstr.c_str());
  reader->Update();

  // sending vertex and face to the polydata
  polyData = vtkPolyData::New();
  polyData->DeepCopy(reader->GetOutput());

  if (polyData->GetNumberOfPolys() == 0)
      return false;

//  if (polyData->GetPointData()) {
//      if (polyData->GetPointData()->GetTCoords()) {
//          qDebug() << "number of components" << polyData->GetPointData()->GetTCoords()->GetNumberOfComponents();
//          qDebug() << "number of texture coordinates" << polyData->GetPointData()->GetTCoords()->GetNumberOfTuples();
//      }
//  }

  int i=0;// temporally one object

  if (parsePLY(filename, materials))
  {
    std::string texturefn = materials[i].textureFilename;

    materials[i].fileType = PLYFILE;

    QString path =  fi.dir().path() + "/";
    //qDebug() << "path:" << path;
    QString texturefilename;
    texturefilename = path + texturefn.c_str();
    //qDebug() << "texturefilename:" << texturefilename;
    QFileInfo texturefi(texturefilename);

    if (texturefi.exists())
    {
      readATexture(texturefilename, channelnames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    } else {
      readAnyTexture(filename, channelnames, wavelengths, texturefilename, rgbTexture, hyperImageData, isTextureOn);
    }
    // DT: fixing texture issue
    if(isTextureOn)
    {
        texturefilenameout = texturefilename;
    }
  } else {
    isTextureOn = false;
    Material *currentMaterial = createMaterial();
    materials.push_back(*currentMaterial);
    materials[i].fileType = PLYFILE;
  }
//  #define _DEBUG // here is OK
//  #ifdef _DEBUG
//    vtkSmartPointer<vtkImageData> output = vtkImageData::SafeDownCast(rgbTexture->GetInput());
//    mkDebug md;  md.qDebugImageData(output); // fine
//  //imagedata1:  512   512   1 :  3
//  //imagedata1 (10,10,0,1):  19
//  #endif

  return true;
} // end of class


bool ReadCHEROb::readAnyTexture(QString filename, std::vector<std::string> &channelNames, std::vector<float> &wavelengths, QString &filenameout, vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn)
{
  // create texture filename candidates
  QFileInfo fi(filename);
  // read texture
  QString path =  fi.dir().path() + "/";
  QString filenamebase =  fi.baseName();
  QString jpgext = ".jpg";
  QString texturefile_jpg = path + filenamebase + jpgext;
  QFileInfo fijpg(texturefile_jpg);

  QString jpegext = ".jpeg";
  QString texturefile_jpeg = path + filenamebase + jpegext;
  QFileInfo fijpeg(texturefile_jpeg);

  QString tifext = ".tif";
  QString texturefile_tif = path + filenamebase + tifext;
  QFileInfo fitif(texturefile_tif);

  QString tiffext = ".tiff";
  QString texturefile_tiff = path + filenamebase + tiffext;
  QFileInfo fitiff(texturefile_tiff);

  QString pngext = ".png";
  QString texturefile_png = path + filenamebase + pngext;
  QFileInfo fipng(texturefile_png);

  QString bmpext = ".bmp";
  QString texturefile_bmp = path + filenamebase + bmpext;
  QFileInfo fibmp(texturefile_bmp);

  QString exrext = ".exr";
  QString texturefile_exr = path + filenamebase + exrext;
  QFileInfo fiexr(texturefile_exr);

  if ( fijpg.exists() )
  {
    readATexture(texturefile_jpg, channelNames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    filenameout = texturefile_jpg;
    return true;
  }
  else if ( fijpeg.exists() ) {
    readATexture(texturefile_jpeg, channelNames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    filenameout = texturefile_jpeg;
    return true;
  }
  else if ( fitif.exists() ) {
    readATexture(texturefile_tif, channelNames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    filenameout = texturefile_tif;
    return true;
  }
  else if ( fitiff.exists() ) {
    readATexture(texturefile_tiff, channelNames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    filenameout = texturefile_tiff;
    return true;
  }
  else if ( fipng.exists() ) {
    readATexture(texturefile_png, channelNames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    filenameout = texturefile_png;
    return true;
  }
  else if ( fibmp.exists() ) {
    readATexture(texturefile_bmp, channelNames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    filenameout = texturefile_bmp;
    return true;
  }
  else if ( fiexr.exists() ) {
    readATexture(texturefile_exr, channelNames, wavelengths, rgbTexture, hyperImageData, isTextureOn);
    filenameout = texturefile_exr;
    return true;
  }
  isTextureOn = false;
  return false;
}

bool ReadCHEROb::readATexture(QString texturefilename, std::vector<std::string> &channelNames, std::vector<float> &wavelengths, vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn)
{
  QFileInfo fit(texturefilename);
  if (texturefilename.isNull() || !fit.isFile())
  {
    isTextureOn = false;
    return false;
  }

  // parse texture filename
  QString exttexture = fit.suffix().toLower();
  vtkImageData * imageData;

  //qDebug() << "texture file: " << texturefilename;

  if (exttexture==tr("jpg") || exttexture==tr("jpeg")) {
    if (!readJPG(texturefilename, rgbTexture)) {isTextureOn = false; return false;}
    hyperImageData = vtkImageData::New();
  }
  else if (exttexture==tr("tif") || exttexture==tr("tiff")) {
    if (!readTIF(texturefilename, rgbTexture)) {isTextureOn = false; return false;}
    hyperImageData = vtkImageData::New();
  }
  else if (exttexture==tr("bmp")) {
    if (!readBMP(texturefilename, rgbTexture)) {isTextureOn = false; return false;}
    hyperImageData = vtkImageData::New();
  }
  else if (exttexture==tr("png")) {
    if (!readPNG(texturefilename, rgbTexture)) {isTextureOn = false; return false;}
    hyperImageData = vtkImageData::New();
  }
  else if (exttexture==tr("exr")) { // for hyperspectral texture
    if (!readEXR(texturefilename, channelNames, wavelengths, rgbTexture, imageData, hyperImageData)) {isTextureOn = false; return false;}
  }
  //================================================================================
//  #define _DEBUG // here is OK
//  #ifdef _DEBUG
//    vtkSmartPointer<vtkImageData> output = vtkImageData::SafeDownCast(rgbTexture->GetInput());
//    mkDebug md;  md.qDebugImageData(output); // fine
//  #endif
  isTextureOn = true;
  return true;
}

//==================================================================================
// STL Index Stuff
typedef std::vector<float>::const_iterator myiter;

struct ordering {
    bool operator ()(std::pair<size_t, myiter> const& a, std::pair<size_t, myiter> const& b) {
        return *(a.second) < *(b.second);
    }
};
//==================================================================================

bool ReadCHEROb::readEXR(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, vtkTexture *(&rgbTexture), vtkImageData* (&rgbImageData), vtkImageData *(&hyperImageData))
{
  // parse filename
  QFileInfo fi(filename);

  QString extension = fi.suffix().toLower();
  if (extension!=tr("exr")) {
//    qDebug() << "the exr file was not found";
    return false;
  }

  std::string fnstr = filename.toLocal8Bit().constData(); // QString -> Std. String
//  const char *filenamesc = fnstr.c_str();

  //MK: the original vtk file only supports reading vertex and indices.
  //MK: this vtkPLYReader2 reads texture coordinates additionally.
  vtkSmartPointer<vtkOpenEXR> reader = vtkSmartPointer<vtkOpenEXR>::New();
  reader->SetFileName(fnstr.c_str());
  reader->Update();

  channelnames = reader->GetChannelNames();
  int chn = channelnames.size();

  int *dims = reader->mDims;
  if ((dims[0] == 0) && (dims[1] == 0))
      return false;

  vtkSmartPointer<vtkImageData> hyperImageDataTemp = vtkSmartPointer<vtkImageData>::New();
  CreateHyperImage(hyperImageDataTemp, dims[0], dims[1], dims[3]);  //NB the last one is [3]
  hyperImageDataTemp->ShallowCopy(reader->mImageData);
  hyperImageDataTemp->Update();

//  mkDebug md; md.qDebugImageData(hyperImageDataTemp); // fine

  //mkDebug md;  md.qDebugImageData(reader->mImageData, hyperImageDataTemp);
  // pulling out wavelength number
  std::vector<float> lwavelengths;
  bool iGotNumberAll = true;
  for (unsigned int k = 0; k < channelnames.size(); k++ )
  {
    if (strcmp(channelnames[k].c_str(), "B") != 0 &&
        strcmp(channelnames[k].c_str(), "G") != 0 &&
        strcmp(channelnames[k].c_str(), "R") != 0 &&
        strcmp(channelnames[k].c_str(), "Y") != 0 &&
        strcmp(channelnames[k].c_str(), "C") != 0 &&
        strcmp(channelnames[k].c_str(), "A") != 0 ) {
      // pulling wavelengths only from the channel
      QString mystring(channelnames[k].c_str());
      char text[MAX_LINE]; memset( text, 0, sizeof(text) );
      int j=0;
      bool iGotNumber = false;
      for (int i=0; i<mystring.size(); i++)
      {
        if (mystring.at(i).isNumber() == true){
          text[j++] = mystring.at(i).toAscii();
          iGotNumber = true;

        } else {
          if (iGotNumber)
            break;
        }
      }
      iGotNumberAll = iGotNumberAll * iGotNumber;
      lwavelengths.push_back( atof(text) );
      //qDebug() << atof(text);
    }// end of if
  }

  // create index pairs
  std::vector<std::pair<size_t, myiter> > order(lwavelengths.size());
  size_t n = 0;
  for (myiter it = lwavelengths.begin(); it != lwavelengths.end(); ++it, ++n)
  {
      order[n] = std::make_pair(n, it);
  }

  // sort index pairs according to wavelength
  std::sort(order.begin(), order.end(), ordering());

  rgbImageData = vtkImageData::New();
//  if (chn == 1)
//    CreateRGBImage(rgbImageData, dims[0], dims[1], 1);
//  else
    CreateRGBImage(rgbImageData, dims[0], dims[1], 3);

//  QString fileName("@executable_path/../Resources/CHER-Ob_Logo128.png");
//  vtkSmartPointer<vtkPNGReader> pngreader = vtkSmartPointer<vtkPNGReader>::New();
//  pngreader->SetFileName(fileName.toLocal8Bit().constData());
//  pngreader->Update();
//  rgbImageData->CopyStructure(pngreader->GetOutput());

  // update wavelengths

  if(order.size() > 0)
  {
    wavelengths.clear();
    for (unsigned int k=0; k< order.size(); k++)
    {
      float index = *(order[k].second);
      wavelengths.push_back(index);
//      qDebug() << "index: " << index;
    }
  } else {
      // Note that these values only for plotting (no HyperImageData)
      // predefined cameras
        if (channelnames.size() <= 1) // for RGB image
        {
            wavelengths.clear();
            wavelengths.push_back(545); //g
        }
      else if (channelnames.size() <= 4) // for RGB image
      {
            wavelengths.clear();
//          wavelengths.push_back(380); //UV
          wavelengths.push_back(480); //b
          wavelengths.push_back(545); //g
          wavelengths.push_back(615); //r
//          wavelengths.push_back(750); //IR
      }
    else if (channelnames.size() < 12)
    {
        wavelengths.clear();
  //    wavelengths.push_back(350); // start
      wavelengths.push_back(370);
      wavelengths.push_back(417);
      wavelengths.push_back(475);
      wavelengths.push_back(525);
      wavelengths.push_back(585);
      wavelengths.push_back(628);
      wavelengths.push_back(675);
      wavelengths.push_back(755);
  //    wavelengths.push_back(880); // end
    }
    else if (channelnames.size() < 57)
    {
        wavelengths.clear();
  //    wavelengths.push_back(350); // start
      wavelengths.push_back(359);
      wavelengths.push_back(364);
      wavelengths.push_back(369);
      wavelengths.push_back(375);
      wavelengths.push_back(382);
      wavelengths.push_back(389);
      wavelengths.push_back(397);
      wavelengths.push_back(405);
      wavelengths.push_back(415);
      wavelengths.push_back(425);
      wavelengths.push_back(437);
      wavelengths.push_back(450);
      wavelengths.push_back(464);
      wavelengths.push_back(480);
      wavelengths.push_back(497);
      wavelengths.push_back(516);
      wavelengths.push_back(523);
      wavelengths.push_back(530);
      wavelengths.push_back(537);
      wavelengths.push_back(544);
      wavelengths.push_back(552);
      wavelengths.push_back(560);
      wavelengths.push_back(568);
      wavelengths.push_back(577);
      wavelengths.push_back(586);
      wavelengths.push_back(595);
      wavelengths.push_back(604);
      wavelengths.push_back(614);
      wavelengths.push_back(624);
      wavelengths.push_back(635);
      wavelengths.push_back(645);
      wavelengths.push_back(657);
      wavelengths.push_back(668);
      wavelengths.push_back(680);
      wavelengths.push_back(692);
      wavelengths.push_back(705);
      wavelengths.push_back(718);
      wavelengths.push_back(732);
      wavelengths.push_back(746);
      wavelengths.push_back(761);
      wavelengths.push_back(776);
      wavelengths.push_back(791);
      wavelengths.push_back(807);
      wavelengths.push_back(824);
      wavelengths.push_back(841);
      wavelengths.push_back(859);
      wavelengths.push_back(878);
      wavelengths.push_back(897);
      wavelengths.push_back(916);
      wavelengths.push_back(937);
      wavelengths.push_back(958);
      wavelengths.push_back(980);
      wavelengths.push_back(1002); // Ch.4
  //    wavelengths.push_back(1010); // end
    }
  }


//  qDebug() << "wavelength size: " << wavelengths.size();
  int rOffset = 0;
  int gOffset = 0;
  int bOffset = 0;
  int yOffset = 0;
  int cOffset = 0;
  int aOffset = 0;

  // decomposition of existing RGB texture data and sorting hyperImageData according to wavelength.
//  float *phyperImageDataTemp = static_cast<float*>(hyperImageDataTemp->GetScalarPointer());
//  float *pRgbImageData = static_cast<float*>(rgbImageData->GetScalarPointer());
  // armardilo data cube
  fcube cdata;
  hyperImageData = vtkImageData::New();
  if(order.size() > 0){
      cdata.resize(dims[0], dims[1], order.size());
      CreateHyperImage(hyperImageData, dims[0], dims[1], order.size());
  }

      int kk = 0 ;
      for (int k=0; k<  chn; k++)
      {
        if (strcmp(channelnames[k].c_str(), "B") == 0)
        {
         int i = 0, j = 0;
         omp_set_num_threads(omp_get_num_procs());
         #pragma omp parallel for private(i,j)
          for (i = 0; i < dims[0]; i++) {
            for (j = 0; j < dims[1]; j++) {
              float value = hyperImageDataTemp->GetScalarComponentAsFloat( i, j, 0, k);
              value = CLAMP8(( pow(value , (1.f/ GAMMA )) ) * 255.f);
              rgbImageData->SetScalarComponentFromFloat(i,j,0,2, value);//B
              // Fast trick by using pointer
    //          float value = phyperImageDataTemp[chn * (j * dims[0] + i) + k];
    //          value = ( pow(value , (1.f/ GAMMA )) ) * 255.f;
    //          pRgbImageData[3 * (j * dims[0] + i) + 2] = value;
             }
            }
          ++bOffset;
        }
        else if (strcmp(channelnames[k].c_str(), "G") == 0)
        {
            int i = 0, j = 0;
            omp_set_num_threads(omp_get_num_procs());
            #pragma omp parallel for private(i,j)
             for (i = 0; i < dims[0]; i++) {
               for (j = 0; j < dims[1]; j++) {
              float value = hyperImageDataTemp->GetScalarComponentAsFloat( i, j, 0, k);
              value = CLAMP8(( pow(value , (1.f/ GAMMA )) ) * 255.f);
              rgbImageData->SetScalarComponentFromFloat(i,j,0,1, value);//G
           }
          }
          ++gOffset;
        }
        else if (strcmp(channelnames[k].c_str(), "A") == 0)
        {
            int i = 0, j = 0;
            omp_set_num_threads(omp_get_num_procs());
            #pragma omp parallel for private(i,j)
             for (i = 0; i < dims[0]; i++) {
               for (j = 0; j < dims[1]; j++) {
              float value = hyperImageDataTemp->GetScalarComponentAsFloat( i, j, 0, k);
              value = CLAMP8(( pow(value , (1.f/ GAMMA )) ) * 255.f);
              rgbImageData->SetScalarComponentFromFloat(i,j,0,3, value);//A // trans
           }
          }
          ++aOffset;
        }
        else if (strcmp(channelnames[k].c_str(), "Y") == 0)
        {
            int i = 0, j = 0;
            omp_set_num_threads(omp_get_num_procs());
            #pragma omp parallel for private(i,j)
             for (i = 0; i < dims[0]; i++) {
               for (j = 0; j < dims[1]; j++) {
              float value = hyperImageDataTemp->GetScalarComponentAsFloat( i, j, 0, k);
              value = CLAMP8(( pow(value , (1.f/ GAMMA )) ) * 255.f);
              rgbImageData->SetScalarComponentFromFloat(i,j,0,0, value);//Y // one gray
              rgbImageData->SetScalarComponentFromFloat(i,j,0,1, value);//Y // one gray
              rgbImageData->SetScalarComponentFromFloat(i,j,0,2, value);//Y // one gray
           }
          }
          ++yOffset;
        }
        else if (strcmp(channelnames[k].c_str(), "C") == 0)
        {
            int i = 0, j = 0;
            omp_set_num_threads(omp_get_num_procs());
            #pragma omp parallel for private(i,j)
             for (i = 0; i < dims[0]; i++) {
               for (j = 0; j < dims[1]; j++) {
              float value = hyperImageDataTemp->GetScalarComponentAsFloat( i, j, 0, k);
              value = CLAMP8(( pow(value , (1.f/ GAMMA )) ) * 255.f);
              rgbImageData->SetScalarComponentFromFloat(i,j,0,1, value);//Y // one gray
           }
          }
          ++cOffset;
        }
        else if (strcmp(channelnames[k].c_str(), "R") == 0)
        {
            int i = 0, j = 0;
            omp_set_num_threads(omp_get_num_procs());
            #pragma omp parallel for private(i,j)
             for (i = 0; i < dims[0]; i++) {
               for (j = 0; j < dims[1]; j++) {
              float value = hyperImageDataTemp->GetScalarComponentAsFloat( i, j, 0, k);
              value = CLAMP8(( pow(value , (1.f/ GAMMA )) ) * 255.f);
              rgbImageData->SetScalarComponentFromFloat(i,j,0,0, value);//R
           }
          }
          ++rOffset;
        }
        else { // hyperspectral channels
            int i = 0, j = 0;
            omp_set_num_threads(omp_get_num_procs());
            #pragma omp parallel for private(i,j)
             for (i = 0; i < dims[0]; i++) {
               for (j = 0; j < dims[1]; j++) {
                int index = order[kk].first;
                float value = hyperImageDataTemp->GetScalarComponentAsFloat( i, j, 0, index+rOffset+gOffset+bOffset+aOffset+cOffset); // bugfixed
                cdata.at(i, j, kk) = value;//coping the number to armardilo
                hyperImageData->SetScalarComponentFromFloat(i,j,0, kk, value);
                 }
              }
    //        qDebug() << "kk, index: " << kk << " " << order[kk].first;
    //        qDebug() << "kk, second: " << kk << " " << *(order[kk].second);
            ++kk;
        }
    }

  hyperImageData->AllocateScalars();
  hyperImageData->Update();

//  mkDebug md;  md.qDebugImageData(hyperImageData);

  // this is only in case there is no reference white
  // if there is no RGB channels add color conversion.
  if ( ( rOffset==0 || gOffset==0 || bOffset==0 ) &&  yOffset == 0)// if any RGB channels are missed
  {
     MKCC mkCC;
     fcube sRGB; // output
     fmat refwhite;
     fcube wcdata; // white balanced cdata
     int islinear = FALSE;
     int isstretch = TRUE;

     // copying wavelength
     fvec wvls(order.size() ); // wavelength
     for (unsigned int k=0; k< order.size(); k++)
     {
       wvls.at(k) =  *(order[k].second);
     }

     mkCC.Rad2sRGB(wvls, cdata, sRGB, refwhite, islinear, isstretch);
     mkCC.Rad2Ref(cdata, refwhite, wcdata);

     // copying the data from armadillo (RGB)
     int i=0, j=0;
     unsigned int k=0;
     omp_set_num_threads(omp_get_num_procs());
     #pragma omp parallel for private(i,j,k)
     for (i = 0; i < dims[0]; i++) {
       for (j = 0; j < dims[1]; j++) {
        for (k=0; k < 3; k++) {
           float value = sRGB.at(i, j, k)*255;
           rgbImageData->SetScalarComponentFromFloat(i,j,0, k, value);
        }
       }
     }
  }

  //  cout << "sorting was done";
    rgbImageData->AllocateScalars();
    rgbImageData->Update();

    rgbTexture = vtkTexture::New();
    rgbTexture->SetInputConnection(rgbImageData->GetProducerPort());
    //  rgbTexture->SetInput(rgbImageData);
    //  rgbTexture->Update(); //vtkOpenGLTexture (0x10a3710c0): Definition of Execute() method should be in subclass and you should really use the ExecuteData(vtkInformation *request,...) signature instead

    return true;
//if (iGotNumberAll)
//  return true;
//else
//  return false;
}



bool ReadCHEROb::readPNG(QString filename, vtkTexture *(&texture) )
{
  QFileInfo fi(filename);
  if(!fi.exists())
  {
    cout << "the file was not found" ;
    return false;
  }

  // read file
  vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
  reader->SetFileName(filename.toLocal8Bit().constData());
  reader->Update();

  texture = vtkTexture::New();
  texture->SetInputConnection(reader->GetOutputPort());

  return true;
}

bool ReadCHEROb::readBMP(QString filename, vtkTexture *(&texture) )
{
  QFileInfo fi(filename);
  if(!fi.exists())
  {
    cout << "the file was not found" ;
    return false;
  }

  // read file
  vtkSmartPointer<vtkBMPReader> reader = vtkSmartPointer<vtkBMPReader>::New();
  reader->SetFileName(filename.toLocal8Bit().constData());
  reader->Update();

  texture = vtkTexture::New();
  texture->SetInputConnection(reader->GetOutputPort());

  return true;
}

bool ReadCHEROb::readTIF(QString filename, vtkTexture *(&texture) )
{
  QFileInfo fi(filename);
  if(!fi.exists())
  {
    cout << "the file was not found" ;
    return false;
  }

  // read file
  vtkSmartPointer<vtkTIFFReader> reader = vtkSmartPointer<vtkTIFFReader>::New();
  reader->SetFileName(filename.toLocal8Bit().constData());
  reader->Update();

  texture = vtkTexture::New();
  texture->SetInputConnection(reader->GetOutputPort());
  return true;
}


bool ReadCHEROb::readJPG(QString filename, vtkTexture *(&texture) )
{
  QFileInfo fi(filename);
  if(!fi.exists())
  {
    cout << "the file was not found" ;
    return false;
  }

  // read file
  vtkSmartPointer<vtkJPEGReader> reader = vtkSmartPointer<vtkJPEGReader>::New();
  reader->SetFileName(filename.toLocal8Bit().constData());
  reader->Update();

  texture = vtkTexture::New();
  texture->SetInputConnection(reader->GetOutputPort());

//#define _DEBUG // here is OK
//#ifdef _DEBUG
////  vtkSmartPointer<vtkImageData> output1 = vtkImageData::SafeDownCast(jpgReader->GetOutput());
////  mkDebug md;  md.qDebugImageData(output1); // fine
//  vtkSmartPointer<vtkImageData> output = vtkImageData::SafeDownCast(texture->GetInput());
//  mkDebug md;  md.qDebugImageData(output); // fine
////imagedata1:  512   512   1 :  3
////imagedata1 (10,10,0,1):  19
//#endif

  return true;
}

void ReadCHEROb::CreateRGBImage(vtkImageData* image, int width, int height, int channels = 3)
{
  image->SetDimensions(width, height, 1);

  image->SetNumberOfScalarComponents(channels);
  image->SetScalarTypeToUnsignedChar();
  image->AllocateScalars();

  image->Modified();
}


void ReadCHEROb::CreateHyperImage(vtkImageData* image, int width, int height, int channels)
{
  image->SetDimensions(width, height, 1);

  image->SetNumberOfScalarComponents(channels);
  image->SetScalarTypeToFloat();
  image->AllocateScalars();

  image->Modified();
}

void ReadCHEROb::updatePolyData(vtkPolyData *(&polyData), vtkDataArray *(&tCoords), vtkDataArray *(&normals))
{
  //================================================================================
  // currently this code supports only one object per file
  // need to be revised later  => This should be from the reading of texture coordinate
  // currently it works with OBJ files only.

//  tCoords = polyData->GetPointData()->GetTCoords();
//  // update texture coordinates and normals in polydata structure
//  updatePolyData(polyData, tCoords, normals);

  // update texture coordinate in the polyData structure
  polyData->GetPointData()->SetTCoords( tCoords );
  polyData->GetPointData()->Update();
}

bool ReadCHEROb::parseOBJ(QString objfilename, QString & mtlfilename)
{
  FILE *in = fopen( objfilename.toLocal8Bit().constData(), "rb" );

  if (in == NULL)
  {
    cout << "file is not found";
    return false;
  }

  // work through the mtl file line by line
  char rawLine[MAX_LINE]; memset( rawLine, 0, sizeof(rawLine) );
  char mtlname[MAX_LINE]; memset( mtlname, 0, sizeof(mtlname) );

  int lineNr = 0;

  while (fgets(rawLine, MAX_LINE, in) != NULL) // read till the end of file
  {
    lineNr++;
    char *pLine = rawLine;
    char *pEnd = rawLine + strlen(rawLine);

    // find the first non-whitespace character
    while (isspace(*pLine) && pLine < pEnd) { pLine++; }

    // this first non-whitespace is the command
    const char *cmd = pLine;

    // skip over non-whitespace
    while (!isspace(*pLine) && pLine < pEnd) { pLine++; }

    // terminate command
    if (pLine < pEnd)
    {
      *pLine = '\0';
      pLine++;
    }

    //qDebug() << cmd;
    // in the MTL format the first characters determine how to interprete the line
    if (cmd[0] == '#' || cmd[0] == '\0') // Note that this is a single quote
    {
      // this is just comment; read and throw this away
      pLine++;
    }
    else if (strcasecmp(cmd, "mtllib") == 0)
    {
      // this is a material id
      if (sscanf (pLine, "%s", mtlname) == 1)
      {
        mtlfilename = mtlname;
      }
      return true;
    }
    else {
      // this is just comment; read and throw this away
      pLine++;
    }
  } // end of while
  fclose(in);
  return false;
}// end of class

Material * ReadCHEROb::createMaterial() {
  Material *material = new Material;

  material->fileType = EMPTYFILE;
  material->numVertex = 0;
  material->numFace = 0;
  material->index = 0;
  memset( material->materialName, 0, sizeof(char) * MAX_LINE );
  material->Ka[0] = material->Ka[1] = material->Ka[2] = DEFAULTKA;
  material->Kd[0] = material->Kd[1] = material->Kd[2] = DEFAULTKD;
  material->Ks[0] = material->Ks[1] = material->Ks[2] = DEFAULTKS;

  material->d = DEFAULTD;
  material->Tr = DEFAULTTR;

  material->illum = DEFAULTILLUM;
  material->Ns = DEFAULTNS;

  memset( material->textureFilename, 0, sizeof(char) * MAX_LINE );
  return material;
}

bool ReadCHEROb::parseMTL(QString mtlfilename, std::vector<Material> &materials)
{
  FILE *in = fopen( mtlfilename.toLocal8Bit().constData(), "rb" );

  if (in == NULL)
  {
    cout << "file is not found";
    return false;
  }

  Material *currentMaterial = createMaterial();
  // work through the mtl file line by line
  char rawLine[MAX_LINE]; memset( rawLine, 0, sizeof(rawLine) );


  int lineNr = 0;
  int index = -1;
  bool readsomething = false;

  while (fgets(rawLine, MAX_LINE, in) != NULL) // read till the end of file
  {
    readsomething = true;
    lineNr++;
    char *pLine = rawLine;
    char *pEnd = rawLine + strlen(rawLine);


    // find the first non-whitespace character
    while (isspace(*pLine) && pLine < pEnd) { pLine++; }

    // this first non-whitespace is the command
    const char *cmd = pLine;

    // skip over non-whitespace
    while (!isspace(*pLine) && pLine < pEnd) { pLine++; }

    // terminate command
    if (pLine < pEnd)
    {
      *pLine = '\0';
      pLine++;
    }

    // in the MTL format the first characters determine how to interprete the line
    if (cmd[0] == '#' || cmd[0] == '\0') // NB these are single quotes
    {
      // this is just comment; read and throw this away
      pLine++;
    }
    else if (strcasecmp(cmd, "newmtl") == 0)
    {
      char objname[MAX_LINE]; memset( objname, 0, sizeof(objname) );
      // this is a material id
      if (sscanf (pLine, "%s", objname) == 1)
      {
        currentMaterial->index = (unsigned int) ++index;
        strcpy(currentMaterial->materialName, objname);
      }
    }
    else if (strcasecmp(cmd, "Ka") == 0) // ambient
    {
      float r = DEFAULTKA;
      float g = DEFAULTKA;
      float b = DEFAULTKA;
      if (sscanf (pLine, "%f %f %f", &r, &g, &b ) == 3)
      {
        currentMaterial->Ka[0] = r;
        currentMaterial->Ka[1] = g;
        currentMaterial->Ka[2] = b;
      }
    }
    else if (strcasecmp(cmd, "Kd") == 0) // diffuse
    {
      float r = DEFAULTKD;
      float g = DEFAULTKD;
      float b = DEFAULTKD;
      if (sscanf (pLine, "%f %f %f", &r, &g, &b ) == 3)
      {
        currentMaterial->Kd[0] = r;
        currentMaterial->Kd[1] = g;
        currentMaterial->Kd[2] = b;
      }
    }
    else if (strcasecmp(cmd, "Ks") == 0) // specular
    {
      float r = DEFAULTKS;
      float g = DEFAULTKS;
      float b = DEFAULTKS;
      if (sscanf (pLine, "%f %f %f", &r, &g, &b ) == 3)
      {
        currentMaterial->Ks[0] = r;
        currentMaterial->Ks[1] = g;
        currentMaterial->Ks[2] = b;
      }
    }
    else if (strcasecmp(cmd, "Tr") == 0) // alpha
    {
      float Tr = DEFAULTTR;
      if (sscanf (pLine, "%f", &Tr ) == 1)
      {
        currentMaterial->Tr = Tr;
      }
    }
    else if (strcasecmp(cmd, "d") == 0) // alpha
    {
      float d = DEFAULTD;
      if (sscanf (pLine, "%f", &d ) == 1)
      {
        currentMaterial->d = d;
      }
    }
    else if (strcasecmp(cmd, "illum") == 0) // shininess
    {
      float illum = DEFAULTILLUM;
      if (sscanf (pLine, "%f", &illum ) == 1)
      {
        currentMaterial->illum = illum;
      }
    }
    else if (strcasecmp(cmd, "Ns") == 0) // shininess
    {
      float Ns = DEFAULTNS;
      if (sscanf (pLine, "%f", &Ns ) == 1)
      {
        currentMaterial->Ns = Ns;
      }
    }
    else if (strcasecmp(cmd, "map_Kd") == 0 || strcasecmp(cmd, "map_Ka") == 0) // texture filename
    {
      char texturefilename[MAX_LINE]; memset( texturefilename, 0, sizeof(texturefilename) );

      if (sscanf (pLine, "%s", texturefilename ) == 1)
      {
        strcpy(currentMaterial->textureFilename, texturefilename);
        materials.push_back(*currentMaterial);
      }
    }
    else {
      // this is just comment; read and throw this away
      pLine++;
    }
  } // end of while
  fclose(in);

  //qDebug() << "material size: " << materials.size();

  if (!readsomething)
  {
    return false;
  }
  return true;
}// end of class


bool ReadCHEROb::parsePLY(QString filename, std::vector<Material> &materials)
{
  FILE *in = fopen( filename.toLocal8Bit().constData(), "rb" );

  if (in == NULL)
  {
//    qDebug() << "file " << filename.toLocal8Bit().constData() << " not found";
    return false;
  }

  Material *currentMaterial = createMaterial();
  // work through the mtl file line by line
  char rawLine[MAX_LINE]; memset( rawLine, 0, sizeof(rawLine) );


  int lineNr = 0;
  int index = -1;

  while (fgets(rawLine, MAX_LINE, in) != NULL) // read till the end of file
  {
    lineNr++;
    char *pLine = rawLine;
    char *pEnd = rawLine + strlen(rawLine);
    char subcategory[MAX_LINE]; memset( subcategory, 0, sizeof(subcategory) );
    char subproperty[MAX_LINE]; memset( subproperty, 0, sizeof(subproperty) );

    // find the first non-whitespace character
    while (isspace(*pLine) && pLine < pEnd) { pLine++; }

    // this first non-whitespace is the command
    const char *cmd = pLine;

    // skip over non-whitespace
    while (!isspace(*pLine) && pLine < pEnd) { pLine++; }

    // terminate command
    if (pLine < pEnd)
    {
      *pLine = '\0';
      pLine++;
    }

    // in the MTL format the first characters determine how to interprete the line
    if (cmd[0] == '#' || cmd[0] == '\0') // NB these are single quotes
    {
      // this is just comment; read and throw this away
      pLine++;
    }
    else if (strcasecmp(cmd, "ply") == 0)
    {
      // this is a material id
      if (sscanf (pLine, "%s", subcategory) == 1)
      {
        currentMaterial->index = (unsigned int) ++index;
      }
    }
    else if (strcasecmp(cmd, "comment") == 0)
    {
      if (sscanf (pLine, "%s %s", subcategory, subproperty) == 2)
      {
        if (strcasecmp(subcategory, "TextureFile") == 0)
        {
          strcpy(currentMaterial->textureFilename, subproperty);
        }
      }
    }
    else if (strcasecmp(cmd, "element") == 0)
    {
      unsigned int vertex = 0;
      unsigned int face = 0;
      if (sscanf (pLine, "%s %s", subcategory, subproperty) == 2)
      {
        if (strcasecmp(subcategory, "vertex") == 0)
        {
          currentMaterial->numVertex = (unsigned int)atof(subproperty);
        }
        else if (strcasecmp(subcategory, "face") == 0)
        {
          currentMaterial->numFace = (unsigned int)atof(subproperty);
        }
      }
    }
    else if (strcasecmp(cmd, "end_header") == 0) // closing
    {
      materials.push_back(*currentMaterial);
      return true;
    }
    else if (strcasecmp(cmd, "format") == 0)
    {
      // currently spaceholder
      pLine++;
    }
    else if (strcasecmp(cmd, "property") == 0)
    {
      // currently spaceholder
      pLine++;
    }
    else {
      // this is just comment; read and throw this away
      pLine++;
    }
  } // end of while
  fclose(in);
  return false;
}// end of class


int ReadCHEROb::readNextToken(FILE *f, char *s )
{
  int len, res;

  res=fscanf(f,"%255s", s);

  // check if it is a comment
  while (res != EOF && s[0] == '#')
    {
      do    // Reads till end of the line
    {
      res=fgetc(f);
    }
      while (res != EOF && res != '\n');

      res=fscanf(f,"%255s",s);
    }

  // separate "{" and "}" (bad file formating)
  len= (int) strlen(s);
  if (len > 1)
    {
      if (s[len-1] == '{')
    {
      s[len-1]=0;
      ungetc('{', f);
    }
      if (s[len-1] == '}')
    {
      s[len-1]=0;
      ungetc('}', f);
    }
    }

  // separate "[" and "]" (bad file formating)
  if (len > 1)
    {
      if (s[len-1] == '[')
    {
      s[len-1]=0;
      ungetc('[', f);
    }
      if (s[len-1] == ']')
    {
      s[len-1]=0;
      ungetc(']', f);
    }
    }

  if (updateProgress(ftell(f)))
    {
      s[0]=0;
      res=EOF;
    }

  return res;
}

// material appearan is located in shape only
bool ReadCHEROb::readMaterial(FILE *f, Material * material)
{
  if (material==NULL) return false;

  int res, brakesCount;
  char textureName[255], s1[256];

  textureName[0]=0;

  brakesCount=0;
  do
  {
      res = readNextToken(f, s1);

      if (strcmp(s1,"}") == 0)
            brakesCount--;
      if (strcmp(s1,"{") == 0)
            brakesCount++;

      // vertext reading
      if (strcasecmp(s1,"IndexedFaceSet") == 0)
      {
          while (fgetc(f) != '}') ;   // Reads till end of vertext
      }

      if (strcasecmp(s1,"coordIndex") == 0)
      {
          while (fgetc(f) != '}') ;// Reads till end of coordIndex
      }

      if (strcasecmp(s1,"TextureCoordinate") == 0)
      {
          while (fgetc(f) != '}') ;// Reads till end of TextureCoordinate
      }


      if (strcasecmp(s1,"texCoordIndex") == 0)
      {
          while (fgetc(f) != '}') ;// Reads till end of texCoordIndex
      }

    // look for texture
    if (strcasecmp(s1,"ImageTexture") == 0)
    {
      // Find the file name
      while (strcasecmp(s1,"url") != 0)
        res=fscanf(f,"%255s",s1);

      res=fscanf(f,"%255s", s1);

      if (strcasecmp(s1,"[") == 0)
        res=fscanf(f,"%255s", s1);

      // remove heading and following \"
      if (s1[0] == '\"')
        strcpy(textureName, s1+1);
      else
        strcpy(textureName, s1);

      if (textureName[strlen(textureName)-1] == '\"')
        textureName[strlen(textureName)-1]=0;

      strcpy(material->textureFilename, textureName);

      // Reads till end of texture
      while (fgetc(f) != '}') ;
    }
    //-------------------------------
    // material
#ifdef SUPPORT_LATER
    // look for texture
    if (strcasecmp(s1,"Material") == 0)
    {
        char k1[256], k2[256], k3[256];
      float x = 1.f;
      float y = 1.f;
      float z = 1.f;

      int tr = 0;

      //------------------------------------
      tr = 0;
      while ((strcasecmp(k1,"ambientIntensity") != 0) && (strcasecmp(k1,"}") != 0) ) {
          if(tr++ > 50) break;
          res=fscanf(f,"%255s",k1);}

      if (tr<50) {
          res=fscanf(f,"%255s", k1);

          material->Ka[0] = atof(k1);
          material->Ka[1] = atof(k1);
          material->Ka[2] = atof(k1);
      }
      //------------------------------------
      tr=0;
      while ((strcasecmp(k1,"diffuseColor") != 0) && (strcasecmp(k1,"}") != 0) ) {
          if(tr++ > 50) break;
          res=fscanf(f,"%255s",k1);}
      if (tr<50) {
          res=fscanf(f,"%255s %255s %255s",k1, k2, k3);

          material->Kd[0] = atof(k1);
          material->Kd[1] = atof(k2);
          material->Kd[2] = atof(k3);
      }
      //------------------------------------
      tr=0;
      while ((strcasecmp(k1,"specularColor") != 0) && (strcasecmp(k1,"}") != 0) ) {
          if(tr++ > 50) break;
          res=fscanf(f,"%255s",k1);}
      if (tr<50) {
          res=fscanf(f,"%255s %255s %255s",k1, k2, k3);

          material->Ks[0] = atof(k1);
          material->Ks[1] = atof(k2);
          material->Ks[2] = atof(k3);
      }
      //------------------------------------
      tr=0;
      while ((strcasecmp(k1,"shininess") != 0) && (strcasecmp(k1,"}") != 0) ) {
          if(tr++ > 50) break;
          res=fscanf(f,"%255s",k1);}
      if (tr<50) {
        res=fscanf(f,"%255s", k1);
        material->Ns = atof(k1);
      }
      //------------------------------------
/*
      tr=0;
      while ((strcasecmp(k1,"emissiveColor") != 0) && (strcasecmp(k1,"}") != 0) ) {
          if(tr++ > 50) break;
          res=fscanf(f,"%255s",k1);}
      if (tr<50) {
          res=fscanf(f,"%255s %255s %255s",k1, k2, k3);

          material->Ke[0] = atof(k1);
          material->Ke[1] = atof(k2);
          material->Ke[2] = atof(k3);
        }
      //------------------------------------
      tr=0;
      while ((strcasecmp(k1,"transparency") != 0) && (strcasecmp(k1,"}") != 0) ) {
          if(tr++ > 50) break;
          res=fscanf(f,"%255s",k1);}
      if (tr<50) {
          res=fscanf(f,"%255s", k1);
          material->Ts = atof(k1);
      }
      */
      // Reads till end of texture
      while (fgetc(f) != '}') ;
    }
#endif


  }
  while (res != EOF && brakesCount != 0);

  return true;
}


//MK: find out mainwindow
MainWindow * ReadCHEROb::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
  return NULL;
}


int ReadCHEROb::updateProgress(int pos)
{
  static int q=0;

  if (q % 100 == 0)
    mw()->qb->setValue(pos);
  q++;

//  return mw()->qb->wasCanceled();
  return 0;
}

void ReadCHEROb::setTotal(int size)
{
  mw()->qb->setMaximum(size);
}


bool ReadCHEROb::parseWRL2(QString filename, std::vector<Material> &materials)
{
  FILE *in = fopen( filename.toLocal8Bit().constData(), "rb" );

  if (in == NULL)
  {
//    qDebug() << "file " << filename.toLocal8Bit().constData() << " not found";
    return false;
  }
  char s1[256], s2[256], s3[256];
  int res = 0;
  Material *currentMaterial = createMaterial();

  fseek(in, 0, SEEK_END);
  setTotal(ftell(in));
  fseek(in, 0, SEEK_SET);

  if (fscanf(in,"%256s %256s %256s", s1, s2, s3) < 1) // Read header
    {
#ifdef _DEBUG1
      qDebug() << "VRML reading error!";
#endif
      return false;
    }

  while (fgetc(in) != '\n'); // Reads till end of the line

  // check header
  if (strcasecmp(s1,"#VRML") != 0 || strcasecmp(s2,"V2.0") != 0)
    {
#ifdef _DEBUG1
      qDebug() << "VRML2 header format error";
#endif

      return false;
    }

    bool noerror = true;
    // actual reading
    do
    {
      // look for a starting point
      do
      {
        res = readNextToken(in, s1); // read
      }
      while (res != EOF && strcasecmp(s1,"Shape") != 0 && strcasecmp(s1,"Transform") != 0);

      if (strcasecmp(s1,"Shape") == 0)
      {
          if (readMaterial(in, currentMaterial))
          {
#ifdef _DEBUG1
              qDebug() << "textureFilename: " << currentMaterial->textureFilename;
              qDebug() << "Kd: " << currentMaterial->Kd[0] << " "  << currentMaterial->Kd[1] << " " << currentMaterial->Kd[2] ;
              qDebug() << "Ka: " << currentMaterial->Ka[0] << " "  << currentMaterial->Ka[1] << " " << currentMaterial->Ka[2] ;
              qDebug() << "Ks: " << currentMaterial->Ks[0] << " "  << currentMaterial->Ks[1] << " " << currentMaterial->Ks[2] ;
#endif
          }
      }
#ifdef _DEBUG1
      if (strcasecmp(s1,"Transform") == 0)
          qDebug() << "currently skip";
#endif
    }
    while (res != EOF);

    fclose(in);

    currentMaterial->fileType = WRLFILE;
    materials.push_back(*currentMaterial);

    mw()->qb->setValue(0);

    return noerror;
}// end of class

