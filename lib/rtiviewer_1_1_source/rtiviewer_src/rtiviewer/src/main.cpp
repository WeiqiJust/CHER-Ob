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


// Local headers
#include "gui.h"

// Qt headers
#include <QApplication>
#include <QObject>
#include <QString>
#include <QThread>

#include <omp.h>
#include "util.h"
#include "SysInfo.h"

int main( int argc, char ** argv )
{
    QApplication app( argc, argv );


#if _MSC_VER || __MINGW32__
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	printf ("There are %d total Mbytes of virtual memory.\n", statex.ullTotalVirtual/(1024*1024));
	printf ("There are %d free Mbytes of virtual memory.\n", statex.ullAvailVirtual/(1024*1024));
#endif
#if _MSC_VER
	omp_set_num_threads(getProcessorCount());
#endif
    RtiViewerDlg *maindlg = new RtiViewerDlg();
    maindlg->show();
	FileOpenEater *filterObj = new FileOpenEater(maindlg);
	app.installEventFilter(filterObj);
	app.processEvents();
	if (argc > 1)
		maindlg->openFile(QString(argv[1]));
	return app.exec();
}
