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

#ifndef SEARCH_H
#define SEARCH_H

#include <QtScript>
#include <QRegExp>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QFile>
#include <QWizard>
#include <QFileDialog>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QObject>
#include <QMessageBox>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "../vtkEnums.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"

class MainWindow;

class SearchWidget : public QWidget
{
	Q_OBJECT

public:
    SearchWidget(QWidget *parent);

	void refreshSearchTab(bool activeWindow);

	static MainWindow* mw(); // find the mainframe

private:
	bool updateCurrentPath();

	void setFilter(int mode);

private slots:
	void search();

	void showTreeWidgetItem(QTreeWidgetItem* item, int column);

	void setFilterMode(int mode);

private:
	QHBoxLayout* hlay;
	QVBoxLayout* vlay;
	QLabel* mLabel;
	QLineEdit* mInput;
	QComboBox* mFilter;
	QPushButton* mButton;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QList<QTreeWidgetItem *> mFilteredItems;
	QString mPath;
	QString mFile;
	int mMode;
};

class SearchAllWidget : public QWidget
{
	Q_OBJECT

public:
    SearchAllWidget(QWidget *parent);

	void refreshSearchTab(bool activeWindow);

	static MainWindow* mw(); // find the mainframe

private:
	bool updateCurrentPath();

	QStringList matchString(QString content, QString matchStr);

	void setFilter(int mode);

private slots:
	void search();

	void showTreeWidgetItem(QTreeWidgetItem* item, int column);

	void setFilterMode(int mode);

private:
	QHBoxLayout* hlay;
	QVBoxLayout* vlay;
	QLabel* mLabel;
	QLineEdit* mInput;
	QPushButton* mButton;
	QComboBox* mFilter;
	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QList<QTreeWidgetItem *> mFilteredItems;
	QString mPath;
	QString mFile;
	int mMode;
};

#endif // SEARCH_H