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

#ifndef FILE_INFO_DIALOG_H
#define FILE_INFO_DIALOG_H

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
#include "../vtkEnums.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"

class MainWindow;
class VtkWidget;

class FileInfoDialog : public QWidget
{
	Q_OBJECT

public:
	FileInfoDialog(VtkWidget* w = 0);

public slots:
	void hide()	{mDialog->hide();}

private:
	void get2DInfo();

	void get3DInfo();

	QString formatFilesize(qint64 bytes);

private:
	QDialog* mDialog;
	QGridLayout* grid;
	QHBoxLayout* hboxButtons;
	QLabel* openGLLabel, *coresLabel, *filenameLabel, *filesizeLabel, *filesizeLabel2, *dimensionsLabel, *dimensionsLabel2;
	QLabel* texFilenameLabel, *texFilesizeLabel, *texFilesizeLabel2, *texDimensionsLabel, *texDimensionsLabel2;
	QLabel* pointLabel, *pointLabel2, *polyLabel, *polyLabel2;
	QLineEdit* filenameLine, *texFilenameLine;
	QPushButton* okButton;
	QVBoxLayout* vbox, *vboxData;

	vtkImageData* hyperdata;
	vtkSmartPointer<vtkImageViewer2> viewer;
	vtkTexture* texture;
	VtkWidget* w;
	QFileInfo* finfo;
	QFileInfo* tfinfo;
	QLocale locale;
	QString textureFilename;
	int points, polys;
	bool isDICOM;
};
#endif // FILE_INFO_DIALOG_H