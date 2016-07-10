/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)

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

#ifndef INPUTIMAGESET_H
#define INPUTIMAGESET_H
#include <QString>
#include <QVector>

class InputImage
{
public:
    InputImage();
    InputImage(double camPos[], double viewVec[], QString filename);
    InputImage(double px, double py, double pz, double vx, double vy, double vz, QString filename);
    void setParameters(double f,
                       double R11, double R12, double R13,
                       double R21, double R22, double R23,
                       double R31, double R32, double R33,
                       double tx, double ty, double tz);
    void setFilename(QString filename);
    double proximityToCamera(double otherCamPos[], double otherViewVec[]);
    void projectWorldToImage(double x, double y, double z, double ptIm[]);

    double f;
    double R[9];
    double t[3];
    double ox, oy;
    int imW, imH;
    double mCamPos[3];
    double mViewVec[3];
    QString mFileName;
};

class InputImageSet
{
public:
    InputImageSet();
    InputImageSet(QString directory);
    bool loadImageSetFromPMVSOutput(QString directory);
    int getClosestImage(double otherCamPos[], double otherViewVec[]);

    QVector<InputImage*> imageSet;
    bool isValid() { return mIsValid; }

private:
    bool mIsValid;
};

#endif // INPUTIMAGESET_H
