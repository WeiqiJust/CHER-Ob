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

/**
 * This class is the tab in project mode to show the text information if there are objects imported
 * from CHE. User can choose specific category from specific CHE.
 */

class ProjectClassifiedInfoTab : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  path      Project folder path.
	 * @param  userName  The current user of the project.
	 */
	ProjectClassifiedInfoTab(const QString path, const QString userName, QWidget *parent = 0);

	/**
	 * @brief  Add the information of the specific CHE.
	 * @param  CHEName     The name of the added CHE.
	 * @param  categories  The vector of categories that the CHE contains.
	 * @param  contents    The vector of the info content of the categories.
	 */
	void addCHEInfo(const QString CHEName, QVector<QString> categories, QVector<QString> contents);

	/**
	 * @brief  Write the information of specific CHE into an project/information.xml file.
	 * @param  CHEName  The name of the CHE.
	 */
	void addCHEInfoToFile(const QString CHEName);

	/**
	 * @brief  Remove the information of specific CHE from the project/information.xml file.
	 * @param  CHEName  The name of the CHE.
	 */
	void removeCHEInfo(const QString CHEName);

	/**
	 * @brief  Remove all the information cached in the tab.
	 */
	void clear();

private:
	/**
	 * @brief  Initiate the project/information.xml by writting the basic information.
	 * @param  userName  The current user of the project.
	 */
	void initFile(const QString userName);

	/**
	 * @brief  Open project/information.xml file and load the content into the tab. 
	 */
	void loadCHEInfo();

private slots:
	/**
	 * @brief  Handle the CHEFilter combobox when user select a CHE.
	 * @param  mode  The int value of selected item.
	 */
	void setCHEFilterMode(int mode);

	/**
	 * @brief  Handle the CategoryFilter combobox when user select a category.
	 * @param  mode  The int value of selected item.
	 */
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
