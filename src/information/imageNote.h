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

class ImageNote: public QWidget
{
	Q_OBJECT

public:
	ImageNote(const QString path, const QString file, const int id);

	QString getPath()	{return mPath;}

	QPushButton* getButton()	{return mButton;}

	QLabel* getImage()	{return mImage;}

	QLabel* getName()	{return mName;}

	void setId(const int id)	{mId = id;}

signals:
	void removeImage(int id);

private slots:
	void remove();

private:
	int mId;
	QString mPath;
	QPushButton* mButton;
	QLabel* mImage;
	QLabel* mName;
};

#endif // IMAGE_NOTE_H