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

#ifndef CHE_BASIC_INFO_H
#define CHE_BASIC_INFO_H

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
#include <QTreeWidget>
#include <QTreeWidgetItem>

/**
 * This class is the base for all CHE information classes.
 */
class CHEInfoBasic : public QWidget
{
	Q_OBJECT

public:
	// Constructor.
	CHEInfoBasic(QWidget *parent = 0);

public:
	QString object, measurement, creation, material, description, conservation,
		analyses, related, administration, documents, other;

};

/**
 * This class is dialog for user to fill in CHE information when create a new CHE.
 */
class CHEInfoDialog : public CHEInfoBasic
{
	Q_OBJECT

public:
	// Constructor.
	CHEInfoDialog(QWidget *parent = 0);

protected:
	QDialog* mDialog;
	QGridLayout* mGrid;
	QVBoxLayout* mVBox;
	QHBoxLayout* mHBox;
	QHBoxLayout* mHBoxDescription;
	QLabel *mObject, *mMeasurement, *mCreation, *mMaterial, *mDescription, *mConservation,
		*mAnalyses, *mRelated, *mAdministration, *mDocuments, *mOther;
	QLineEdit *mObjectContent, *mMeasurementContent, *mCreationContent, *mMaterialContent, 
		*mDescriptionContent, *mConservationContent, *mAnalysesContent, *mRelatedContent,
		*mAdministrationContent, *mDocumentsContent, *mOtherContent;
};

#endif // CHE_BASIC_INFO_H