/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

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
#ifndef IMAGE_NOTE_H
#define IMAGE_NOTE_H
#include <QtScript>
#include <QDir>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QTextBrowser>
#include <QLabel>
#include <QFile>
#include <QFileDialog>

/**
 * This class is designed to save attached images of notes.
 */
class ImageNote: public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor.
	 * Path is the absolute location of the object where the image attachs.
	 * File is the image data. If file is not specified (empty string), then user
	 * need to specify the image file.
	 * Id is the image id starting from 0.
	 */
	ImageNote(const QString path, const QString file, const int id);

	/**
	 * Get the image location.
	 */
	QString getPath()	{return mPath;}

	/**
	 * Get the remove button so that it can be added to the note dialog layout.
	 */
	QPushButton* getButton()	{return mRemoveButton;}

	/**
	 * Get the image file which is saved in QLabel.
	 */
	QLabel* getImage()	{return mImage;}

	/**
	 * Get the name of the image.
	 */
	QLabel* getName()	{return mName;}

	/**
	 * Provide API to set image id.
	 */
	void setId(const int id)	{mId = id;}

signals:
	/**
	 * Send signal when the image is removed by remove button.
	 */
	void removeImage(int id);

private slots:
	/**
	 * Handle remove button.
	 */
	void remove();

private:
	int mId;
	QString mPath;
	QPushButton* mRemoveButton;
	QLabel* mImage;
	QLabel* mName;
};

#endif // IMAGE_NOTE_H