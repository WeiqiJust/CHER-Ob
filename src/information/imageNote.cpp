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
   annonymous help by various software engineering communities.

*****************************************************************************/
#include "imageNote.h"


ImageNote::ImageNote(const QString path, const QString file, const int id)
{
	mPath = QString();
	QString folder = path;
	folder = QDir::toNativeSeparators(folder);
	folder.append(QDir::separator() + QString("images"));
	QDir().mkdir(folder);	// if folder exists, do nothing
	QDir dir(folder);

	QString fileName;
	if (file == QString())
	{
		QStringList filters;
		filters.push_back("Images (*.png *.jpg *.jpeg *.tif *.bmp)");
		filters.push_back("*.png");
		filters.push_back("*.jpg");
		filters.push_back("*.jpeg");
		filters.push_back("*.tif");
		filters.push_back("*.bmp");
		fileName = QFileDialog::getOpenFileName(this,tr("Open an image"), QString(), filters.join(";;"));
		if (fileName == QString())
			return;
	}
	else
	{
		fileName = file;
	}

	mId = id;
	fileName = QDir::toNativeSeparators(fileName);
	QStringList nameElements = fileName.split(QDir::separator());
	mName = new QLabel(nameElements[nameElements.size() - 1]);

	mPath = folder + QDir::separator() + nameElements[nameElements.size() - 1];
    QFile::copy(fileName, mPath);
	//qDebug()<<mPath;
	QPixmap image(mPath);
	mImage = new QLabel();
	mImage->setPixmap(image);
	mImage->setScaledContents(true);

	int imgNoteHeight = image.height();
	int imgNoteWidth = image.width();
	if (imgNoteHeight == 0 || imgNoteWidth == 0)
		return;
	int height, width;
	if (imgNoteWidth/imgNoteHeight >= 3)
	{	
		width = 300;
		height = (double)width * imgNoteHeight / imgNoteWidth;
	}
	else if (imgNoteWidth/imgNoteHeight >= 0.5)
	{
		width = 150;
		height = (double)width * imgNoteHeight / imgNoteWidth;
	}
	else
	{
		height = 250;
		width = (double)height * imgNoteWidth / imgNoteHeight;
	};

    mImage->setFixedSize(width, height);
	mRemoveButton = new QPushButton("Remove");
	connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(remove()));
}

void ImageNote::remove()
{
	emit removeImage(mId);
}