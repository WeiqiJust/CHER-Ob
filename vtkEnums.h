/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

 - Writers:  David Tidmarsh (tidmarsh@aya.yale.edu)

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
   anonymous help from various software engineering communities.

   This file defines the different classes of bookmark that the user may work
   with (corresponding to the different file types that may be displayed).

*****************************************************************************/

#ifndef VTKENUMS_H
#define VTKENUMS_H

enum USERMODE{CREATOR=0, MODIFIER, VIEWER};
enum CTOrientation{FRONTCT=0,TOPCT,SIDECT};
enum CTVisualization{STACK=0,VOLUMEGPU};
enum CTVolumeRenderMode{AUTOMATIC=0,DEFAULTGPU,CPURAYCASTTEXTURE,CPURAYCASTONLY};
enum RenderMode3D{SURFACE3D=0, WIREFRAME3D, POINTS3D};
enum OrthogonalView3D{FRONT3D=0, LEFT3D, RIGHT3D, TOP3D, BOTTOM3D, BACK3D};
enum WidgetMode{EMPTYWIDGET=0, IMAGE2D, MODEL3D, CTSTACK, CTVOLUME, RTI2D};
enum CTVolumeRenderingCurve{WOOD=0, MIP, CRAMP, CSRAMP, CT_SKIN, CT_BONE, CT_MUSCLE};
enum Interpolation{INTERPOLATION_OFF=0, INTERPOLATION_ON};
enum Texture{TEXTURE_OFF=0, TEXTURE_ON};
enum CameraMode3D{TRACKBALLMODE=0, SURFACEWALKERMODE, RUBBERBANDPICKMODE};
enum NoteMode{UNDECLARE=0, POINTNOTE, SURFACENOTE, FRUSTUMNOTE};
enum ColorType{RED=0, YELLOW, BLUE, GREEN, PINK, PURPLE};
enum LightControlType{Model3DLIGHTCONTROL=0, RTILIGHTCONTROL}; //YY

double const ColorPixel[6][3] = {
  {1, 0, 0},	// red
  {1, 0.75, 0},	// yellow
  {0, 0, 1},	// blue
  {0, 1, 0},	// green
  {0.73, 0.33, 0.83},	// pink
  {0.3, 0, 0.5}		// purple
};

#endif // VTKENUMS_H
