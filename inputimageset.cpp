#include "inputimageset.h"
#include <vtkMath.h>
#include <QVector>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>

InputImage::InputImage()
{
    mCamPos[0] = mCamPos[1] = mCamPos[2] = 0;
    mViewVec[0] = mViewVec[1] = mViewVec[2] = 0;
}

InputImage::InputImage(double camPos[], double viewVec[], QString filename)
{
    mCamPos[0] = camPos[0];
    mCamPos[1] = camPos[1];
    mCamPos[2] = camPos[2];

    mViewVec[0] = viewVec[0];
    mViewVec[1] = viewVec[1];
    mViewVec[2] = viewVec[2];

    mFileName = filename;
}

InputImage::InputImage(double px, double py, double pz, double vx, double vy, double vz, QString filename)
{
    mCamPos[0] = px;
    mCamPos[1] = py;
    mCamPos[2] = pz;

    mViewVec[0] = vx;
    mViewVec[1] = vy;
    mViewVec[2] = vz;

    mFileName = filename;
}

void InputImage::setParameters(double _f, double R11, double R12, double R13, double R21, double R22, double R23, double R31, double R32, double R33, double _tx, double _ty, double _tz)
{
    f = _f;
    R[0] = R11; R[1] = R12; R[2] = R13;
    R[3] = R21; R[4] = R22; R[5] = R23;
    R[6] = R31; R[7] = R32; R[8] = R33;
    t[0] = _tx; t[1] = _ty; t[2] = _tz;

    double posx = -R11*_tx - R21*_ty - R31*_tz;
    double posy = -R12*_tx - R22*_ty - R32*_tz;
    double posz = -R13*_tx - R23*_ty - R33*_tz;

    double viewX = -R31;
    double viewY = -R32;
    double viewZ = -R33;

    mCamPos[0] = posx;
    mCamPos[1] = posy;
    mCamPos[2] = posz;

    mViewVec[0] = viewX;
    mViewVec[1] = viewY;
    mViewVec[2] = viewZ;
}

void InputImage::setFilename(QString filename)
{
    mFileName = filename;
    vtkSmartPointer<vtkJPEGReader> jPEGReader = vtkSmartPointer<vtkJPEGReader>::New();
    jPEGReader->SetFileName(filename.toStdString().c_str());
    jPEGReader->Update();
    imW = jPEGReader->GetOutput()->GetDimensions()[0];
    imH = jPEGReader->GetOutput()->GetDimensions()[1];
    ox = imW / 2;
    oy = imH / 2;
    qDebug() << "loading image " << filename << " width " << imW;
}

double InputImage::proximityToCamera(double otherCamPos[], double otherViewVec[])
{
    double score = vtkMath::Dot(mViewVec,otherViewVec);
    return score;
}

void InputImage::projectWorldToImage(double xw, double yw, double zw, double ptIm[])
{
    double xc = R[0]*xw + R[1]*yw + R[2]*zw + t[0];
    double yc = R[3]*xw + R[4]*yw + R[5]*zw + t[1];
    double zc = R[6]*xw + R[7]*yw + R[8]*zw + t[2];

    double xch = -xc/zc;
    double ych = -yc/zc;

    double xi1 = xch * f;
    double yi1 = ych * f;

    double xi2 = xi1 + ox;
    double yi2 = oy - yi1;

    ptIm[0] = xi2;
    ptIm[1] = yi2;
    ptIm[2] = 1.0;
}

InputImageSet::InputImageSet()
{
    mIsValid = false;
}

InputImageSet::InputImageSet(QString directory)
{
    mIsValid = false;
    loadImageSetFromPMVSOutput(directory);
}

bool InputImageSet::loadImageSetFromPMVSOutput(QString directory)
{
    QString filename = directory;
    filename.append("bundle.rd.out");
    QFile myfile(filename);
    if(!myfile.open(QIODevice::ReadOnly)) {
		qDebug () << "Open InputImageSet Failed";
        return false;
    }
    QTextStream in(&myfile);

    QString foo = in.readLine();
    QString topline = in.readLine();
    QStringList topfields = topline.split(" ");
    int num_cams = topfields[0].toInt();
    topfields.clear();
	
    for(int k = 0; k < num_cams; k++)
    {
        QString line1 = QString(in.readLine());
        QStringList fields1 = line1.split(" ");
        double f = fields1[0].toDouble();

        QString line2 = in.readLine();
        QStringList fields2 = line2.split(" ");
        double R11 = fields2[0].toDouble();
        double R12 = fields2[1].toDouble();
        double R13 = fields2[2].toDouble();

        QString line3 = in.readLine();
        QStringList fields3 = line3.split(" ");
        double R21 = fields3[0].toDouble();
        double R22 = fields3[1].toDouble();
        double R23 = fields3[2].toDouble();

        QString line4 = in.readLine();
        QStringList fields4 = line4.split(" ");
        double R31 = fields4[0].toDouble();
        double R32 = fields4[1].toDouble();
        double R33 = fields4[2].toDouble();

        QString line5 = in.readLine();
        QStringList fields5 = line5.split(" ");
        float tx = fields5[0].toDouble();
        float ty = fields5[1].toDouble();
        float tz = fields5[2].toDouble();

        InputImage *newImage = new InputImage();
        newImage->setParameters(f,R11,R12,R13,R21,R22,R23,R31,R32,R33,tx,ty,tz);

        QString imagename = directory;
        char buffer[20];
        sprintf(buffer,"%08d.jpg",k);
        QString foo(buffer);
        imagename.append(foo);
        newImage->setFilename(imagename);
        imageSet.push_back(newImage);
    }
    mIsValid = true;
    return true;
}

int InputImageSet::getClosestImage(double otherCamPos[], double otherViewVec[])
{
    int bestImage = -1;  // bug is here, previous is -1
    double bestScore = -9999;
    for(int k = 0; k < imageSet.size(); ++k)
    {
        double score_k = imageSet[k]->proximityToCamera(otherCamPos, otherViewVec);
        if(score_k > bestScore)
        {
            bestScore = score_k;
            bestImage = k;
        }
    }
    qDebug() << "The best image is " << bestImage;
    return bestImage;
}
