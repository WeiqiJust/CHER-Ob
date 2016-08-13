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


#include "openremotedlg.h"

#include <QGridLayout>
#include <QSpacerItem>
#include <QMessageBox>

OpenRemoteDlg::OpenRemoteDlg(QUrl& u, QWidget *parent) : QDialog(parent),
	url(u)
{
	QLabel* info = new QLabel("Insert the url of the file to load");
	QLabel* label = new QLabel("URL");
	if (url.isEmpty())
		input = new QLineEdit("http://");
	else
		input = new QLineEdit(url.toString());
	input->setMinimumWidth(500);
	
	okBtn = new QPushButton("OK");
	okBtn->setDefault(true);
	cancelBtn = new QPushButton("Cancel");
	cancelBtn->setAutoDefault(false);

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(okBtn, QDialogButtonBox::ActionRole);
	buttonBox->addButton(cancelBtn, QDialogButtonBox::RejectRole);
	
	connect(okBtn, SIGNAL(clicked()), this, SLOT(okPressed()));
	connect(cancelBtn, SIGNAL(clicked()), this, SLOT(close()));

	QSpacerItem* spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(info, 0, 0, 1, 3, Qt::AlignLeft);
	layout->addWidget(label, 1, 0, 1, 1, Qt::AlignLeft);
	layout->addWidget(input, 1, 1, 1, 2);
	layout->addItem(spacer, 2, 1, 1, 1);
	layout->addWidget(buttonBox, 2, 2, 1, 1);
	setLayout(layout);
	
	setModal(true);
	setWindowTitle(tr("HTTP"));
	input->setFocus();
}

void OpenRemoteDlg::okPressed()
{
	QString text = input->text();
	url.setUrl(text, QUrl::StrictMode);
	if (!url.isValid())
		QMessageBox::critical(this, tr("Error"), tr("Invalid Url."));
	else
	{
		QString path = url.path();
		/*if (!path.endsWith(".rrti", Qt::CaseInsensitive))
			QMessageBox::critical(this, tr("Error"), tr("Unkwon file type. Insert a valid url\n(Example: http://host/filename.rrti)"));
		else*/
		done(QDialog::Accepted);
	}
}
