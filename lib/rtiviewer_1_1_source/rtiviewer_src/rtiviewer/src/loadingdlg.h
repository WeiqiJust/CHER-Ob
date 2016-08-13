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


#ifndef LOADINGDLG_H
#define LOADINGDLG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>

//! Loading window
/*!
  The class defines and manages a window with a progress bar. 
*/
class LoadingDlg: public QDialog 
{

Q_OBJECT

// private data members
private:
	
	static QLabel* info; /*!< Label. */
	static QProgressBar* progress; /*!< Progress bar. */


public:

	//! Constructor
	LoadingDlg(QWidget *parent=0);

	/*!
	  Static method to update the label and the progress bar.
	  \param pos value of the pogress bar.
	  \param str label
	*/
	static bool QCallBack(int pos, QString str);
};

#endif /* LOADINGDLG_H */
