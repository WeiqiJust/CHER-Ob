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
#ifndef ANNOTATION_FILTER_DIALOG_H
#define ANNOTATION_FILTER_DIALOG_H

#include <QtScript>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFile>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QMessageBox>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class AnnotationFilterDialog : public QWidget
{
	Q_OBJECT

public:
	AnnotationFilterDialog(const QVector<QString> users);
		
	QVector<QString> getSelectedUsers();

	void exec()	{mDialog->exec();}

	bool isFilter() {return isFiltered;}

private slots:
	void filter();

	void cancel();

	void selectAll();

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;

	QGridLayout* mButtonGridBox;

	QPushButton* mFilterButton;
	QPushButton* mCancelButton;
	QPushButton* mSelectAllButton;

	QLabel *mLabel;

	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QVector<QString> mSelectedUsers;
	bool isFiltered; //filter button pressed true, otherwise false;
};

#endif