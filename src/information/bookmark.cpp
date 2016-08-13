/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  David Tidmarsh (tidmarsh@aya.yale.edu)
 			 Weiqi Shi (weiqi.shi@yale.edu)

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

#include <QDateTime>
#include <QUuid>
#include <QMessageBox>
#include "bookmark.h"
#include "bookmarkWidget.h"


#include "../function/defaultrendering.h"
#include "../function/diffusegain.h"
#include "../function/specularenhanc.h"
#include "../function/normalenhanc.h"
#include "../function/unsharpmasking.h"
#include "../function/coeffenhanc.h"
#include "../function/detailenhanc.h"
#include "../function/dyndetailenhanc.h"
#include "../function/normalsrendering.h"

// Camera perspective and filetype are common to all bookmarks.
Bookmark::Bookmark(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode)
    : doc(d)
{
    double pos[3]; // camera position
    double foc[3]; // focal point
    double angle; // view angle
    double view[3]; // view up direction
    double clip[2]; // clipping range
    double scale; // parallel scale

    camera->GetPosition(pos);
    camera->GetFocalPoint(foc);
    angle = camera->GetViewAngle();
    camera->GetViewUp(view);
    camera->GetClippingRange(clip);
    scale = camera->GetParallelScale();

    bmk = doc.createElement(BOOKMARK_NAME);
    bmk.setAttribute(TITLE, caption);
    bmk.setAttribute(UUID_NAME, uuid);
    QString dt = QDateTime::currentDateTimeUtc().toString();
    bmk.setAttribute(DATE_CREATED, dt);
    bmk.setAttribute(DATE_MODIFIED, dt);
    bmk.setAttribute(DATE_ACCESSED, dt);

    saveCameraPosition(pos);
    saveFocalPoint(foc);
    saveCameraAngle(angle);
    saveViewUp(view);
    saveClippingRange(clip);
    saveParallelScale(scale);
    saveFileType(mode);
}

/*
 * Saves the key/fill lights.
 * Used by BookmarkMODEL3D and BookmarkCTSTACK.
 */
void Bookmark::saveBrightnessContrast(double brt, double cst)
{
    QDomElement brtns = doc.createElement("brightness");
    bmk.appendChild(brtns);
    brtns.appendChild(doc.createTextNode(QString::number(brt)));

    QDomElement ctrst = doc.createElement("contrast");
    bmk.appendChild(ctrst);
    ctrst.appendChild(doc.createTextNode(QString::number(cst)));
}

/*
 * Saves the lighting vector.
 * Used by BookmarkMODEL3D and BookmarkCTVolume.
 */
void Bookmark::saveLightVector(double orientation[3])
{
    QDomElement ort = doc.createElement("light_vector");
    bmk.appendChild(ort);
    QDomElement ort_x = doc.createElement("x");
    QDomElement ort_y = doc.createElement("y");
    QDomElement ort_z = doc.createElement("z");
    ort.appendChild(ort_x);
    ort.appendChild(ort_y);
    ort.appendChild(ort_z);
    ort_x.appendChild(doc.createTextNode(QString::number(orientation[0])));
    ort_y.appendChild(doc.createTextNode(QString::number(orientation[1])));
    ort_z.appendChild(doc.createTextNode(QString::number(orientation[2])));
}

/*
 * Saves interpolation on/off for 2D images (IMAGE2D and CTSTACK).
 */
void Bookmark::save2DInterpolation(bool on)
{
    QDomElement interp = doc.createElement("interpolation_on");
    bmk.appendChild(interp);
    if (on)
        interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_ON)));
    else
        interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_OFF)));
}

// 2D image bookmark preserves only interpolation on/off.
BookmarkIMAGE2D::BookmarkIMAGE2D(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode,
                                 bool interpOn) : Bookmark(caption, uuid, d, camera, mode)
{
    save2DInterpolation(interpOn);
}


BookmarkRTI2D::BookmarkRTI2D(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode, 
							 bool interpOn, vcg::Point3f light, RenderingRTI renderMode, RenderingMode* renderingMode) : Bookmark(caption, uuid, d, camera, mode)
{
	double orientation[3];
	orientation[0] = light[0];
	orientation[1] = light[1];
	orientation[2] = light[2];
	saveLightVector(orientation);
	saveDisplayRTIMode(renderMode);
	saveDisplayRTIPara(renderMode, renderingMode);

	save2DInterpolation(interpOn);
}

