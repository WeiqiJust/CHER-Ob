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

/**
 * This class is the dialog to input CHE info. It is shown after user choose Next button in
 * newCHEDialog.
 */

class NewCHEInfoDialog : public CHEInfoDialog
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 */
	NewCHEInfoDialog();

	/**
	 * @brief  Show the dialog. Overload from QWidget.
	 */
	void exec()	{mDialog->exec();}

	/**
	 * @brief  Hide the dialog. Overload from QWidget.
	 */
	void hide() {mDialog->hide();}

private slots:
	/**
	 * @brief  Handle Ok button.
	 */
	void okPressed();

	/**
	 * @brief  Handle Back button.
	 */
	void backPressed();

	/**
	 * @brief  Handle Cancel button.
	 */
	void cancelPressed(){emit cancel();}

signals:
	/**
	 * @brief  Emit signal to newCHEDialog when Ok button is pressed.
	 */
	void ok(const CHEInfoBasic* info);

	/**
	 * @brief  Emit signal to newCHEDialog when Back button is pressed.
	 */
	void back();

	/**
	 * @brief  Emit signal to newCHEDialog when Cancel button is pressed.
	 */
	void cancel();

private:
	QPushButton* okButton;
	QPushButton* backButton;
	QPushButton* cancelButton;
};

#endif // NEW_CHE_INFO_DIALOG_H
