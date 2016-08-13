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
#if defined(_WIN32) || defined(_WIN64)
  #define snprintf _snprintf
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp

  #if _MSC_VER >= 1600
  # define CHAR16_T wchar_t
  #endif
#endif

#include <omp.h>
#include <string>
#include <iostream>
#include <ctype.h>
#include <stddef.h>
#include <string>
#include <cassert>
#include <vector>

#include <QFileInfo>
#include <QString>
#include <QDebug>
#include <QObject>

#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkSortDataArray.h>
#include <vtkVector.h>
#include <vtkTexture.h>
#include "../function/mkTools.hpp"

#include "vtkOpenEXR.h"

#define MAX_LINE (1024)

using namespace Imf;
using namespace Imath;
using namespace Iex;

using std::cout;
using std::endl;
using std::flush;

using Imath::Box2i;

vtkStandardNewMacro(vtkOpenEXR)


// Construct object with merging set to true.
vtkOpenEXR::vtkOpenEXR()
{
  this->FileName = NULL;

  this->SetNumberOfInputPorts(0);
}

vtkOpenEXR::~vtkOpenEXR()
{
  if (this->FileName)
    {
    delete [] this->FileName;
    }
}

void vtkOpenEXR::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
}

// Read the header of an EXR file.
// (ref) http://www.openexr.com/ReadingAndWritingImageFiles.pdf
int vtkOpenEXR::exrInfo(const char *fileName, std::vector<std::string> &channelNames)
{
  // parse filename
  QFileInfo fi(fileName);

  QString extension = fi.suffix().toLower();
  if ( extension.compare("exr") != 0 ) {
//    qDebug() << "the exr file was not found";
    return false;
  }

  if (!fi.exists())
  {
//    qDebug() << "the exr file was not found";
    return false;
  }

  int numChannels = 0;

  try {
    RgbaInputFile file(fileName);

    const ChannelList &ch = file.header().channels();

    int ic = 0;
    for (ChannelList::ConstIterator i = ch.begin(); i != ch.end(); ++i) {
      const Channel &channel = i.channel();
      const char* channelName = i.name(); // by Min
      PixelType type = channel.type;
      const char* t = (type == UINT) ? "uint" :
          ((type == HALF) ? "half" : "float");

      channelNames.push_back(channelName);
      ++ic;
//      qDebug() << "channel " << ic << ": " << t << ": " << channelName ;
    }
    numChannels = ic;
    const StringAttribute *comments =
    file.header().findTypedAttribute <StringAttribute> ("comments");

    const M44fAttribute *cameraTransform =
    file.header().findTypedAttribute <M44fAttribute> ("cameraTransform");

//    if (comments)
//       qDebug() << "comments\n   " << comments->value() ;

//    if (cameraTransform)
//       qDebug() << "cameraTransform\n" << cameraTransform->value() << flush;

  } catch (const std::exception &exc) {
//    qDebug() << exc.what();
    return false;
  }

  return numChannels; // number of channels
}

// Utility to fill the given array with the names of all the channels
inline void vtkOpenEXR::convertChannelNames(const ChannelList & channels, std::vector<std::string> & result)
{
    typedef ChannelList::ConstIterator CIter;

    for (CIter it = channels.begin(); it != channels.end(); ++it) {
        result.push_back(std::string(it.name()));
    }
}

// Prepares a framebuffer for the requested channels, allocating also the
// appropriate Matlab memory
void vtkOpenEXR::prepareFrameBuffer(FrameBuffer & fb, const Box2i & dataWindow, const ChannelList & channels,
    const std::vector<std::string> & requestedChannels, std::vector< Array2D<float> *> & outData) // std::vector<mxArray *> & outMatlabData
{
    assert(!requestedChannels.empty());

    const Box2i & dw = dataWindow;
    const int width  = dw.max.x - dw.min.x + 1;
    const int height = dw.max.y - dw.min.y + 1;

    // The "weird" strides are because Matlab uses column-major order (check this out later)
//    const int xStride = height; // for matlab
//    const int yStride = 1;
    const int xStride = 1;
    const int yStride = width; // for c++

    // Offset for all the slices
    const off_t offset = - (dw.min.x * xStride + dw.min.y * yStride); // why negative?

    for (size_t i = 0; i != requestedChannels.size(); ++i) {
        // Allocate the memory
        //mxArray * data = mxCreateNumericMatrix(height, width, mxSINGLE_CLASS, mxREAL);
        Array2D<float> * data = new Array2D<float>;
        //Array2D<float> * data(height, width);
        data->resizeErase(height, width);

        outData[i] = data;

        float * ptr = static_cast<float*>(data[0][0]);  // check this out

        // Get the appropriate sampling factors
        int xSampling = 1, ySampling = 1;
        ChannelList::ConstIterator cIt = channels.find(requestedChannels[i].c_str());
        if (cIt != channels.end()) {
            xSampling = cIt.channel().xSampling;
            ySampling = cIt.channel().ySampling;
        }

        // Insert the slice in the framebuffer
        fb.insert(requestedChannels[i].c_str(), Slice(FLOAT, (char*)(ptr + offset), sizeof(float) * xStride, sizeof(float) * yStride, xSampling, ySampling));
    }
}


