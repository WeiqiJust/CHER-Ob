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

 - Acknowlegments: Some portions of this file are based on the example codes
   of ITK/VTK library from Kitware, QT API from Nokia. I would like to thank
   annonymous help by various software engineering communities.

*****************************************************************************/
#ifndef READCHEROb_H
#define READCHEROb_H

#include <QObject>

#include <vtkPolyData.h>
#include <vtkTexture.h>
#include <vtkImageData.h>
#include <vtkStringArray.h>
#include "vtkVRMLSource2.h"

#include <omp.h> // openMP

#define MAX_LINE (256)

class MainWindow;

enum FileType{EMPTYFILE=0,OBJFILE,PLYFILE,WRLFILE};

struct Material
{
  FileType fileType;

  // for ply header parser
  unsigned int numVertex;
  unsigned int numFace;

  // for obj header parser
  unsigned int index; //index of material
  char materialName[MAX_LINE];

  float Ka[3]; // ambient
  float Kd[3]; // diffuse
  float Ks[3]; // specular
  float Ke[3]; // emissive color

  float d;    // alpha
  float Tr;   // alpha

  int illum;  // specular illumination on/off
  float Ns;   // shininess
  float Ts;   // transparency

  char textureFilename[MAX_LINE]; // texture filename (map_Kd in OBJ structure)
};


class ReadCHEROb : QObject
{
  Q_OBJECT

public:
  ReadCHEROb() {}
  ~ReadCHEROb() {}

  bool read3D(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData * (&polyData), vtkTexture * (&rgbTexture), vtkImageData * (&hyperImageData), bool &isTextureOn);

  bool readEXR(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, vtkTexture *(&rgbTexture), vtkImageData* (&rgbImageData), vtkImageData *(&hyperImageData) );

protected:
  // for VRML parser
  int readNextToken(FILE *f, char *s);
  bool readMaterial(FILE *f, Material *material);
  char buffer[256];
  // for progress bar
//  QProgressDialog *pd;
  int updateProgress(int pos);
  void setTotal(int size);
  MainWindow* mw(); // find the mainframe

  Material * createMaterial();
  void CreateRGBImage(vtkImageData* image, int width, int height, int channels);
  void CreateHyperImage(vtkImageData* image, int width, int height, int channels);

  bool readOBJ(QString objfilename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData *(&polyData), vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn);
  bool readPLY(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData *(&polyData), vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn);
  bool readWRL(QString filename, std::vector<std::string> &channelnames, std::vector<float> &wavelengths, QString &texturefilenameout, std::vector<Material> &materials, vtkPolyData *(&polyData), vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn);

  void updatePolyData(vtkPolyData *(&polyData), vtkDataArray *(&tCoords), vtkDataArray *(&normals) );

  bool parseOBJ(QString objfilename, QString &mtlfilename);
  bool parseMTL(QString mtlfilename, std::vector<Material> &materials);
  bool parsePLY(QString filename, std::vector<Material> &materials);
  bool parseWRL2(QString filename, std::vector<Material> &materials);

  bool readJPG(QString filename, vtkTexture *(&texture) );
  bool readTIF(QString filename, vtkTexture *(&texture) );
  bool readPNG(QString filename, vtkTexture *(&texture) );
  bool readBMP(QString filename, vtkTexture *(&texture) );

  bool readAnyTexture(QString filename, std::vector<std::string> &channelNames, std::vector<float> &wavelengths, QString &filenameout, vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn);
  bool readATexture(QString texturefilename, std::vector<std::string> &channelNames, std::vector<float> &wavelengths, vtkTexture *(&rgbTexture), vtkImageData *(&hyperImageData), bool &isTextureOn);

private:  
//  std::vector<std::string> mChannelNames;
};


#endif // READCHEROb_H
