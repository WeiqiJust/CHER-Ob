/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

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
#include "dialog.h"
#include "../visualization/vtkWidget.h"

#define SIZE (1024)

NewProjectDialog::NewProjectDialog(const QString path)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mLastPath = path;

	mDialog->setWindowTitle(tr("New Project"));

	mRoleVBox = new QVBoxLayout();
	mGroupBox = new QGroupBox(tr("User Mode"));
	mCreatorRadioButton = new QRadioButton(tr("Creator"));
    mModifierRadioButton = new QRadioButton(tr("Modifier"));
    mViewerRadioButton = new QRadioButton(tr("Viewer"));
    mCreatorRadioButton->setChecked(true);
	mUserNameLabel = new QLabel(tr("User Name:"));
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(350);
    mUserNameLabel->setBuddy(mUserNameLineEdit);
	mUserNameHBox = new QHBoxLayout();
	mUserNameHBox->addWidget(mUserNameLabel);
	mUserNameHBox->addWidget(mUserNameLineEdit, 0 , Qt::AlignRight);
    mRoleVBox->addWidget(mCreatorRadioButton);
    mRoleVBox->addWidget(mModifierRadioButton);
    mRoleVBox->addWidget(mViewerRadioButton);
	mRoleVBox->addLayout(mUserNameHBox);
    mRoleVBox->addStretch(1);
    mGroupBox->setLayout(mRoleVBox);

	mObjectLabel = new QLabel(tr("2D/3D Data:"));
	mObjectLineEdit = new QLineEdit();
	mObjectLineEdit->setFixedWidth(270);
    mObjectLabel->setBuddy(mObjectLineEdit);
	mObjectBrowseButton = new QPushButton("Browse");
	connect(mObjectBrowseButton, SIGNAL(clicked()), this, SLOT(objectBrowse()));
	mObjectHBox = new QHBoxLayout();
	mObjectHBox->addWidget(mObjectLabel);
	mObjectHBox->addWidget(mObjectLineEdit, 0 , Qt::AlignRight);
	mObjectHBox->addWidget(mObjectBrowseButton);

	mCTLabel = new QLabel(tr("CT Director: "));
    mCTLineEdit = new QLineEdit();
	mCTLineEdit->setFixedWidth(270);
    mCTLabel->setBuddy(mCTLineEdit);
	mCTBrowseButton = new QPushButton("Browse");
	connect(mCTBrowseButton, SIGNAL(clicked()), this, SLOT(ctBrowse()));
	mCTHBox = new QHBoxLayout();
	mCTHBox->addWidget(mCTLabel);
	mCTHBox->addWidget(mCTLineEdit, 0 , Qt::AlignRight);
	mCTHBox->addWidget(mCTBrowseButton);

	mLoadDataVBox = new QVBoxLayout();
	mLoadDataGroupBox = new QGroupBox(tr("Load Data"));
	mLoadDataLabel = new QLabel("Load 2D/3D data file or CT directory, or do it later");
	mLoadDataVBox->addWidget(mLoadDataLabel);
	mLoadDataVBox->addLayout(mObjectHBox);
	mLoadDataVBox->addLayout(mCTHBox);
	mLoadDataVBox->addStretch(1);
	mLoadDataGroupBox->setLayout(mLoadDataVBox);

    mProjectNameLabel = new QLabel(tr("Name:  "));
	mProjectNameEdit = new QLineEdit();
	mProjectNameEdit->setFixedWidth(350);
    mProjectNameLabel->setBuddy(mProjectNameEdit);
	connect(mProjectNameEdit, SIGNAL(textChanged(QString)), this, SLOT(projectNameChanged(QString)));
	mProjectHBox = new QHBoxLayout();
	mProjectHBox->addWidget(mProjectNameLabel, 0 , Qt::AlignLeft);
    mProjectHBox->addWidget(mProjectNameEdit, 0 , Qt::AlignRight);
	
    mLocationLabel = new QLabel(tr("Location:"));
	mLocationLineEdit = new QLineEdit();
    mLocationLabel->setBuddy(mLocationLineEdit);
	connect(mLocationLineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectPathChanged(QString)));
	mLocationLineEdit->setText(QCoreApplication::applicationDirPath());
	mProjectPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
	mLocationLineEdit->setFixedWidth(270);
	mLocationBrowseButton = new QPushButton("Browse");
	connect(mLocationBrowseButton, SIGNAL(clicked()), this, SLOT(locationBrowse()));
	mLocationHBox = new QHBoxLayout();
	mLocationHBox->addWidget(mLocationLabel);
	mLocationHBox->addWidget(mLocationLineEdit, 0 , Qt::AlignRight);
    mLocationHBox->addWidget(mLocationBrowseButton);

	mKeywordLabel = new QLabel(tr("Keywords:"));
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(350);
    mKeywordLabel->setBuddy(mKeywordLineEdit);
	mKeywordHBox = new QHBoxLayout();
	mKeywordHBox->addWidget(mKeywordLabel);
    mKeywordHBox->addWidget(mKeywordLineEdit, 0 , Qt::AlignRight);

	mAffiliationLabel = new QLabel(tr("Affiliation:"));
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(350);
    mAffiliationLabel->setBuddy(mAffiliationLineEdit);
	mAffiliationHBox = new QHBoxLayout();
	mAffiliationHBox->addWidget(mAffiliationLabel);
    mAffiliationHBox->addWidget(mAffiliationLineEdit, 0 , Qt::AlignRight);

	mDescriptionLabel = new QLabel(tr("Description:"));
	mDescriptionEdit = new QTextEdit(mDialog);

	mProjectInfoVBox = new QVBoxLayout();
	mProjectInfoGroupBox = new QGroupBox(tr("Project Info"));
	mProjectInfoVBox->addLayout(mProjectHBox);
	mProjectInfoVBox->addLayout(mLocationHBox);
	mProjectInfoVBox->addLayout(mKeywordHBox);
	mProjectInfoVBox->addLayout(mAffiliationHBox);
	mProjectInfoVBox->addWidget(mDescriptionLabel);
	mProjectInfoVBox->addWidget(mDescriptionEdit);
	mProjectInfoVBox->addStretch(1);
	mProjectInfoGroupBox->setLayout(mProjectInfoVBox);

	mButtonGridBox = new QGridLayout();
	mNextButton = new QPushButton("Next");
	mNextButton->setEnabled(false);
	connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mButtonGridBox->addWidget(mNextButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	
	mVbox->addWidget(mGroupBox);
	mVbox->addWidget(mLoadDataGroupBox);
	mVbox->addWidget(mProjectInfoGroupBox);
	mVbox->addLayout(mButtonGridBox);
	
	connect(this, SIGNAL(infoUpdate()), this, SLOT(enableNextButton()));
	mDialog->setMinimumWidth(450);
	mDialog->setLayout(mVbox);
}