void vtkOpenEXR::CreateHyperImage(vtkImageData* image, int width, int height, int channels)
{
  image->SetDimensions(width, height, 1);

  image->SetNumberOfScalarComponents(channels);
  image->SetScalarTypeToFloat();
  image->AllocateScalars();

  image->Modified();
}

bool vtkOpenEXR::exrReadChannels(const char *fileName, std::vector<std::string> channelNames, vtkImageData * (&imageDataOut))
{
  char text[MAX_LINE]; memset( text, 0, sizeof(text) );
  try {
      InputFile img(this->FileName);

      if (!channelNames.empty()) {
          // Validate that the channels are actually on the file
          const ChannelList & channels = img.header().channels();
          for (size_t i = 0; i != channelNames.size(); ++i) {
	    if (channels.find(channelNames[i].c_str()) == channels.end()) {
                  sprintf( text, "OpenEXR: Channel not in file: %s", channelNames[i].c_str());
//                  qDebug() << text;
              }
          }
      } else {
          // If there are no explicitly required channels, read all
          this->convertChannelNames(img.header().channels(), channelNames);
      }
      assert(!channelNames.empty());

      // Prepare the framebuffer
      const Box2i & dw = img.header().dataWindow();
      const ChannelList & imgChannels = img.header().channels();
      // copy img to imageData
      const int width  = dw.max.x - dw.min.x + 1;
      const int height = dw.max.y - dw.min.y + 1;
      const int chn = channelNames.size();

      mDims[0] = width; mDims[1] = height; mDims[2] = 1; mDims[3] = chn;

      std::vector< Array2D<float> *> outData(channelNames.size());

      FrameBuffer framebuffer;
      this->prepareFrameBuffer(framebuffer, dw, imgChannels, channelNames, outData);

      // Actually read the pixels
      img.setFrameBuffer(framebuffer);
      img.readPixels(dw.min.y, dw.max.y); // reading is done.

//      vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
      vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
      CreateHyperImage(imageData, width, height, chn);

      int i = 0, j = 0, k=0;
      omp_set_num_threads(omp_get_num_procs());
      #pragma omp parallel for private(i,j,k)
      for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
          for (k = 0; k < chn; k++ ) {
            float value = (*outData[k])[height-1-j][i];
            imageData->SetScalarComponentFromFloat(i,j,0, k, value); // x, y, z, component
          }
       }
      }

    imageData->Update();
    imageDataOut = vtkImageData::New();
    imageDataOut->ShallowCopy(imageData);

//      mkDebug md;  md.qDebugImageData(imageDataOut); // fine
//      imagedata1:  626   832   1 :  11
//      imagedata1 (10,10,0,1):  0.0315552

  }
  catch( Iex::BaseExc &e ) {
//      qDebug() << "OpenEXR:exception" << e.what();
      return false;
  }
  return true;
}

int vtkOpenEXR::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
{
  // get the info object
  vtkSmartPointer<vtkInformation> outInfo = outputVector->GetInformationObject(0);
  // get the ouptut
  vtkSmartPointer<vtkImageData> output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

//  mImageData = this->AllocateOutputData(output);

  if (!this->FileName) { return 0; }

  // (1) read channel names from the file
  if(!exrInfo(FileName, mChannelNames))
  {
    return 0;
  }

  // (2) read image exr images
  if (!exrReadChannels(FileName, mChannelNames, mImageData))
  {
    return 0;
  }

  return 1;
}
