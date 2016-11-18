/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  David Tidmarsh (tidmarsh@aya.yale.edu)
			 Weiqi Shi (weiqi.shi@yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)

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
enum OPENRESULT{SUCCEED=0, FAILED, EXISTED};
enum CTOrientation{FRONTCT=0, TOPCT, SIDECT};
enum CTVisualization{STACK=0,VOLUMEGPU};
enum CTVolumeRenderMode{AUTOMATIC=0,DEFAULTGPU,CPURAYCASTTEXTURE,CPURAYCASTONLY};
enum RenderMode3D{SURFACE3D=0, WIREFRAME3D, POINTS3D};
enum OrthogonalView3D{FRONT3D=0, LEFT3D, RIGHT3D, TOP3D, BOTTOM3D, BACK3D};
enum WidgetMode{EMPTYWIDGET=0, IMAGE2D, MODEL3D, CTSTACK, CTVOLUME, RTI2D};
enum CTVolumeRenderingCurve{WOOD=0, MIP, CRAMP, CSRAMP, CT_SKIN, CT_BONE, CT_MUSCLE};
enum Interpolation{INTERPOLATION_OFF=0, INTERPOLATION_ON};
enum Texture{TEXTURE_OFF=0, TEXTURE_ON};
enum CameraMode3D{TRACKBALLMODE=0, SURFACEWALKERMODE, RUBBERBANDPICKMODE};
enum NoteType{NOTE2D = 0, NOTE3D, NONE};
enum NoteMode{UNDECLARE=0, POINTNOTE, SURFACENOTE, POLYGONNOTE, FRUSTUMNOTE, ANNOTATION};
enum ColorType{MAROON=0, RED, ORANGE, YELLOW, LIME, GREEN, AQUA, BLUE, PINK, PURPLE, WHITE};
enum LightControlType{Model3DLIGHTCONTROL=0, RTILIGHTCONTROL}; //YY

enum RenderingRTI
{
	DEFAULT,
    DIFFUSE_GAIN,
	SPECULAR_ENHANCEMENT,
	NORMAL_ENHANCEMENT,
	UNSHARP_MASKING_IMG,
	UNSHARP_MASKING_LUM,
	COEFF_ENHANCEMENT,
	DETAIL_ENHANCEMENT,
	DYN_DETAIL_ENHANCEMENT,
    NORMALS
};

double const ColorPixel[11][3] = {
  {0.52, 0.08, 0.29},	// marron
  {1.00, 0.00, 0.00},	// red
  {1.00, 0.52, 0.11},	// orange
  {1.00, 0.75, 0.00},	// yellow
  {0.00, 1.00, 0.44},	// lime
  {0.24, 0.60, 0.25},	// green
  {0.50, 0.86, 1.00},	// aqua
  {0.00, 0.00, 1.00},	// blue 
  {0.73, 0.33, 0.83},	// pink
  {0.30, 0.00, 0.50},	// purple
  {1.00, 1.00, 1.00}	// white
};

static std::string num2category(int mode)
{
	switch (mode)
	{
		case 0: return "Object / Work";
		case 1: return "Physical Dimensions / Measurement";
		case 2: return "Creation";
		case 3: return "Materials and Techniques";
		case 4: return "Stylistic Analysis and Descriptions";
		case 5: return "Condition and Conservation";
		case 6: return "Analyses";
		case 7: return "Related Works";
		case 8: return "Exhibition / Loans and Legal Issues";
		case 9: return "Image/Audio Documentation";
		case 10: return "Others";
		default: return "Others";
	}
}

static std::string num2label(int mode)
{
	switch (mode)
	{
		case 0: return "Object";
		case 1: return "Measurement";
		case 2: return "Creation";
		case 3: return "Material";
		case 4: return "Description";
		case 5: return "Conservation";
		case 6: return "Analyses";
		case 7: return "Related";
		case 8: return "Administration";
		case 9: return "Documentation";
		case 10: return "Others";
		default: return "Others";
	}
}

static int color2type(const std::string str)
{
	if (str == "MAROON")
		return 0;
	else if (str == "RED")
		return 1; 
	else if (str == "ORANGE")
		return 2;
	else if (str == "YELLOW")
		return 3;
	else if (str == "LIME")
		return 4;
	else if (str == "GREEN")
		return 5;
	else if (str == "AQUA")
		return 6;
	else if (str == "BLUE")
		return 7;
	else if (str == "PINK")
		return 8;
	else if (str == "PURPLE")
		return 9;
	else if (str == "WHITE")
		return 10;
	else
		return 10;
}

static std::string colortype2str(const ColorType type)
{
	switch(type)
	{
		case 0:  return "MAROON";
		case 1:  return "RED";
		case 2:  return "ORANGE";
		case 3:  return "YELLOW";
		case 4:  return "LIME";
		case 5:  return "GREEN";
		case 6:  return "AQUA";
		case 7:  return "BLUE";
		case 8:  return "PINK";
		case 9:  return "PURPLE";
		case 10: return "WHITE";
		default: return "WHITE";
	}
}

static ColorType str2colortype(const std::string str)
{
	if (str == "MAROON")
		return MAROON;
	else if (str == "RED")
		return RED; 
	else if (str == "ORANGE")
		return ORANGE;
	else if (str == "YELLOW")
		return YELLOW;
	else if (str == "LIME")
		return LIME;
	else if (str == "GREEN")
		return GREEN;
	else if (str == "AQUA")
		return AQUA;
	else if (str == "BLUE")
		return BLUE;
	else if (str == "PINK")
		return PINK;
	else if (str == "PURPLE")
		return PURPLE;
	else if (str == "WHITE")
		return WHITE;
	else
		return WHITE;
}


#endif // VTKENUMS_H