void NewProjectDialog::locationBrowse()
{
	
	mProjectPath = QFileDialog::getExistingDirectory(this, tr("Location"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mLocationLineEdit->setText(mProjectPath);
	mLocationLineEdit->displayText();
}

void NewProjectDialog::objectBrowse()
{
	QStringList filters;
	filters.push_back("Images (*.ply *.obj *.wrl *.png *.jpg *.tif *.bmp *.exr *.dcm *rti *ptm *hsh)");
	filters.push_back("*.ply");
	filters.push_back("*.obj");
	filters.push_back("*.wrl");
	filters.push_back("*.exr");
	filters.push_back("*.png");
	filters.push_back("*.jpg");
	filters.push_back("*.tif");
	filters.push_back("*.bmp");
	filters.push_back("*.dcm");
	filters.push_back("*.rti");
	filters.push_back("*.ptm");
	filters.push_back("*.hsh");

	QStringList fileNames;
	fileNames = QFileDialog::getOpenFileNames(this,tr("Load an Image or Object"), mLastPath, filters.join(";;")); // entering the last path
	QString fileName;
	for (int i = 0; i < fileNames.size(); i++)
	{
		fileName = fileName + fileNames[i] + QString(";");
	}
	mObjectLineEdit->setText(fileName);
	mObjectLineEdit->displayText();
}

void NewProjectDialog::ctBrowse()
{
	QString ctDir;
	ctDir = QFileDialog::getExistingDirectory(this, tr("Load a CT Directory"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mCTLineEdit->setText(ctDir);
	mCTLineEdit->displayText();
}

void NewProjectDialog::next()
{
	mFullPath = mProjectPath.simplified();
	if (!QDir().exists(mProjectPath))
	{
		qDebug()<<"the path do not exist";
		QString notice;
		notice.append(QString("Error!\nThe directory ") + mProjectPath + QString(" does not exist!"));
		//mError = new ErrorDialog(notice);
		QMessageBox::critical(this, tr("Object Opening Error"), "Unable to open " + mProjectPath + ".");
		return;
	}
	if (mFullPath.split(QDir::separator())[1] == QString())
	{
		mFullPath.append(mProjectName);
	}
	else
	{
		mFullPath.append(QDir::separator() + mProjectName);
	}
	if (QDir().exists(mFullPath))
	{
		QString message("The project with specified name already exists in the specified location!\n");
		message.append(QString("Do you want to overwrite the existing project with this new project?"));

		QMessageBox::StandardButton reply;
		reply = QMessageBox::warning(this, tr("WARNING"), message, QMessageBox::Ok|QMessageBox::No);
		if (reply == QMessageBox::Ok) 
		{
			nextReady();
		}
		else 
		{
			nextCancel();
		}
	}
	else
	{
		nextReady();
	}
}

void NewProjectDialog::nextReady()
{
	USERMODE userMode;
	if (mCreatorRadioButton->isChecked()) userMode = CREATOR;
	else if (mModifierRadioButton->isChecked()) userMode = MODIFIER;
	else userMode = VIEWER;
	emit nextPressed(mFullPath, mProjectName, userMode, mUserNameLineEdit->text(), mObjectLineEdit->text(), 
		mCTLineEdit->text(), mKeywordLineEdit->text(), mAffiliationLineEdit->text(), mDescriptionEdit->toPlainText());
	mDialog->hide();
	
}

void NewProjectDialog::nextCancel()
{
	mProjectNameEdit->clear();
}


void NewProjectDialog::cancel()
{
	mDialog->hide();
}

void NewProjectDialog::projectNameChanged(QString name)
{
	mProjectName = name;
	emit infoUpdate();
}

void NewProjectDialog::projectPathChanged(QString path)
{
	mProjectPath = path;
	emit infoUpdate();
}

void NewProjectDialog::enableNextButton()
{
	if (!mProjectPath.isEmpty() && !mProjectName.isEmpty())
	{
		mNextButton->setEnabled(true);
	}
	else
	{
		mNextButton->setEnabled(false);
	}
}

QString NewProjectDialog::getProjectName()
{
	return mProjectNameEdit->text();
}

QString NewProjectDialog::getProjectPath()
{
	return mLocationLineEdit->text();
}

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

ProjectInfoDialog::ProjectInfoDialog(const QString name, const QString path, const QString keyword, const QString affiliation,
									 const QString userName, const QString description)
{
	mDialog = new QDialog();
	mGrid = new QGridLayout();
	mVBox = new QVBoxLayout();
	mHBox = new QHBoxLayout();
	mHBoxDescription = new QHBoxLayout();
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(200);
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(200);
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(200);
	mName = new QLabel("Project Name:");
	mNameContent = new QLabel(name);
	mName->setBuddy(mNameContent);
	mPath = new QLabel("Location:");
	mPathContent = new QLabel(path);
	mPath->setBuddy(mPathContent);
	mKeyword = new QLabel("Keyword:");
	mKeywordContent = new QLabel(keyword);
	mKeyword->setBuddy(mKeywordContent);
	mAffiliation = new QLabel("Affiliation:");
	mAffiliationContent = new QLabel(affiliation);
	mAffiliation->setBuddy(mAffiliationContent);
	mUserName = new QLabel("User Name:");
	mUserNameContent = new QLabel(userName);
	mUserName->setBuddy(mUserNameContent);
	mDescription = new QLabel("Description:");
	mDescriptionContent = new QLabel(description);
	mDescriptionContent->setFixedHeight(150);
	mDescriptionContent->setAlignment(Qt::AlignTop);
	mDescriptionText = new QTextEdit();
	mDescriptionText->setText(description);
	cancelButton = new QPushButton("Cancel");
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
	editButton = new QPushButton("Edit");
	connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
	saveButton = new QPushButton("Save");
	saveButton->setEnabled(false);
	connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
	mHBox->addWidget(saveButton, 0, Qt::AlignRight);
	mHBox->addWidget(editButton, 0, Qt::AlignRight);
	mHBox->addWidget(cancelButton, 0, Qt::AlignRight);

	mGrid->setColumnStretch(0,0);
    mGrid->setColumnStretch(1,5);
    mGrid->setVerticalSpacing(10);
   // mGrid->setSizeConstraint(QLayout::SetFixedSize);

	int row;
	row = mGrid->rowCount();
	mGrid->addWidget(mName, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mNameContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mPath, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mPathContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mUserName, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mUserNameContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mKeyword, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mKeywordContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mAffiliation, row, 0, 1, 1, Qt::AlignLeft);
    mGrid->addWidget(mAffiliationContent, row, 1, 1, 1, Qt::AlignLeft);
	mGrid->setRowMinimumHeight(row, 20);

	row = mGrid->rowCount();
	mGrid->addWidget(mDescription, row, 0, 1, 1, Qt::AlignLeft);

	mHBoxDescription->addWidget(mDescriptionContent);

	mVBox->addLayout(mGrid);
	//mVBox->setSpacing(20);
	mVBox->addLayout(mHBoxDescription);
	mVBox->addLayout(mHBox);

	mDialog->setLayout(mVBox);

    QString title = tr("Project Info: ") + name;
	
	mDialog->setWindowTitle(title);

    mDialog->setMinimumWidth(350);
    mDialog->adjustSize();
	mDialog->installEventFilter(this);

	mCurrentKeyword = keyword;
	mCurrentUserName = userName;
	mCurrentAffiliation = affiliation;
	mCurrentDescription = description;

}

void ProjectInfoDialog::hide()
{
	bool isHide = saveNotification();
	if (isHide)
		mDialog->hide();
}

void ProjectInfoDialog::edit()
{
	editButton->setEnabled(false);
	saveButton->setEnabled(true);

	QString text = mUserNameContent->text();
	mGrid->removeWidget(mUserNameContent);
	delete mUserNameContent;
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(200);
	mUserNameLineEdit->setText(text);
	mUserName->setBuddy(mUserNameLineEdit);
	mGrid->addWidget(mUserNameLineEdit, 3, 1, 1, 1, Qt::AlignLeft);

	text = mKeywordContent->text();
	mGrid->removeWidget(mKeywordContent);
	delete mKeywordContent;
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(200);
	mKeywordLineEdit->setText(text);
	mKeyword->setBuddy(mKeywordLineEdit);
	mGrid->addWidget(mKeywordLineEdit, 4, 1, 1, 1, Qt::AlignLeft);

	text = mAffiliationContent->text();
	mGrid->removeWidget(mAffiliationContent);
	delete mAffiliationContent;
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(200);
	mAffiliationLineEdit->setText(text);
	mAffiliation->setBuddy(mAffiliationLineEdit);
	mGrid->addWidget(mAffiliationLineEdit, 5, 1, 1, 1, Qt::AlignLeft);

	text = mDescriptionContent->text();
	mHBoxDescription->removeWidget(mDescriptionContent);
	delete mDescriptionContent;
	mDescriptionText = new QTextEdit();
	mDescriptionText->setText(text);
	mDescriptionText->setFixedHeight(150);
	mHBoxDescription->addWidget(mDescriptionText);
}

void ProjectInfoDialog::save()
{
	editButton->setEnabled(true);
	saveButton->setEnabled(false);

	mCurrentUserName = mUserNameLineEdit->text();
	mGrid->removeWidget(mUserNameLineEdit);
	delete mUserNameLineEdit;
	mUserNameContent = new QLabel(mCurrentUserName);
	mUserName->setBuddy(mUserNameContent);
	mGrid->addWidget(mUserNameContent, 3, 1, 1, 1, Qt::AlignLeft);

	mCurrentKeyword = mKeywordLineEdit->text();
	mGrid->removeWidget(mKeywordLineEdit);
	delete mKeywordLineEdit;
	mKeywordContent = new QLabel(mCurrentKeyword);
	mKeyword->setBuddy(mKeywordContent);
	mGrid->addWidget(mKeywordContent, 4, 1, 1, 1, Qt::AlignLeft);

	mCurrentAffiliation = mAffiliationLineEdit->text();
	mGrid->removeWidget(mAffiliationLineEdit);
	delete mAffiliationLineEdit;
	mAffiliationContent = new QLabel(mCurrentAffiliation);
	mAffiliation->setBuddy(mAffiliationContent);
	mGrid->addWidget(mAffiliationContent, 5, 1, 1, 1, Qt::AlignLeft);

	mCurrentDescription = mDescriptionText->toPlainText();
	mHBoxDescription->removeWidget(mDescriptionText);
	delete mDescriptionText;
	mDescriptionContent = new QLabel();
	mDescriptionContent->setText(mCurrentDescription);
	mDescriptionContent->setFixedHeight(150);
	mDescriptionContent->setAlignment(Qt::AlignTop);
	mHBoxDescription->addWidget(mDescriptionContent);

	emit projectInfoChanged(mCurrentUserName, mCurrentKeyword, mCurrentAffiliation, mCurrentDescription);

}

bool ProjectInfoDialog::eventFilter(QObject * obj, QEvent * e)
{
    switch (e->type())
    {
        case QEvent::Close:
        {
			bool isHide = saveNotification();
			if (!isHide)
			{
				e->ignore();
				return true;
			}
            break;
        }
        default:
            qt_noop();
    }
    return QObject::eventFilter(obj, e);
}

bool ProjectInfoDialog::saveNotification()
{
	if (saveButton->isEnabled() && !mDialog->isHidden())
	{
		if (mCurrentUserName != mUserNameLineEdit->text() || mCurrentKeyword != mKeywordLineEdit->text() || mCurrentAffiliation != mAffiliationLineEdit->text() 
			|| mCurrentDescription != mDescriptionText->toPlainText())
		{
			int ret = QMessageBox::warning(this, tr("Warning"),
				   tr("Current Project Info has unsaved changes. Do you want to save before closing this window?"),
				   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
				   QMessageBox::Save);
			if(ret == QMessageBox::Cancel) 
			{
				return false;
			}
			else if (ret == QMessageBox::Save)
			{
				save();
			}
		}
	}
	return true;
}


OpenWindowDialog::OpenWindowDialog()
{
	mDialog = new QDialog();
	mVBox = new QVBoxLayout();
	mGrid = new QGridLayout();
	mDialog->setWindowTitle(tr("Open Window"));
	mInstruction = new QLabel(tr("Please select the closed window to open"));

	mOkButton = new QPushButton("OK");
	mCancelButton = new QPushButton("Cancel");
	mOkButton->adjustSize();
	mCancelButton->adjustSize();
	
	connect(mOkButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	mTreeWidget = new QTreeWidget();
	mTreeWidget->setColumnCount(1);
	mTreeWidget->setColumnWidth(0, 250);

	QStringList ColumnNames;
	ColumnNames << "Object" ;
	mTreeWidget->setHeaderLabels(ColumnNames);
	mTreeWidget->setSortingEnabled(true);
	mTreeWidget->setSelectionMode(QAbstractItemView::MultiSelection);

	mGrid->addWidget(mOkButton, 0, 3, 1, 2,  Qt::AlignVCenter | Qt::AlignRight);
	mGrid->addWidget(mCancelButton, 0, 3, 1, 3, Qt::AlignVCenter | Qt::AlignRight);
	
	mVBox->addWidget(mInstruction);
	mVBox->addWidget(mTreeWidget);
	mVBox->addLayout(mGrid);
	mDialog->setLayout(mVBox);
}

void OpenWindowDialog::addItem(QString name)
{
	QStringList list;
	list.append(name);
	QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
	mItems.append(item);
	mTreeWidget->addTopLevelItem(item);
}

void OpenWindowDialog::ok()
{
	QList<QTreeWidgetItem*> selected = mTreeWidget->selectedItems();
	foreach(QTreeWidgetItem* item, selected)
	{
		mSelection.append(item->text(0));
	}
	mDialog->close();
}

void OpenWindowDialog::cancel()
{
	mDialog->close();
}

RemoveObjectDialog::RemoveObjectDialog()
{
	mDialog = new QDialog();
	mVBox = new QVBoxLayout();
	mGrid = new QGridLayout();
	mDialog->setWindowTitle(tr("Remove Object"));
	mInstruction = new QLabel(tr("Please select the obejct to be removed"));

	mOkButton = new QPushButton("OK");
	mCancelButton = new QPushButton("Cancel");
	mOkButton->adjustSize();
	mCancelButton->adjustSize();
	
	connect(mOkButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	mTreeWidget = new QTreeWidget();
	mTreeWidget->setColumnCount(1);
	mTreeWidget->setColumnWidth(0, 250);

	QStringList ColumnNames;
	ColumnNames << "Object" ;
	mTreeWidget->setHeaderLabels(ColumnNames);
	mTreeWidget->setSortingEnabled(true);
	mTreeWidget->setSelectionMode(QAbstractItemView::MultiSelection);

	mGrid->addWidget(mOkButton, 0, 3, 1, 2,  Qt::AlignVCenter | Qt::AlignRight);
	mGrid->addWidget(mCancelButton, 0, 3, 1, 3, Qt::AlignVCenter | Qt::AlignRight);
	
	mVBox->addWidget(mInstruction);
	mVBox->addWidget(mTreeWidget);
	mVBox->addLayout(mGrid);
	mDialog->setLayout(mVBox);
	isExport = false;
	mPath = QString("");
}

void RemoveObjectDialog::addItem(QString name)
{
	QStringList list;
	list.append(name);
	QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)0, list);
	mItems.append(item);
	mTreeWidget->addTopLevelItem(item);
}

void RemoveObjectDialog::ok()
{
	int ret = QMessageBox::warning(this, tr("Warning"),
                               tr("Do you want to save the selected object into other place before removing it? "),
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                               QMessageBox::Yes);
	if(ret == QMessageBox::Cancel) return;
	else
	{
		QList<QTreeWidgetItem*> selected = mTreeWidget->selectedItems();
		foreach(QTreeWidgetItem* item, selected)
		{
			mSelection.append(item->text(0));
		}
		if (ret == QMessageBox::Yes)
		{
			mPath = QFileDialog::getExistingDirectory(this, tr("Export Location"), QString(),
													   QFileDialog::ShowDirsOnly
													   | QFileDialog::DontResolveSymlinks);
			isExport = true;
		}
	}
	mDialog->close();
}

void RemoveObjectDialog::cancel()
{
	mDialog->close();
}

SaveProjectAsDialog::SaveProjectAsDialog(const QString user, const QString keyword, const QString affiliation, const QString description, const QString path)
{
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();

	mLastPath = path;

	mDialog->setWindowTitle(tr("Save Project As.."));

	mUserNameLabel = new QLabel(tr("User Name:"));
	mUserNameLineEdit = new QLineEdit();
	mUserNameLineEdit->setFixedWidth(350);
	mUserNameLineEdit->setText(user);
    mUserNameLabel->setBuddy(mUserNameLineEdit);
	mUserNameHBox = new QHBoxLayout();
	mUserNameHBox->addWidget(mUserNameLabel);
	mUserNameHBox->addWidget(mUserNameLineEdit, 0 , Qt::AlignRight);

    mProjectNameLabel = new QLabel(tr("Name:  "));
	mProjectNameEdit = new QLineEdit();
	mProjectNameEdit->setFixedWidth(350);
    mProjectNameLabel->setBuddy(mProjectNameEdit);
	connect(mProjectNameEdit, SIGNAL(textChanged(QString)), this, SLOT(projectNameChanged(QString)));
	mProjectHBox = new QHBoxLayout();
	mProjectHBox->addWidget(mProjectNameLabel, 0 , Qt::AlignLeft);
    mProjectHBox->addWidget(mProjectNameEdit, 0 , Qt::AlignRight);
	
    mLocationLabel = new QLabel(tr("Location:"));
	mLocationLineEdit = new QLineEdit();
    mLocationLabel->setBuddy(mLocationLineEdit);
	connect(mLocationLineEdit, SIGNAL(textChanged(QString)), this, SLOT(projectPathChanged(QString)));
	mLocationLineEdit->setText(QCoreApplication::applicationDirPath());
	mProjectPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
	mLocationLineEdit->setFixedWidth(270);
	mLocationBrowseButton = new QPushButton("Browse");
	connect(mLocationBrowseButton, SIGNAL(clicked()), this, SLOT(locationBrowse()));
	mLocationHBox = new QHBoxLayout();
	mLocationHBox->addWidget(mLocationLabel);
	mLocationHBox->addWidget(mLocationLineEdit, 0 , Qt::AlignRight);
    mLocationHBox->addWidget(mLocationBrowseButton);

	mKeywordLabel = new QLabel(tr("Keywords:"));
	mKeywordLineEdit = new QLineEdit();
	mKeywordLineEdit->setFixedWidth(350);
	mKeywordLineEdit->setText(keyword);
    mKeywordLabel->setBuddy(mKeywordLineEdit);
	mKeywordHBox = new QHBoxLayout();
	mKeywordHBox->addWidget(mKeywordLabel);
    mKeywordHBox->addWidget(mKeywordLineEdit, 0 , Qt::AlignRight);

	mAffiliationLabel = new QLabel(tr("Affiliation:"));
	mAffiliationLineEdit = new QLineEdit();
	mAffiliationLineEdit->setFixedWidth(350);
	mAffiliationLineEdit->setText(affiliation);
    mAffiliationLabel->setBuddy(mAffiliationLineEdit);
	mAffiliationHBox = new QHBoxLayout();
	mAffiliationHBox->addWidget(mAffiliationLabel);
    mAffiliationHBox->addWidget(mAffiliationLineEdit, 0 , Qt::AlignRight);

	mDescriptionLabel = new QLabel(tr("Description:"));
	mDescriptionEdit = new QTextEdit(mDialog);
	mDescriptionEdit->setText(description);

	mButtonGridBox = new QGridLayout();
	mNextButton = new QPushButton("Next");
	mNextButton->setEnabled(false);
	connect(mNextButton, SIGNAL(clicked()), this, SLOT(next()));
	mCancelButton = new QPushButton("Cancel");
	connect(mCancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	mButtonGridBox->addWidget(mNextButton, 0, 5, 1, 4,  Qt::AlignVCenter | Qt::AlignRight);
	mButtonGridBox->addWidget(mCancelButton, 0, 5, 1, 5, Qt::AlignVCenter | Qt::AlignRight);
	

	mVbox->addLayout(mProjectHBox);
	mVbox->addLayout(mLocationHBox);
	mVbox->addLayout(mUserNameHBox);
	mVbox->addLayout(mKeywordHBox);
	mVbox->addLayout(mAffiliationHBox);
	mVbox->addWidget(mDescriptionLabel);
	mVbox->addWidget(mDescriptionEdit);
	mVbox->addLayout(mButtonGridBox);
	
	connect(this, SIGNAL(infoUpdate()), this, SLOT(enableNextButton()));
	mDialog->setMinimumWidth(350);
	mDialog->setLayout(mVbox);
	isOk = false;
}

void SaveProjectAsDialog::locationBrowse()
{
	
	mProjectPath = QFileDialog::getExistingDirectory(this, tr("Location"), mLastPath,
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
	mLocationLineEdit->setText(mProjectPath);
	mLocationLineEdit->displayText();
}


void SaveProjectAsDialog::next()
{
	mFullPath = mProjectPath.simplified();
	if (!QDir().exists(mProjectPath))
	{
		qDebug()<<"the path do not exist";
		QString notice;
		notice.append(QString("Error!\nThe directory ") + mProjectPath + QString(" does not exist!"));
		//mError = new ErrorDialog(notice);
		QMessageBox::critical(this, tr("Object Opening Error"), "Unable to open " + mProjectPath + ".");
		return;
	}
	if (mFullPath.split(QDir::separator())[1] == QString())
	{
		mFullPath.append(mProjectName);
	}
	else
	{
		mFullPath.append(QDir::separator() + mProjectName);
	}
	if (QDir().exists(mFullPath))
	{
		QString message("The project with specified name already exists in the specified location!\n");
		message.append(QString("Do you want to overwrite the existing project with this new project?"));

		QMessageBox::StandardButton reply;
		reply = QMessageBox::warning(this, tr("WARNING"), message, QMessageBox::Ok|QMessageBox::No);
		if (reply == QMessageBox::Ok) 
		{
			nextReady();
		}
		else 
		{
			nextCancel();
		}
	}
	else
	{
		nextReady();
	}
}

void SaveProjectAsDialog::nextReady()
{
	isOk = true;
	mDialog->hide();
	
}

void SaveProjectAsDialog::nextCancel()
{
	mProjectNameEdit->clear();
}


void SaveProjectAsDialog::cancel()
{
	mDialog->hide();
}

void SaveProjectAsDialog::projectNameChanged(QString name)
{
	mProjectName = name;
	emit infoUpdate();
}

void SaveProjectAsDialog::projectPathChanged(QString path)
{
	mProjectPath = path;
	emit infoUpdate();
}

void SaveProjectAsDialog::enableNextButton()
{
	if (!mProjectPath.isEmpty() && !mProjectName.isEmpty())
	{
		mNextButton->setEnabled(true);
	}
	else
	{
		mNextButton->setEnabled(false);
	}
}
