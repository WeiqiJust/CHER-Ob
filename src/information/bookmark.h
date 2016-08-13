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

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QDomDocument>
#include <QDomElement>
#include <QUuid>
#include <QXmlStreamReader>

#include "vtkAssembly.h"
#include "vtkCamera.h"
#include "vtkSmartPointer.h"
#include "../function/renderingmode.h"

#include "../vtkEnums.h"

#include <vcg/space/point3.h>


/**
 * Base Bookmark class.
 */
class Bookmark
{
public:
    Bookmark(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode);

    virtual void loadBookmark() = 0;

    inline QDomElement getBookmark()
	{
        return bmk;
    }

protected:
    inline void saveCameraAngle(double angle)
	{
        QDomElement ang = doc.createElement("camera_angle");
        ang.appendChild(doc.createTextNode(QString::number(angle)));
        bmk.appendChild(ang);
    }

    inline void saveCameraPosition(double pos[3])
    {
        QDomElement position = doc.createElement("camera_position");
        bmk.appendChild(position);
        QDomElement pos_x = doc.createElement("x");
        QDomElement pos_y = doc.createElement("y");
        QDomElement pos_z = doc.createElement("z");
        position.appendChild(pos_x);
        position.appendChild(pos_y);
        position.appendChild(pos_z);
        pos_x.appendChild(doc.createTextNode(QString::number(pos[0])));
        pos_y.appendChild(doc.createTextNode(QString::number(pos[1])));
        pos_z.appendChild(doc.createTextNode(QString::number(pos[2])));
    }

    inline void saveClippingRange(double clip[2])
    {
        QDomElement clipping = doc.createElement("clipping_range");
        bmk.appendChild(clipping);
        QDomElement clip_near = doc.createElement("near_plane");
        QDomElement clip_far = doc.createElement("far_plane");
        clipping.appendChild(clip_near);
        clipping.appendChild(clip_far);
        clip_near.appendChild(doc.createTextNode(QString::number(clip[0])));
        clip_far.appendChild(doc.createTextNode(QString::number(clip[1])));
    }

    inline void saveFocalPoint(double foc[3])
    {
        QDomElement focal = doc.createElement("focal_point");
        bmk.appendChild(focal);
        QDomElement foc_x = doc.createElement("x");
        QDomElement foc_y = doc.createElement("y");
        QDomElement foc_z = doc.createElement("z");
        focal.appendChild(foc_x);
        focal.appendChild(foc_y);
        focal.appendChild(foc_z);
        foc_x.appendChild(doc.createTextNode(QString::number(foc[0])));
        foc_y.appendChild(doc.createTextNode(QString::number(foc[1])));
        foc_z.appendChild(doc.createTextNode(QString::number(foc[2])));
    }

    inline void saveParallelScale(double scale)
    {
        QDomElement parallel = doc.createElement("parallel_scale");
        bmk.appendChild(parallel);
        parallel.appendChild(doc.createTextNode(QString::number(scale)));
    }

    inline void saveViewUp(double view[3])
    {
        QDomElement viewup = doc.createElement("view_up");
        bmk.appendChild(viewup);
        QDomElement viewup_x = doc.createElement("x");
        QDomElement viewup_y = doc.createElement("y");
        QDomElement viewup_z = doc.createElement("z");
        viewup.appendChild(viewup_x);
        viewup.appendChild(viewup_y);
        viewup.appendChild(viewup_z);
        viewup_x.appendChild(doc.createTextNode(QString::number(view[0])));
        viewup_y.appendChild(doc.createTextNode(QString::number(view[1])));
        viewup_z.appendChild(doc.createTextNode(QString::number(view[2])));
    }

    inline void saveFileType(WidgetMode mode)
	{
        QDomElement filetype = doc.createElement("filetype");
        filetype.appendChild(doc.createTextNode(QString::number(mode)));
        bmk.appendChild(filetype);
    }

    void save2DInterpolation(bool on);
    void saveBrightnessContrast(double brt, double cst);
    void saveLightVector(double orientation[3]);

    QDomDocument& doc;
    QDomElement bmk;
};

/**
 * Class for 2D images (jpg, png, bmp, tif, exr).
 */
class BookmarkIMAGE2D : public Bookmark
{
public:
    BookmarkIMAGE2D(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode, bool interpOn);

    void loadBookmark() { }
};

/**
 * Class for 2D RTI images (rti and ptm).
 */
class BookmarkRTI2D : public Bookmark
{
public:
    BookmarkRTI2D(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode, bool interpOn, vcg::Point3f light, RenderingRTI renderMode, RenderingMode* renderingMode);

    void loadBookmark() { }

private:
	void saveDisplayRTIMode(RenderingRTI mode);
	void saveDisplayRTIPara(RenderingRTI renderMode, RenderingMode* renderingMode);
};

/**
 * Class for 3D models (ply, obj, wrl).
 */
class BookmarkMODEL3D : public Bookmark
{
public:
    BookmarkMODEL3D(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode,
                    bool interpOn, bool dirLightOn, vtkSmartPointer<vtkAssembly> assembly, double brightness, double contrast,
                    RenderMode3D renderMode, bool textureOn, QString tfn);

    void loadBookmark() { }

private:
    void save3DInterpolation(bool on, QString tfn);
    void saveDirectionalLightOn(bool on);
    void saveDisplayMode(RenderMode3D mode);
    void saveTextureOn(bool on, QString tfn);
};

/**
 * Class for 2D CT images.
 */
class BookmarkCTSTACK : public Bookmark
{
public:
    BookmarkCTSTACK(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode,
                    bool interpOn, double brightness, double contrast, int slice, CTOrientation orientation);

    void loadBookmark() { }

private:
    void saveSlice(int slice);
    void saveOrientation(CTOrientation orientation);
};

/**
 * Class for 3D CT images.
 */
class BookmarkCTVOLUME : public Bookmark
{
public:
    BookmarkCTVOLUME(QString caption, QUuid uuid, QDomDocument& d, vtkSmartPointer<vtkCamera> camera, WidgetMode mode,
                     vtkSmartPointer<vtkAssembly> assembly, CTVolumeRenderMode vRenderMode, int blendType, float resolution);

    void loadBookmark() { }

private:
    void saveVolumeRenderMode(CTVolumeRenderMode mode);
    void saveBlendType(int type);
    void saveResolution(float r);
};

#endif // BOOKMARK_H
