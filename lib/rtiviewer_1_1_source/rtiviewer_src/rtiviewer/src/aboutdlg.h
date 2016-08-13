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

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QString>
#include <QWidget>

/*====================================================================
 *
 * AboutDlg class header
 *
 *===================================================================*/

//! AboutDlg class.
/*!
  The AboutDlg class displays a dialog with information about RTIViewer.
*/

class AboutDlg : public QDialog
{
    Q_OBJECT

public:

    /*====================================================================
     * Public methods
     *===================================================================*/

    //! Constructor
    /*!
      Constructs an About dialog.
    */

    AboutDlg(QWidget *parent = 0);

    //! Translate the strings in the dialog.
    /*!
      Translates the strings in the dialog. For more information, see QApplication::translate.
    */

    void retranslateUi();
    
private slots:
    
    /*====================================================================
     * Private slots
     *===================================================================*/

    void showManual(QString file);
    void showURL(QString url);

private:

    /*====================================================================
     * Private methods to lay out the dialog
     *===================================================================*/

    void initAbout();
    void initLayout();
    QLayout * initInfoLayout();
    QLayout * initTitleLayout();
    QLayout * initUserGuideLayout();
    QLayout * initLogosLayout();
    QLayout * initUniversityLayout();
    QLayout * initMuseumLayout();
    QLayout * initHPButtonLayout();
    QLayout * initButtonLayout();

    /*====================================================================
     * Private methods to construct strings
     *===================================================================*/

    char * getAboutDialogTitle();
    char * getTitleHTML();
    char * getGplHTML();
    char * getGuideHTML();
    char * getDescHTML();

    /*====================================================================
     * Private method to connect signals and slots
     *===================================================================*/

    void setConnections();

    /*====================================================================
     * Private data members
     *===================================================================*/

    QLabel * titleLabel;
    QLabel * gplLabel;
    QLabel * guideLabel;
    QLabel * descLabel;
    QPushButton * okButton;
};

#endif // ABOUTDLG_H
