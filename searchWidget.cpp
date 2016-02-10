/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

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
   anonymous help by various software engineering communities.

*****************************************************************************/
#include <QMouseEvent>

#include "searchWidget.h"
#include "mainWindow.h"

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
{
	//int width = this->frameGeometry().width();
	//int height = this->frameGeometry().height();

	mLabel = new QLabel(tr("Please input the keyword to search in current object files"));
	mInput = new QLineEdit();
	mInput->setFixedWidth(200);
    mLabel->setBuddy(mInput);
	mButton = new QPushButton("Search");
	mTreeWidget = new QTreeWidget();
	mTreeWidget->setColumnCount(1);
	connect(mButton, SIGNAL(clicked()), this, SLOT(search()));
	
	hlay = new QHBoxLayout();
	vlay = new QVBoxLayout();
	hlay->addWidget(mInput);
	hlay->addWidget(mButton);
	vlay->addWidget(mLabel);
	vlay->addLayout(hlay);
	vlay->addWidget(mTreeWidget);
	
	this->setLayout(vlay);
	this->show();
}

bool SearchWidget::search()
{
	return false;
}