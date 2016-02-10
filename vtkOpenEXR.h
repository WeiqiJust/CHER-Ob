/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

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

 - Acknowledgments: This EXR file reader is based on the original code of
   exrreadchannel for MATLAB by Edgar Velazquez-Armendariz.

*****************************************************************************/
#ifndef VTKOPENEXR_H
#define VTKOPENEXR_H

#include <vtkPolyDataAlgorithm.h>
#include <vtkImageData.h>
#include <vtkStringArray.h>
#include <vtkSmartPointer.h>
#include <vtkVector.h>

#include <ImfArray.h>
#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfChannelList.h>
#include <ImfPixelType.h>
#include <Iex.h>
#include <ImfInputFile.h>
#include <ImfChannelList.h>


class vtkOpenEXR : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkOpenEXR,vtkPolyDataAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with merging set to true.
  static vtkOpenEXR *New();

  // Description:
  // Specify file name of stereo lithography file.
  vtkSetStringMacro(FileName)
  vtkGetStringMacro(FileName)

  std::vector<std::string> GetChannelNames() { return mChannelNames; }
 // vtkSmartPointer<vtkImageData> GetImageData() {return mImageData; }
  vtkImageData *mImageData;
  int mDims[4];

protected:
  vtkOpenEXR();
  ~vtkOpenEXR();

  // Description:
  static int exrInfo(const char *fileName, std::vector<std::string> &channelNames);

  char *FileName;
  std::vector<std::string> mChannelNames;
  void CreateHyperImage(vtkImageData* image, int width, int height, int channels);
  bool exrReadChannels(const char *fileName, std::vector<std::string> channelNames, vtkImageData * (&imageData));
  inline void convertChannelNames(const Imf::ChannelList & channels, std::vector<std::string> & result);
  void prepareFrameBuffer(Imf::FrameBuffer & fb, const Imath::Box2i & dataWindow, const Imf::ChannelList & channels,
                          const std::vector<std::string> & requestedChannels, std::vector< Imf::Array2D<float> *> & outData);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkOpenEXR(const vtkOpenEXR&);  // Not implemented.
  void operator=(const vtkOpenEXR&);  // Not implemented.
};

#endif // VTKOPENEXR_H
