/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/


#ifndef OPENREMOTEDLG_H
#define OPENREMOTEDLG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QUrl>

//! Remote dialog class.
/*!
  The class defines a dialog to insert the url of a remote RTI image.
*/
class OpenRemoteDlg: public QDialog 
{

Q_OBJECT

// private data members
private:
	
	QLineEdit* input; /*!< Input box. */
	QPushButton* okBtn; /*!< Ok button. */
	QPushButton* cancelBtn; /*!< Cancel button. */
	QDialogButtonBox* buttonBox;

	QUrl& url; /*!< Reference to the url. */

public:

	//! Constructor.
	/*!
	  \param u reference to the url.
	  \param parent
	*/
	OpenRemoteDlg(QUrl& u, QWidget *parent=0);

// public Qt slots
public slots:

	/*!
	  Invoked when the user presses the OK button.
	*/
	void okPressed();
};

#endif /* OPENREMOTEDLG_H */
