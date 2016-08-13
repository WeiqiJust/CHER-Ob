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
#include "rti.h"
#include "ptm.h"
#include "hsh.h"
#include "universalrti.h"
#include "zorder.h"
#include <jpeg2000.h>

// Standard headers
#include <iostream>

// Qt headers
#include <qapplication.h>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QImage>
#include <QColor>
#include <QtXml/QDomDocument>

using std::cout;
using std::endl;

int main( int argc, char ** argv )
{
	QApplication app( argc, argv );

#if defined(Q_OS_MAC)
        QString appDir = QCoreApplication::applicationDirPath();
        QCoreApplication::addLibraryPath(appDir + QString("/../PlugIns"));
#endif

	if (argc < 2)
	{
		cout << std::endl << "Usage:" << endl << endl;
		cout << "        rtiwebmaker <input> <level>" << endl << endl;
		cout << "        <input>  RTI file to decompose (LRGB-PTM, HSH)." << endl;
		cout << "        <level>  Levels of resolution (optional) (default: 3)." << endl;
		exit(0);
	}

	// parse arguments
	QString filename;
	int level;
	if (argc == 2)
	{
		QString str1(argv[1]);
		filename = str1;
		level = 3;
	}
	else
	{
		QString str1(argv[1]);
		QString str2(argv[2]);
		filename = str1;
		level = str2.toInt();
	}
	
	QFileInfo fi(filename);

	// load rti image
	//////////////////////////////////////////////////////////////

	Rti *image;
	if (fi.suffix() == "ptm")
	{
		QFile data(filename);
		if (!data.open(QFile::ReadOnly))
		{
			cout << "I/0 error." << endl;
			exit(0);
		}
		QTextStream input(&data);
		image = Ptm::getPtm(input);
		data.close();
		//LRGBPtm *ptm;
		if (dynamic_cast<RGBPtm*>(image))
		{
			cout << "Unsupported file format. The tool accepts LRGB-PTM or HSH file." << endl;
			exit(0);
		}
	}
	else if (fi.suffix() == "hsh")
	{
		image = new Hsh();
	}
	else if (fi.suffix() == "rti")
	{
		image = new UniversalRti();
	}
	else
	{
		cout << "Unsupported file format. The tool accepts LRGB-PTM or HSH images." << endl;
		exit(0);
	}

	if (image->load(filename) != 0)
	{
		cout << "The file is invalid." << endl;
		exit(0);
	}

	int width = image->width();
	int height = image->height();

	// folder creation

	
	QString name = fi.baseName();
	QString pathname = fi.absoluteFilePath();
	QDir dir(fi.absolutePath());

	if (dir.exists(name))
	{
		dir.cd(name);

		// remove all files
		dir.setFilter(QDir::Files);
		QFileInfoList filelist = dir.entryInfoList();
		for (int i = 0; i < filelist.size(); ++i) 
		{
			QFileInfo fileInfo = filelist.at(i);
			QFile f(fileInfo.absoluteFilePath());
			f.remove();
		}

		dir.cdUp();
		dir.rmdir(name);
	}

	dir.mkdir(name);
	dir.cd(name);

	// decomposition and compression
	//////////////////////////////////////////////////////////////

	float aspect_ratio = static_cast<float>(height) / static_cast<float>(width);
	int previewWidth = 400;
	int previewHeight = previewWidth * aspect_ratio;
	QImage *img = image->createPreview(previewWidth, previewHeight);
	QString previewname = fi.absolutePath() + QString("/") + name + QString("/") + QString("thumb.jpg");
        img->save(previewname, 0, 100);

	int x1,y1,x2,y2;

	for (int k = 1; k <= level; k++)
	{
		int *zm = ZOrder::createZMatrix(k);
		int size = 1;
		for (int jj=0; jj < k; jj++)
			size <<= 1;

		float deltaW = static_cast<float>(width) / static_cast<float>(size);
		float deltaH = static_cast<float>(height) / static_cast<float>(size);

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
			{
				QString patchname = fi.absolutePath() + QString("/") + name + QString("/");
				patchname += QString("tile_lvl%1_%2.dat").arg(k).arg(zm[j + i * size]);

				cout << patchname.toStdString() << endl;

				x1 = static_cast<int>(deltaW * j);
				y1 = static_cast<int>(deltaH * i);
				x2 = static_cast<int>(deltaW * (j+1));
				y2 = static_cast<int>(deltaH * (i+1));

				cout << "Tile Size: " << (x2-x1) << " x " << (y2-y1) << endl;

				// save compressed tile
				image->saveCompressed(x1,y1,x2,y2,level-k,patchname);
			}

		delete [] zm;
	}

	// Save XML information file
	//////////////////////////////////////////////////////////////

	QString infofilename = fi.absolutePath() + QString("/") + name + QString("/") + QString("%1.%2").arg(name).arg(fi.suffix());
	image->saveRemoteDescr(infofilename, level);
	
	delete image;
	delete img;
	return 0;
}