void BookmarkRTI2D::saveDisplayRTIPara(RenderingRTI renderMode, RenderingMode* renderingMode)
{
	QDomElement ort = doc.createElement("rti_para");
    bmk.appendChild(ort);

	switch (renderMode)
	{
	case DIFFUSE_GAIN: 
		{
			DiffuseGain* dg = static_cast<DiffuseGain*>(renderingMode);
			QDomElement p = doc.createElement("gain");
			ort.appendChild(p);
			p.appendChild( doc.createTextNode(QString::number(dg->getGain())));
			break;
		}
	
	case SPECULAR_ENHANCEMENT: 
		{
			SpecularEnhancement* se = static_cast<SpecularEnhancement*>(renderingMode);
			QList<QDomElement> p;
			p.append( doc.createElement("diffuseColor") );
			p.append( doc.createElement("specularity") );
			p.append( doc.createElement("highlightSize") );

			for (int i = 0; i < 3; i++) { ort.appendChild(p[i]); }

			p[0].appendChild( doc.createTextNode(QString::number(se->getKd())));
			p[1].appendChild( doc.createTextNode(QString::number(se->getKs())));
			p[2].appendChild( doc.createTextNode(QString::number(se->getExp())));

			break;
		}

	case NORMAL_ENHANCEMENT: 
		{
			NormalEnhancement* ne = static_cast<NormalEnhancement*>(renderingMode);

			QList<QDomElement> p;
			p.append( doc.createElement("gain") );
			p.append( doc.createElement("environment") );

			for (int i = 0; i < 2; i++) { ort.appendChild(p[i]); }

			p[0].appendChild( doc.createTextNode(QString::number(ne->getGain())));
			p[1].appendChild( doc.createTextNode(QString::number(ne->getEnvIll())));

			break;
		}
	
	case UNSHARP_MASKING_IMG:
		{
			UnsharpMasking* um_img = static_cast<UnsharpMasking*>(renderingMode);

			QDomElement p = doc.createElement("gain");
			ort.appendChild(p);
			p.appendChild( doc.createTextNode(QString::number(um_img->getGain())));

			break;
		}

	case COEFF_ENHANCEMENT:
        {
            CoeffEnhancement* ce = static_cast<CoeffEnhancement*>(renderingMode);
            
			QDomElement p = doc.createElement("gain");
			ort.appendChild(p);
			p.appendChild( doc.createTextNode(QString::number(ce->getGain())));

			break;
        }

	case DETAIL_ENHANCEMENT:
        {
            DetailEnhancement* de = static_cast<DetailEnhancement*>(renderingMode);

			QList<QDomElement> p;
			p.append( doc.createElement("localOffset") );
			p.append( doc.createElement("tileSize") );
			p.append( doc.createElement("numberInitialTiles") );

			p.append( doc.createElement("sharpnessOperator") );
			p.append( doc.createElement("lightSampling") );
			p.append( doc.createElement("k1Sharpness") );

			p.append( doc.createElement("k2Lightness") );
			p.append( doc.createElement("threshold") );
			p.append( doc.createElement("smoothingFilter") );
			p.append( doc.createElement("numberIterationSmoothing") );

			for (int i = 0; i < 10; i++) { ort.appendChild(p[i]); }

			p[0].appendChild( doc.createTextNode(QString::number(de->getNOffset())));
			p[1].appendChild( doc.createTextNode(QString::number(de->getMinTileSize())));
			p[2].appendChild( doc.createTextNode(QString::number(de->getMinLevel())));

			p[3].appendChild( doc.createTextNode(QString::number(de->getSharpnessOperator())));
			p[4].appendChild( doc.createTextNode(QString::number(de->getSphereSampling())));
			p[5].appendChild( doc.createTextNode(QString::number(de->getK1())));

			p[6].appendChild( doc.createTextNode(QString::number(de->getK2())));
			p[7].appendChild( doc.createTextNode(QString::number(de->getThreshold())));
			p[8].appendChild( doc.createTextNode(QString::number(de->getFilter())));
			p[9].appendChild( doc.createTextNode(QString::number(de->getNIterSmoothing())));

			break;
        }

	case DYN_DETAIL_ENHANCEMENT:
        {
            DynamicDetailEnh* dde = static_cast<DynamicDetailEnh*>(renderingMode);

			QList<QDomElement> p;
			p.append( doc.createElement("degreeOffset") );
			p.append( doc.createElement("tileSize") );
			p.append( doc.createElement("sharpnessOperator") );
			p.append( doc.createElement("lightSampling") );
			p.append( doc.createElement("k1Sharpness") );
			p.append( doc.createElement("k2Lightness") );
			p.append( doc.createElement("threshold") );
			p.append( doc.createElement("smoothingFilter") );
			p.append( doc.createElement("numberIterationSmoothing") );

			for (int i = 0; i < 9; i++) { ort.appendChild(p[i]); }

			p[0].appendChild( doc.createTextNode(QString::number(dde->getDegreeOffset())));
			p[1].appendChild( doc.createTextNode(QString::number(dde->getTileSize())));
			p[2].appendChild( doc.createTextNode(QString::number(dde->getSharpnessOperator())));
			p[3].appendChild( doc.createTextNode(QString::number(dde->getSphereSampling())));
			p[4].appendChild( doc.createTextNode(QString::number(dde->getK1())));
			p[5].appendChild( doc.createTextNode(QString::number(dde->getK2())));
			p[6].appendChild( doc.createTextNode(QString::number(dde->getThreshold())));
			p[7].appendChild( doc.createTextNode(QString::number(dde->getFilter())));
			p[8].appendChild( doc.createTextNode(QString::number(dde->getNIterSmoothing())));

			break;
        }

	default: { break; }

	}
}

