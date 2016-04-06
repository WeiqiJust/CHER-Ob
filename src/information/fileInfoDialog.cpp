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
#include "fileInfoDialog.h"
#include "../visualization/vtkWidget.h"

#define SIZE (1024)

FileInfoDialog::FileInfoDialog(VtkWidget *gla)
{

    hyperdata = NULL;
    texture = NULL;

    vbox = new QVBoxLayout(this);

    if(!gla) 
	{
		QMessageBox mb;
        mb.critical(this, tr("File Info Error"),
			tr("Information about this file is not available because the file does not exist."));
        return;
	}
    w = gla;

    if(w->mHyperImageData)
        hyperdata = w->mHyperImageData;
    if (w->mRgbTexture)
        texture = w->mRgbTexture;

    grid = new QGridLayout();
    textureFilename = w->mRgbTextureFilename;
    finfo = new QFileInfo(w->mFilename);
    tfinfo = new QFileInfo(textureFilename);
    locale = QLocale::system();
    int mode = w->mWidgetMode;
    viewer = w->mVtkImageViewer;
    isDICOM = w->mIsDICOM;
    points = w->mNumberOfPoints;
    polys = w->mNumberOfPolys;

    if(w->format.hasOpenGL()) {
        QString openGLVersion = "OpenGL " + QString::number(w->format.majorVersion()) + "." + QString::number(w->format.minorVersion());
        openGLLabel = new QLabel(openGLVersion, this);
    } else {
        openGLLabel = new QLabel(tr("OpenGL not supported"), this);
    }

    QString cores = tr("Number of Cores: ") + QString::number(omp_get_num_procs());
    coresLabel = new QLabel(cores, this);

    int row = grid->rowCount();
    grid->addWidget(openGLLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(coresLabel, row, 1, 1, 1, Qt::AlignRight);

    if(mode == IMAGE2D || mode == CTSTACK || mode == RTI2D) {
        get2DInfo();

    } else if(mode == MODEL3D || mode == CTVOLUME) {
        get3DInfo();

    } else {
        QMessageBox mb;
        mb.critical(this, tr("File Info Error"),
                    tr("Information about this file is not available because it is of an unknown file type."));
        return;
    }

    grid->setColumnStretch(0,0);
    grid->setColumnStretch(1,5);
    grid->setVerticalSpacing(15);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    vbox->addLayout(grid);
    vbox->setSpacing(20);

    vboxData = new QVBoxLayout();
    hboxButtons = new QHBoxLayout();
    okButton = new QPushButton(tr("OK"));
    hboxButtons->addWidget(okButton, 0 , Qt::AlignRight);

    connect(okButton, SIGNAL(clicked()), this, SLOT(hide()));

    vbox->addLayout(hboxButtons);
	
	mDialog = new QDialog();
    mDialog->setLayout(vbox);

    QString title = tr("File Info: ") + finfo->fileName();
	
    mDialog->setWindowTitle(title);

    mDialog->setMinimumWidth(350);
    mDialog->adjustSize();
    mDialog->show();
}

void FileInfoDialog::get2DInfo()
{
    QString dimensions;
    int numCompo;
    int dims[3];
    int row;

    vtkSmartPointer<vtkImageData> image = viewer->GetInput();

    if (image)
    {
        image->GetDimensions(dims);
        numCompo = image->GetNumberOfScalarComponents();

        if(isDICOM) {
            dimensions = locale.toString(dims[0]) + " x " + locale.toString(dims[1]) + " x " + locale.toString(dims[2]);
        } else {
            dimensions = locale.toString(dims[0]) + " x " + locale.toString(dims[1]) + " x " + locale.toString(numCompo);
        }
    }

    filenameLabel = new QLabel(tr("Filename:"), this);
    filenameLine = new QLineEdit(QDir::toNativeSeparators(finfo->absoluteFilePath()), this);
    filenameLine->setMinimumWidth(380);
    filenameLine->setReadOnly(true);

    filesizeLabel = new QLabel(tr("File Size:"), this);
    if(finfo->isFile())
        filesizeLabel2 = new QLabel(formatFilesize(finfo->size()), this);
    else {
        QDirIterator it(finfo->absoluteFilePath());
        qint64 size = 0;
        while(it.hasNext()) {
            QFileInfo fi(it.next());
            size += fi.size();
        }
        filesizeLabel2 = new QLabel(formatFilesize(size), this);
    }

    dimensionsLabel = new QLabel(tr("Image Dimensions:"), this);
    dimensionsLabel2 = new QLabel(dimensions, this);

    row = grid->rowCount();
    grid->addWidget(filenameLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filenameLine, row, 1);

    row = grid->rowCount();
    grid->addWidget(filesizeLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filesizeLabel2, row, 1, 1, 1, Qt::AlignLeft);

    row = grid->rowCount();
    grid->addWidget(dimensionsLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(dimensionsLabel2, row, 1, 1, 1, Qt::AlignLeft);
}

void FileInfoDialog::get3DInfo()
{
    int hdims[3] = {0,0,0};
    int ldims[3] = {0,0,0};
    QString texDimensions;
    int row;

    if (texture)
    {
      hyperdata->GetDimensions(hdims);
      hdims[2] = hyperdata->GetNumberOfScalarComponents();
      texture->GetInput()->GetDimensions(ldims);
      ldims[2] = texture->GetInput()->GetNumberOfScalarComponents();
    }
    if (hdims[0] != 0 || hdims[1] != 0 ) // hyperspectral data
    {
        texDimensions = locale.toString(hdims[0]) + " x " +
                        locale.toString(hdims[1]) + " x " +
                        locale.toString(hdims[2]);
    } else
    {
        texDimensions = locale.toString(ldims[0]) + " x " +
                        locale.toString(ldims[1]) + " x " +
                        locale.toString(ldims[2]);
    }

    filenameLabel = new QLabel(tr("Geometry Filename:"), this);
    filenameLine = new QLineEdit(QDir::toNativeSeparators(finfo->absoluteFilePath()), this);
    filenameLine->setMinimumWidth(360);
    filenameLine->setReadOnly(true);

    filesizeLabel = new QLabel(tr("Geometry File Size:"), this);
    if(finfo->isFile())
        filesizeLabel2 = new QLabel(formatFilesize(finfo->size()), this);
    else {
        QDirIterator it(finfo->absoluteFilePath());
        qint64 size = 0;
        while(it.hasNext()) {
            QFileInfo fi(it.next());
            size += fi.size();
        }
        filesizeLabel2 = new QLabel(formatFilesize(size), this);
    }

    if(!textureFilename.isEmpty()) {
        texFilenameLabel = new QLabel(tr("Texture Filename:"), this);
        texFilenameLine = new QLineEdit(QDir::toNativeSeparators(tfinfo->absoluteFilePath()), this);
        texFilenameLine->setMinimumWidth(360);
        texFilenameLine->setReadOnly(true);

        texFilesizeLabel = new QLabel(tr("Texture File Size:"), this);
        if(finfo->isFile())
            texFilesizeLabel2 = new QLabel(formatFilesize(tfinfo->size()), this);
        else {
            QDirIterator it(tfinfo->absoluteFilePath());
            qint64 size = 0;
            while(it.hasNext()) {
                QFileInfo fi(it.next());
                size += fi.size();
            }
            texFilesizeLabel2 = new QLabel(formatFilesize(size), this);
        }

        texDimensionsLabel = new QLabel(tr("Texture Dimensions:"), this);
        texDimensionsLabel2 = new QLabel(texDimensions, this);
    }

    if(points > 0) {
        pointLabel = new QLabel(tr("Number of Points:"), this);
        pointLabel2 = new QLabel(locale.toString(points), this);
    }

    if(polys > 0) {
        polyLabel = new QLabel(tr("Number of Polygons:"), this);
        polyLabel2 = new QLabel(locale.toString(polys), this);
    }

    row = grid->rowCount();
    grid->addWidget(filenameLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filenameLine, row, 1);

    row = grid->rowCount();
    grid->addWidget(filesizeLabel, row, 0, 1, 1, Qt::AlignLeft);
    grid->addWidget(filesizeLabel2, row, 1, 1, 1, Qt::AlignLeft);

    row = grid->rowCount();
    if(points > 0) {
        grid->addWidget(pointLabel, row, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(pointLabel2, row, 1, 1, 1, Qt::AlignLeft);
    }

    row = grid->rowCount();
    if(polys > 0) {
        grid->addWidget(polyLabel, row, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(polyLabel2, row, 1, 1, 1, Qt::AlignLeft);
    }

    row = grid->rowCount();
    if(!textureFilename.isEmpty()) {
        grid->addWidget(texFilenameLabel, row, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(texFilenameLine, row, 1);

        grid->addWidget(texFilesizeLabel, row+1, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(texFilesizeLabel2, row+1, 1);

        grid->addWidget(texDimensionsLabel, row+2, 0, 1, 1, Qt::AlignLeft);
        grid->addWidget(texDimensionsLabel2, row+2, 1, 1, 1, Qt::AlignLeft);
    }
}

QString FileInfoDialog::formatFilesize(qint64 bytes)
{
    QString filesize;

    if(bytes > SIZE) { // kb
        bytes >>= 10;
        if(bytes > SIZE) { // mb
            bytes >>= 10;
            if(bytes > SIZE) { // gb
                bytes >>= 10;
                filesize = QString::number(bytes) + " GB";
            } else
                filesize = QString::number(bytes) + " MB";
        } else
            filesize = QString::number(bytes) + " KB";
    } else
        filesize = QString::number(bytes) + " B";

    return filesize;
}

