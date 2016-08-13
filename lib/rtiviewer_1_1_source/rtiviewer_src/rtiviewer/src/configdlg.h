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


#ifndef CONFIG_DLG_H
#define CONFIG_DLG_H

#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>

//! Configuration dialog.
/*!
  The class defines the dialog to modify the application settings.
*/
class ConfigDlg : public QDialog
{
	Q_OBJECT

private: 

	QSpinBox* widthSpinBox; /*!< Spinbox to set the width of the browser. */
	QSpinBox* heightSpinBox; /*!< Spinbox ro set the height of the browser. */
	QCheckBox* fullSizeCkb; /*!< Checkbox to select the full-size. */ 

public:
	
	//! Constructor
	/*!
	\param currentW current width of the browser.
	\param currentH current height of the browser.
	\param maxBrowserSize max size for the browser.
	\param parent
	*/
	ConfigDlg(int currentW, int currentH, const QSize& maxBrowserSize, QWidget* parent = 0)
		: QDialog (parent)
	{
		QVBoxLayout* layout = new QVBoxLayout;

		QGroupBox* groupBox = new QGroupBox("Browser Size", this);
		QGridLayout* groupLayout = new QGridLayout;
		QLabel* label1 = new QLabel("Width");
		QLabel* label2 = new QLabel("Height");
		widthSpinBox = new QSpinBox(groupBox);
		widthSpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
		widthSpinBox->setKeyboardTracking(true);
		widthSpinBox->setMinimum(512);
		widthSpinBox->setMaximum(maxBrowserSize.width());
		widthSpinBox->setValue(currentW);

		heightSpinBox = new QSpinBox(groupBox);
		heightSpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
		heightSpinBox->setKeyboardTracking(true);
		heightSpinBox->setMinimum(512);
		heightSpinBox->setMaximum(maxBrowserSize.height());
		heightSpinBox->setValue(currentH);

		fullSizeCkb = new QCheckBox("Full size", groupBox);

		groupLayout->setColumnStretch(0, 1);
		groupLayout->setColumnStretch(1, 2);
		groupLayout->addWidget(label1, 0, 0, 1, 1);
		groupLayout->addWidget(widthSpinBox, 0, 1, 1, 1);
		groupLayout->addWidget(label2, 1, 0, 1, 1);
		groupLayout->addWidget(heightSpinBox, 1, 1, 1, 1);
		groupLayout->addWidget(fullSizeCkb, 2, 1, 1, 1);
		groupBox->setLayout(groupLayout);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(groupBox);
		buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

		layout->addWidget(groupBox);
		layout->addWidget(buttonBox);
		setLayout(layout);
		
		setMinimumSize(240, 160);
		setMaximumSize(300, 200);

		connect(fullSizeCkb, SIGNAL(stateChanged(int)), this, SLOT(setFullSize(int)));
	};

	/*!
	  Returns the size readed from the spinboxes.
	*/
	QSize getCurrentSize()
	{
		return QSize(widthSpinBox->value(), heightSpinBox->value());
	};

private slots:

	/*!
	  Sets the full-size.
	*/
	void setFullSize(int state)
	{
		if (state == 0)
		{
			widthSpinBox->setEnabled(true);
			heightSpinBox->setEnabled(true);
		}
		else
		{
			widthSpinBox->setValue(widthSpinBox->maximum());
			heightSpinBox->setValue(heightSpinBox->maximum());
			widthSpinBox->setEnabled(false);
			heightSpinBox->setEnabled(false);
		}
	};

};


#endif //CONFIG_DLG_H