void BookmarkRTI2D::saveDisplayRTIMode(RenderingRTI renderMode)
{
	QDomElement display = doc.createElement("display_mode");
    if(renderMode == DEFAULT) {
        display.appendChild(doc.createTextNode(QString::number(DEFAULT)));
        bmk.appendChild(display);
    } else if(renderMode == DIFFUSE_GAIN) {
        display.appendChild(doc.createTextNode(QString::number(DIFFUSE_GAIN)));
        bmk.appendChild(display);
    } else if(renderMode == SPECULAR_ENHANCEMENT) {
        display.appendChild(doc.createTextNode(QString::number(SPECULAR_ENHANCEMENT)));
        bmk.appendChild(display);
    } else if(renderMode == NORMAL_ENHANCEMENT) {
        display.appendChild(doc.createTextNode(QString::number(NORMAL_ENHANCEMENT)));
        bmk.appendChild(display);
    } else if(renderMode == UNSHARP_MASKING_IMG) {
        display.appendChild(doc.createTextNode(QString::number(UNSHARP_MASKING_IMG)));
        bmk.appendChild(display);
    } else if(renderMode == UNSHARP_MASKING_LUM) {
        display.appendChild(doc.createTextNode(QString::number(UNSHARP_MASKING_LUM)));
        bmk.appendChild(display);
    } else if(renderMode == COEFF_ENHANCEMENT) {
        display.appendChild(doc.createTextNode(QString::number(COEFF_ENHANCEMENT)));
        bmk.appendChild(display);
    } else if(renderMode == DETAIL_ENHANCEMENT) {
        display.appendChild(doc.createTextNode(QString::number(DETAIL_ENHANCEMENT)));
        bmk.appendChild(display);
    } else if(renderMode == DYN_DETAIL_ENHANCEMENT) {
        display.appendChild(doc.createTextNode(QString::number(DYN_DETAIL_ENHANCEMENT)));
        bmk.appendChild(display);
	} else if (renderMode == NORMALS) {
		display.appendChild(doc.createTextNode(QString::number(NORMALS)));
        bmk.appendChild(display);
	}
}

// 3D model bookmarks preserve directional light on/off, lighting position & key/fill lights, interpolation,
// display mode, and texture on/off.
BookmarkMODEL3D::BookmarkMODEL3D(QString caption, QUuid uuid, QDomDocument &d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode,
                                 bool interpOn, bool dirLightOn, vtkSmartPointer<vtkAssembly> assembly, double brightness,
                                 double contrast, RenderMode3D renderMode, bool textureOn, QString tfn) :
    Bookmark(caption, uuid, d, camera, mode)
{
    saveDirectionalLightOn(dirLightOn);

    double orientation[3];
    assembly->GetOrientation(orientation);
    saveLightVector(orientation);

    saveBrightnessContrast(brightness, contrast);
    save3DInterpolation(interpOn, tfn);
    saveDisplayMode(renderMode);
    saveTextureOn(textureOn, tfn);
}

