/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Zeyu Wang (zeyu.wang@yale.edu)

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

#ifndef VIDEO_FILTER_H
#define VIDEO_FILTER_H

#include <QtScript>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QTextBrowser>
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

/**
 * This class is the dialog help user to select contents that would appear
 * in video.
 */
class VideoFilter : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  The vector of all the objcts' absolute path.
	 */
	VideoFilter(QVector<QString> objects);

	/**
	 * @brief  Get the objects that should appear in the video.
	 * @return The vector of the selected object full path.
	 */
	QVector<QString> getFilterList();

	/**
	 * @brief  Get the CHE categories of the given object.
	 * @param  object  The object absolute path.
	 * @return The vector of categories encoded as int id.
	 */
	QVector<int> getCategories(const QString object);

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

	/**
	 * @brief  Test whether the dialog should be processed or canceled.
	 * @return If Generate button is pressed then return true, otherwise return false.
	 */
	bool checkGenerate() {return isGenerate;}

private slots:
	/**
	 * @brief  Handle Generate button and start to generate the report.
	 */
	void generate();

	/**
	 * @brief  Handle Cancel button.
	 */
	void cancel();

	/**
	 * @brief  Handle SelectAll button.
	 */
	void selectAll();

	/**
	 * @brief  Handle the change of check state of items in tree widget.
	 *         If the object item is checked, then all its child (category items) should all be checked.
	 *         If the object item is unchecked, then all its child (category items) should all be unchecked.
	 * @param  item    item that is checked.
	 * @param  column  checked column number to match the signal, useless here.
	 */
	void itemChanged(QTreeWidgetItem * item, int column);

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;

	QGridLayout* mButtonGridBox;

	QPushButton* mGenerateButton;
	QPushButton* mCancelButton;
	QPushButton* mSelectAllButton;

	QLabel *mLabel;

	QTreeWidget* mTreeWidget;
	QList<QTreeWidgetItem *> mItems;
	QVector<QString> mObject;
	QMap<QString, QVector<int> > mSelected;
	bool isGenerate;

};

#endif // VIDEO_FILTER