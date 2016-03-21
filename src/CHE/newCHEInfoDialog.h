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

#ifndef NEW_CHE_INFO_DIALOG_H
#define NEW_CHE_INFO_DIALOG_H

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
#include "CHEInfoDialog.h"

class NewCHEInfoDialog : public CHEInfoDialog
{
	Q_OBJECT

public:
	NewCHEInfoDialog();

	void exec()	{mDialog->exec();}

	void hide() {mDialog->hide();}

private slots:
	void okPressed();

	void backPressed();

	void cancelPressed(){emit cancel();}

signals:
	void ok(const CHEInfoBasic* info);

	void back();

	void cancel();

private:
	QPushButton* okButton;
	QPushButton* backButton;
	QPushButton* cancelButton;
};

#endif // NEW_CHE_INFO_DIALOG_H