void BookmarkMODEL3D::save3DInterpolation(bool on, QString tfn)
{
    QDomElement interp;
    if (on) {
        interp = doc.createElement("interpolation_on");
        bmk.appendChild(interp);
        interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_ON)));
    } else if(!tfn.isEmpty()){
        interp = doc.createElement("interpolation_on");
        bmk.appendChild(interp);
        interp.appendChild(doc.createTextNode(QString::number(INTERPOLATION_OFF)));
    }
}

void BookmarkMODEL3D::saveDirectionalLightOn(bool on)
{
    QDomElement dir = doc.createElement("directional_light");
    bmk.appendChild(dir);
    dir.appendChild(doc.createTextNode(QString::number(on)));
}

void BookmarkMODEL3D::saveDisplayMode(RenderMode3D renderMode)
{
    QDomElement display = doc.createElement("display_mode");
    if(renderMode == POINTS3D) {
        display.appendChild(doc.createTextNode(QString::number(POINTS3D)));
        bmk.appendChild(display);
    } else if(renderMode == WIREFRAME3D) {
        display.appendChild(doc.createTextNode(QString::number(WIREFRAME3D)));
        bmk.appendChild(display);
    } else if(renderMode == SURFACE3D) {
        display.appendChild(doc.createTextNode(QString::number(SURFACE3D)));
        bmk.appendChild(display);
    }
}

void BookmarkMODEL3D::saveTextureOn(bool on, QString tfn)
{
    if(on) {
        QDomElement texture = doc.createElement("texture_on");
        bmk.appendChild(texture);
        texture.appendChild(doc.createTextNode(QString::number(TEXTURE_ON)));
    } else if (!tfn.isEmpty()){
        QDomElement texture = doc.createElement("texture_on");
        bmk.appendChild(texture);
        texture.appendChild(doc.createTextNode(QString::number(TEXTURE_OFF)));
    }
}

// 2D CT bookmarks preserve key/fill lights, interpolation, and current slice/orientation.
BookmarkCTSTACK::BookmarkCTSTACK(QString caption, QUuid uuid, QDomDocument &d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode,
                                 bool interpOn, double brightness, double contrast, int slice, CTOrientation orientation) :
    Bookmark(caption, uuid, d, camera, mode)
{
    saveBrightnessContrast(brightness, contrast);
    save2DInterpolation(interpOn);
    saveSlice(slice);
    saveOrientation(orientation);
}

void BookmarkCTSTACK::saveSlice(int slice)
{
    QDomElement slc = doc.createElement("slice");
    bmk.appendChild(slc);
    slc.appendChild(doc.createTextNode(QString::number(slice)));
}

void BookmarkCTSTACK::saveOrientation(CTOrientation orientation)
{
    QDomElement ort = doc.createElement("orientation");
    bmk.appendChild(ort);
    ort.appendChild(doc.createTextNode(QString::number(orientation)));

}

// 3D CT bookmarks preserve lighting orientation, volume rendering mode, blend type, and resolution.
BookmarkCTVOLUME::BookmarkCTVOLUME(QString caption, QUuid uuid, QDomDocument &d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode,
                                   vtkSmartPointer<vtkAssembly> assembly, CTVolumeRenderMode vRenderMode, int blendType, float resolution) :
    Bookmark(caption, uuid, d, camera, mode)
{
    double orientation[3];
    assembly->GetOrientation(orientation);
    saveLightVector(orientation);

    saveVolumeRenderMode(vRenderMode);
    saveBlendType(blendType);
    saveResolution(resolution);
}

void BookmarkCTVOLUME::saveVolumeRenderMode(CTVolumeRenderMode mode)
{
    QDomElement rmode = doc.createElement("render_mode");
    bmk.appendChild(rmode);
    rmode.appendChild(doc.createTextNode(QString::number(mode)));
}

void BookmarkCTVOLUME::saveBlendType(int type)
{
    QDomElement blend = doc.createElement("blend_type");
    bmk.appendChild(blend);
    blend.appendChild(doc.createTextNode(QString::number(type)));
}

void BookmarkCTVOLUME::saveResolution(float r)
{
    QDomElement res = doc.createElement("resolution");
    bmk.appendChild(res);
    res.appendChild(doc.createTextNode(QString::number(r)));
}
