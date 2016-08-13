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


#include "loadingdlg.h"
#include <QApplication>

QLabel *LoadingDlg::info = 0;
QProgressBar *LoadingDlg::progress = 0;

LoadingDlg::LoadingDlg(QWidget *parent) : QDialog(parent)
{
	info = new QLabel(this);
	info->setText("Loading...");
	info->setFixedWidth(150);
	progress = new QProgressBar(this);
	progress->setFixedWidth(150);
	progress->setMinimum(0);
	progress->setMaximum(100);
	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(info);
	layout->addWidget(progress);
	setLayout(layout);
	setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle("Loading...");
	setModal(true);
}


bool LoadingDlg::QCallBack(int pos, QString str)
{
	if(pos==-1) return true;
	info->setText(str);
	progress->show();
	progress->setEnabled(true);
	progress->setValue(pos);
	progress->update();
	info->update();
	QApplication::processEvents();
	return true;
}