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
