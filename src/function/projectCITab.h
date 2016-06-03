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

#ifndef PROJECT_CI_TAB_H
#define PROJECT_CI_TAB_H

#include <QtScript>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QComboBox>
#include <QDomDocument>

class ProjectClassifiedInfoTab : public QWidget
{
	Q_OBJECT

public:
	ProjectClassifiedInfoTab(const QString path, const QString userName, QWidget *parent = 0);

	void addCHEInfo(const QString CHEName, QVector<QString> categories, QVector<QString> contents);

	void addCHEInfoToFile(const QString CHEName);

	void removeCHEInfo(const QString CHEName);

	void clear();

private:
	void initFile(const QString userName);

	void loadCHEInfo();

private slots:
	void setCHEFilterMode(int mode);

	void setCategoryFilterMode(int mode);

private:
	QMap<QString, QVector<QPair<QString, QString> > > mClassifiedInfo;
	QComboBox *mCHEFilter, *mCategoryFilter;
	QLabel *mCHELabel, *mCategoryLabel;
	QVBoxLayout* mVBox;
	QGridLayout* mGrid;
	QTextEdit* mTextEdit;
	QFile* mFile;
	QString mPath;
	QDomDocument mDoc;
};


#endif
