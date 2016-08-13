/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Ying Yang (ying.yang.yy368@yale.edu)

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

 - Acknowledgments: This EXR file reader is based on the original code of
   exrreadchannel for MATLAB by Edgar Velazquez-Armendariz.

*****************************************************************************/
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


#include "renderingdialog.h"

#include <QVBoxLayout>
#include <QDebug>

RenderingDialog::RenderingDialog(QMap<int, RenderingMode*>* l, int currRendering, QWidget *parent, bool remote) : QWidget(parent) 
{
	list = l;
	modeList = new MyComboBox(this);
	modeList->setDuplicatesEnabled(false);
	if (l)
	{
		int selected = currRendering;
		QMapIterator<int, RenderingMode*> iter(*list);
		int i = 0;
		while (iter.hasNext())
		{
			iter.next();
			if (remote)
			{
				if ((iter.value())->supportRemoteView())
				{	
					modeList->addItem((iter.value())->getTitle(), QVariant(iter.key()));
					if (currRendering == iter.key())
						selected = i;
					i++;
				}
			}
			else
			{
				modeList->addItem((iter.value())->getTitle(), QVariant(iter.key()));
				i++;
			}
		}
		modeList->setCurrentIndex(selected);
		control = list->value(currRendering)->getControl(this);
	}
	else
		control = new QWidget(this);
	control->setMinimumHeight(100);
	connect(modeList, SIGNAL(activated(int)), this, SLOT(renderingModeUpdate(int)));
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(modeList, 0, Qt::AlignTop);
	layout->addWidget(control, 2, Qt::AlignTop);
	setLayout(layout);
}


void RenderingDialog::renderingModeUpdate(int index)
{
	int idx = modeList->itemData(index).toInt();
	QWidget* c = list->value(idx)->getControl(this);
	c->setMinimumHeight(100);
	QBoxLayout* layout =(QBoxLayout*) this->layout();
	layout->removeWidget(control);
	layout->addWidget(c, 2, Qt::AlignTop);
	control->close();
	delete control;
	control = c;
	emit renderingModeChanged(idx);
	update();
}


//YY
void RenderingDialog::updateRTIRendDlg(QMap<int, RenderingMode*>* l, int currRendering)
{
	this->setRenderingMode(l, currRendering);
}



void RenderingDialog::setRenderingMode(QMap<int, RenderingMode*>* l, int currRendering, bool remote)
{
	modeList->clear();
	list = l;
	if (l)
	{
		int selected = currRendering;
		QMapIterator<int, RenderingMode*> iter(*list);
		int i = 0;
		while (iter.hasNext())
		{
			iter.next();
			if (remote)
			{
				if ((iter.value())->supportRemoteView())
				{	
					modeList->addItem((iter.value())->getTitle(), QVariant(iter.key()));
					if (currRendering == iter.key())
						selected = i;
					i++;
				}
			}
			else
			{
				modeList->addItem((iter.value())->getTitle(), QVariant(iter.key()));
				i++;
			}
		}
		modeList->setCurrentIndex(selected);
		renderingModeUpdate(selected);
	}
	else
	{
		QWidget* c = new QWidget(this);
		c->setMinimumHeight(100);
		QBoxLayout* layout =(QBoxLayout*) this->layout();
		layout->removeWidget(control);
		layout->addWidget(c, 0, Qt::AlignTop);
		delete control;
		control = c;
		update();
	}	
}

void RenderingDialog::setRenderingMode(int currRendering)
{
    int index = modeList->findData(QVariant(currRendering));
    modeList->setCurrentIndex(index);
    renderingModeUpdate(index);
}

void RenderingDialog::updateRenderingList(int currRendering, bool remote)
{
	modeList->clear();
	emit resetRemote();
	if (list)
	{
		int selected = currRendering;
		QMapIterator<int, RenderingMode*> iter(*list);
		int i = 0;
		while (iter.hasNext())
		{
			iter.next();
			if (remote)
			{
				if ((iter.value())->supportRemoteView())
				{	
					modeList->addItem((iter.value())->getTitle(), QVariant(iter.key()));
					if (currRendering == iter.key())
						selected = i;
					i++;
				}
			}
			else
			{
				modeList->addItem((iter.value())->getTitle(), QVariant(iter.key()));
				i++;
			}
		}
		modeList->setCurrentIndex(selected);
		renderingModeUpdate(selected);
	}
}

QWidget * RenderingDialog::getRenderingControl()
{
    return control;
}
