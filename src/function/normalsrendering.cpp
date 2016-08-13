/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Ying Yang (ying.yang.yy368@yale.edu)

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
/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/

#include "normalsrendering.h"
#include "../io/util.h"

/*====================================================================
 *
 * NormalsRendering class
 *
 *===================================================================*/

NormalsRendering::NormalsRendering() {}

NormalsRendering::~NormalsRendering() {}

QString NormalsRendering::getTitle()
{
    return "Normals Visualization";
}

QWidget* NormalsRendering::getControl(QWidget *parent)
{
    // The normals rendering does not use any controls, so we just
    // create an empty widget. This follows the code in RenderingDialog,
    // which does the same thing when there are no entries in the list
    // of rendering options.

    QWidget* c = new QWidget(parent);
    return c;
}

bool NormalsRendering::isLightInteractive()
{
    return false;
}

bool NormalsRendering::supportRemoteView()
{
    return false;
}

bool NormalsRendering::enabledLighting()
{
    return false;
}

void NormalsRendering::applyPtmLRGB(const PyramidCoeff& coeff, const PyramidRGB& rgb, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
    renderNormals(mipMapSize, normals, info, buffer);
}

void NormalsRendering::applyPtmRGB(const PyramidCoeff& redCoeff, const PyramidCoeff& greenCoeff, const PyramidCoeff& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
    renderNormals(mipMapSize, normals, info, buffer);
}

void NormalsRendering::applyHSH(const PyramidCoeffF& redCoeff, const PyramidCoeffF& greenCoeff, const PyramidCoeffF& blueCoeff, const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
    renderNormals(mipMapSize, normals, info, buffer);
}

void NormalsRendering::renderNormals(const QSize* mipMapSize, const PyramidNormals& normals, const RenderingInfo& info, unsigned char* buffer)
{
    // Create the maps of the normals.

    int offsetBuf = 0;
    const vcg::Point3f* normalsLevel = normals.getLevel(info.level);
    for (int y = info.offy; y < info.offy + info.height; y++)
    {
        for (int x = info.offx; x < info.offx + info.width; x++)
        {
            int offset = y * mipMapSize[info.level].width() + x;
            for (int i = 0; i < 3; i++)
                buffer[offsetBuf + i] = toColor(normalsLevel[offset][i]);
            buffer[offsetBuf + 3] = 255;
            offsetBuf += 4;
        }
    }
}
