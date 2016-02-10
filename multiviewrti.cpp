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


#include "multiviewrti.h"

#include <QTime>
#include <QFileInfo>
#include <QLabel>
#include <QGridLayout>



ViewpointControl::ViewpointControl(int initValueX, int nViewX, int initValueY, int nViewY, bool enableFlow, bool useFlow, QWidget *parent) : QWidget(parent)
{
	maxViewX = nViewX;
	maxViewY = nViewY;
	QLabel* label = new QLabel("Horizontal shift");
	viewpointSlider = new QSlider(Qt::Horizontal);
	viewpointSlider->setTickPosition(QSlider::TicksBothSides);
	snapNearest = new QCheckBox("Nearest viewpoint");
	viewpointSlider->setRange(0, 100*(maxViewX - 1));
	viewpointSlider->setTickInterval(100);
	viewpointSlider->setValue(initValueX * 100);
	QLabel* label1 = new QLabel("Vertical shift");
	vertical = new QSlider(Qt::Vertical);
	vertical->setTickPosition(QSlider::TicksBothSides);
	if (maxViewY > 1)
	{
		vertical->setRange(0, (maxViewY -1));
		vertical->setTickInterval(1);
		vertical->setValue(initValueY);
		vertical->setPageStep(1);
		vertical->setSingleStep(1);
	}
	if (!enableFlow)
	{
		viewpointSlider->setRange(0, (maxViewX - 1));
		viewpointSlider->setTickInterval(1);
		viewpointSlider->setValue(initValueX);
		snapNearest->setCheckState(Qt::Checked);
		snapNearest->setDisabled(true);
	}
	else if (useFlow)
	{
		snapNearest->setCheckState(Qt::Unchecked);
		viewpointSlider->setPageStep(10);
		viewpointSlider->setSingleStep(5);
	}
	else
	{
		snapNearest->setCheckState(Qt::Checked);
		viewpointSlider->setPageStep(100);
		viewpointSlider->setSingleStep(100);
	}
	this->useFlow = useFlow;
	connect(viewpointSlider, SIGNAL(sliderReleased()), this, SLOT(sliderReleased())); 
	connect(viewpointSlider, SIGNAL(valueChanged(int)), this, SIGNAL(viewpointChanged(int)));
	connect(snapNearest, SIGNAL(stateChanged(int)), this, SLOT(updateSlider(int)));
	connect(vertical, SIGNAL(valueChanged(int)), this, SIGNAL(rowChanged(int)));
	
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(label, 0, 0, 1 , 1);
	layout->addWidget(viewpointSlider, 1, 0, 1, 3);
	layout->addWidget(snapNearest, 2, 0, 1, 1);
	if (maxViewY > 1)
	{
		layout->addWidget(label1, 0, 3, 1, 1, Qt::AlignHCenter);
		layout->addWidget(vertical, 1, 3, 2, 1, Qt::AlignHCenter);
	}
	setLayout(layout);
}


MultiviewRti::MultiviewRti(): Rti(),
	posX(-1),
	posY(-1)
{
	currentRendering = DEFAULT;
	// Create list of supported rendering mode.
	list = new QMap<int, RenderingMode*>();
	viewpointLayout = Eigen::MatrixXi(1, 1);
}

MultiviewRti::~MultiviewRti()
{
	//delete viewpointLayout;
	for(int i = 0; i < images.size(); i++)
		delete images[i];
	for(int i = 0; i < flow.size(); i++)
	{
		if (flow[i].down) delete flow[i].down;
		if (flow[i].up) delete flow[i].up;
		if (flow[i].left) delete flow[i].left;
		if (flow[i].right) delete flow[i].right;
	}
}


int MultiviewRti::load(CallBackPos *cb)
{
	if (filename.isEmpty())
		return -1;
	else
		return load(filename, cb);
}


int MultiviewRti::load(QString name, CallBackPos *cb)
{
#ifdef PRINT_DEBUG
	QTime first = QTime::currentTime();
#endif

	remote = false;
	if (cb != NULL)	(*cb)(0, "Loading Multiview RTI...");
	filename = name;

	type = "Multiview RTI";

	QFile data(filename);
	if (!data.open(QFile::ReadOnly))
		return -1;

	bool error;
	QTextStream stream(&data);
	QString line = "";
	do 
	{
		line = stream.readLine();
	} while (line.startsWith("#"));
	QStringList strList = line.split(' ',  QString::SkipEmptyParts);
	if (strList.count() < 3)
		return -1;
	maxViewX = strList.at(0).toInt(&error);
	if (!error) return -1;
	maxViewY = strList.at(1).toInt(&error);
	if (!error) return -1;
	nViewpoint = strList.at(2).toInt(&error);
	if (!error) return -1;

	line = stream.readLine();
	strList = line.split(' ',  QString::SkipEmptyParts);
	if (strList.count() < 2)
		return -1;
	startX = strList.at(0).toInt(&error);
	if (!error) return -1;
	startY = strList.at(1).toInt(&error);
	if (!error) return -1;
		
	line = stream.readLine();
	strList = line.split(' ',  QString::SkipEmptyParts);
	if (strList.count() < 4)
		return -1;
	int temp = strList.at(0).toInt(&error);
	if (!error) return -1;
	useFlow = (temp == 1);
	separationX = strList.at(2).toInt(&error);
	if (!error) return -1;
	separationY = strList.at(3).toInt(&error);
	if (!error) return -1;

	list->insert(DEFAULT, new DefaultMRti(startX, startY, maxViewX, maxViewY, useFlow, true));
	QFileInfo info(filename);
	images = std::vector<UniversalRti*>(nViewpoint);
	for (int i = 0; i < nViewpoint; i++)
	{
		if (cb != NULL)(*cb)(i * 60 / nViewpoint, "Loading RTI file...");
		line = stream.readLine();
		strList = line.split(' ',  QString::SkipEmptyParts);
		if (strList.count() < 2)
			return -1;
		QFile image(QString("%1/%2").arg(info.absolutePath()).arg(strList.at(1)));
		if (!image.exists()) return -1;
		images[i] = new UniversalRti();
		images[i]->setFileName(image.fileName());
		images[i]->load();
	}
	
	w = images[0]->width();
	h = images[0]->height();

	//viewpointLayout = new vcg::ndim::Matrix<int>(maxViewY, maxViewX);
	viewpointLayout.resize(maxViewY, maxViewX);
	for (int i = 0; i < maxViewY; i++)
	{
		line = stream.readLine();
		strList = line.split(' ',  QString::SkipEmptyParts);
		if (strList.count() < maxViewX)
			return -1;
		for (int j = 0; j < maxViewX; j++)
		{
			//(*viewpointLayout)[i][j] = strList.at(j).toInt(&error) - 1;
			viewpointLayout(i, j) = strList.at(j).toInt(&error) - 1;
			if (!error) return -1;
		}
	}
	
	if (useFlow)
	{
		leftImage.hFlow = new float[w*h];
		//leftImage.vFlow = new float[w*h];
		rightImage.hFlow = new float[w*h];
		//rightImage.vFlow = new float[w*h];
		//leftUpImage.hFlow = new float[w*h];
		//leftUpImage.vFlow = new float[w*h];
		//rightUpImage.hFlow = new float[w*h];
		//rightUpImage.vFlow = new float[w*h];
		
		flow = std::vector<OpticalFlowData>(nViewpoint);
		for (int i = 0; i < nViewpoint; i++)
		{
			if (cb != NULL)(*cb)(60 + i * 40 / nViewpoint, "Loading Optical flow data ...");
			line = stream.readLine();
			strList = line.split(' ',  QString::SkipEmptyParts);
			if (strList.count() < 5)
				return -1;
			for (int j = 0; j < 4; j++)
			{
				if (strList.at(j+1) == "0")
				{
					switch(j)
					{ 
						case 0: flow[i].left = NULL; break;
						case 1: flow[i].right = NULL; break;
						case 2: flow[i].up = NULL; break;
						case 3: flow[i].down = NULL; break;
					}
				}
				else
				{
					int retVal;
					switch(j)
					{ 
						case 0: retVal = loadFlowData(QString("%1/%2").arg(info.absolutePath()).arg(strList.at(j+1)), &flow[i].left); break;
						case 1: retVal = loadFlowData(QString("%1/%2").arg(info.absolutePath()).arg(strList.at(j+1)), &flow[i].right); break;
						case 2: retVal = loadFlowData(QString("%1/%2").arg(info.absolutePath()).arg(strList.at(j+1)), &flow[i].up); break;
						case 3: retVal = loadFlowData(QString("%1/%2").arg(info.absolutePath()).arg(strList.at(j+1)), &flow[i].down); break;
					}
					if (retVal != 0)
						return -1;

				}
			}
		}
	}

	data.close();
	if (cb != NULL)	(*cb)(99, "Done");

#ifdef PRINT_DEBUG
	QTime second = QTime::currentTime();
        float diff = first.msecsTo(second) / 1000.0;
        printf("Multiview RTI Loading: %.5f s\n", diff);
#endif

	return 0;
}


int MultiviewRti::loadFlowData(const QString &path, std::vector<float>** output)
{
#ifdef WIN32
  #ifndef __MINGW32__
	FILE* file;
	if (fopen_s(&file, path.toStdString().c_str(), "rb") != 0)
		return -1;
  #else
	FILE* file = fopen(path.toStdString().c_str(), "rb");
	if (file == NULL)
		return -1;
  #endif
#else
	FILE* file = fopen(path.toStdString().c_str(), "rb");
	if (file == NULL)
		return -1;
#endif

	bool eof, error;
	QString str;
	do
	{
		str = getLine(file, &eof);
		if (eof) return -1;
	} while (str.startsWith("#"));

	QStringList strList = str.split(' ',  QString::SkipEmptyParts);
	if (strList.count() < 2) return -1;
	int width = strList.at(0).toInt(&error);
	if (!error) return -1;
	int height = strList.at(1).toInt(&error);
	if (!error) return -1;

	str = getLine(file, &eof);
	if (eof) return -1;
	strList = str.split(' ',  QString::SkipEmptyParts);
	if (strList.count() < 3) return -1;
	int elementSize = strList.at(0).toInt(&error);
	if (!error) return -1;
	float scale = strList.at(1).toFloat(&error);
	if (!error) return -1;
	float bias = strList.at(2).toFloat(&error);
	if (!error) return -1;
	*output = new std::vector<float>(width*height);
	
	void* temp;
	unsigned char c;
	short s;
	float f;
	
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			switch(elementSize)
			{
				case 1: fread(&c, sizeof(unsigned char), elementSize, file); f = (float)c /255.0; break;
				case 2: fread(&s, sizeof(unsigned char), elementSize, file); f = (float)s /255.0; break;
				case 4: fread(&f, sizeof(unsigned char), elementSize, file); f /= 255.0; break;
				default: fread(&c, sizeof(unsigned char), 1, file);
			}
			f = f * scale + bias;
			(*output)->at(j*width + i) = f;
		}
	}
	fclose(file);
	return 0;
}


int MultiviewRti::loadData(FILE* file, int width, int height, int basisTerm, bool urti, CallBackPos * cb,const QString& text)
{
	return 0;
}


int MultiviewRti::save(QString name)
{
	// Not implemented for now...
	return 0;
}


int MultiviewRti::loadCompressed()
{
	if (filename.isEmpty())
		return -1;
	else
		return loadCompressed(filename);
}


int MultiviewRti::loadCompressed(QString name)
{
	return loadCompressed(0,0,w,h,name);
}


int MultiviewRti::loadCompressed(int xinf, int yinf, int xsup, int ysup, QString name)
{
	
	return 0;
}


int MultiviewRti::saveCompressed(QString name)
{
	return saveCompressed(0,0,w,h,0,name);
}


int MultiviewRti::saveCompressed(int xinf, int yinf, int xsup, int ysup, int reslevel, QString name)
{
	return 0;
}


int MultiviewRti::createImage(unsigned char** buffer, int& width, int& height, const vcg::Point3f& light, const QRectF& rect, int level, int mode)
{
#ifdef PRINT_DEBUG
	QTime first = QTime::currentTime();
#endif

	DefaultMRti* rend = (DefaultMRti*)list->value(DEFAULT);
	float newPosX = rend->getCurrentPosX();
	float newPosY = rend->getCurrentPosY();
		
	if (useFlow)
	{
		//with optical flow
		int tempW, tempH;
		int left, right, up, down;
		int newLeft, newRight, newUp, newDown;
		
		left = floorf(posX);
		right = ceilf(posX);
		up = floorf(posY);
		down = ceilf(posY);

		newLeft = floorf(newPosX);
		newRight = ceilf(newPosX);
		newUp = floorf(newPosY);
		newDown = ceilf(newPosY);

		float distX = newPosX - newLeft;
		float distY = newPosY - newDown;
		
		if (newLeft != newRight && newUp != newDown)
		{

		}
		else if( newLeft == newRight && newUp != newDown)
		{

		}
		else if( newLeft != newRight && newUp == newDown)
		{
			leftUpImage.valid = false;
			rightUpImage.valid = false;
			//int leftIndex = (*viewpointLayout)[newDown][newLeft];
			int leftIndex = viewpointLayout(newDown, newLeft);
			if (leftImage.buffer)
				delete[] leftImage.buffer;
			images[leftIndex]->createImage(&leftImage.buffer, tempW, tempH, light, QRectF(0,0,w,h));
			leftImage.valid = true;
			unsigned char* tLeft = new unsigned char[tempW*tempH*4];
			//int rightIndex = (*viewpointLayout)[newDown][newRight];
			int rightIndex = viewpointLayout(newDown, newRight);
			if (rightImage.buffer)
				delete[] rightImage.buffer;
			images[rightIndex]->createImage(&rightImage.buffer, tempW, tempH, light, QRectF(0,0,w,h));
			rightImage.valid = true;
			unsigned char* tRight = new unsigned char[tempW*tempH*4];

			applyOpticalFlow(leftImage.buffer, *flow[leftIndex].right, distX, tLeft, leftImage.hFlow);
			applyOpticalFlow(rightImage.buffer, *flow[rightIndex].left, 1.0 - distX, tRight, rightImage.hFlow);
			
			width = ceil(rect.width());
			height = ceil(rect.height()); 
			int offx = rect.x();
			int offy = rect.y();

			(*buffer) = new unsigned char[width*height*4];
			unsigned char* ptrBuffer = (*buffer);
			
			int offsetBuf = 0;
			for (int y = offy; y < offy + height; y++)
			{
				for (int x = offx; x < offx + width; x++)
				{
					int offset = y * w + x;
					int offset4 = offset*4;
					if (leftImage.hFlow[offset] < 1 && rightImage.hFlow[offset] < 1)
					{ 
						for (int i = 0; i < 3; i++)
							ptrBuffer[i] = tobyte(tLeft[offset4 + i]*(1.0 - distX) + tRight[offset4 + i]*distX);
						ptrBuffer[3] = 255;
					}
					else if(leftImage.hFlow[offset] < 1)
					{
						for (int i = 0; i < 3; i++)
							ptrBuffer[i] = tLeft[offset4 + i];
						ptrBuffer[3] = 255;
					}
					else if (rightImage.hFlow[offset] < 1) 
					{
						for (int i = 0; i < 3; i++)
							ptrBuffer[i] = tRight[offset4 + i];
						ptrBuffer[3] = 255;
					}
					else if(leftImage.hFlow[offset] < rightImage.hFlow[offset])
					{
						for (int i = 0; i < 3; i++)
							ptrBuffer[i] = tLeft[offset4 + i];
						ptrBuffer[3] = 255;
					}
					else if (leftImage.hFlow[offset] > rightImage.hFlow[offset])
					{
						for (int i = 0; i < 3; i++)
							ptrBuffer[i] = tRight[offset4 + i];
						ptrBuffer[3] = 255;
					}
					else 
					{
						for (int i = 0; i < 3; i++)
							ptrBuffer[i] = 0;
						ptrBuffer[3] = 0;
					}
					ptrBuffer += 4;
				}
			}

			ptrBuffer = (*buffer);
			for (int y = 0; y < height; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					int offset = y * width + x;
					if (ptrBuffer[offset*4 + 3] == 0)
					{
						int startX = x - 1;
						while (ptrBuffer[offset*4 + 3] == 0 && x < width)
						{
							offset++;
							x++;
						}
						if (x < width)
						{
							int stopX = x;
                                                        unsigned char* leftPx = &ptrBuffer[(y * width + startX)<< 2];
                                                        unsigned char* rightPx = &ptrBuffer[(y * width + stopX)<< 2];
							for (int i = startX + 1; i < stopX; i++)
							{
                                                                unsigned char* ptr = &ptrBuffer[(y*width + i)<<2];
								for (int j  = 0; j < 3; j++)
									ptr[j] = leftPx[j] + (i - startX) * (rightPx[j] - leftPx[j]) / (stopX - startX);
								ptr[3] = 255;
							}

						}

					}
				}
			}
			delete[] tLeft;
			delete[] tRight;
		}
		else if( newLeft == newRight && newUp == newDown)
		{
			leftImage.valid = false;
			rightImage.valid = false;
			leftUpImage.valid = false;
			rightUpImage.valid = false;
			//images[(*viewpointLayout)[newDown][newLeft]]->createImage(buffer, width, height, light, rect);
			images[viewpointLayout(newDown, newLeft)]->createImage(buffer, width, height, light, rect);
		}
	}
	else
	{
		//without optical flow
		leftImage.valid = false;
		rightImage.valid = false;
		leftUpImage.valid = false;
		rightUpImage.valid = false;
		//images[(*viewpointLayout)[(int)newPosY][(int)newPosX]]->createImage(buffer, width, height, light, rect, level, mode);
		images[viewpointLayout((int)newPosY, (int)newPosX)]->createImage(buffer, width, height, light, rect, level, mode);
	}
	posX = newPosX;
	posY = newPosY;

#ifdef PRINT_DEBUG
	QTime second = QTime::currentTime();
        float diff = first.msecsTo(second) / 1000.0;
        printf("Default rendering Multiview RTI: %.5f s\n", diff);
	
#endif

	return 0;
}



void MultiviewRti::applyOpticalFlow(const unsigned char* image, const std::vector<float>& flowData, float dist, unsigned char* outImg, float* outFlow)
{
	int offset = 0;
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			outFlow[offset++] = 50;

        float* tempImg = new float[w*h*4];
        memset(tempImg, 0, sizeof(float)*w*h*4);
        float* contrib = new float[w*h];
        memset(contrib, 0, sizeof(float)*w*h);

	const unsigned char* ptrImage = image;
	offset = 0;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			offset = y*w + x;
			float shift = flowData[offset];
			if (shift != 9999)
			{
				if (x + shift < w && x + shift >= 0)
				{
                                        float value = shift * dist;
					int left = floor(value);
					int right = ceil(value);
                                        float* ptrTemp = &tempImg[(offset+left)<<2];
					if (left != right)
					{
                                                float leftRem = vcg::math::Abs(value - left);
                                                float rightRem = vcg::math::Abs(value - right);
						int leftIndex = offset + left;
						contrib[leftIndex] += rightRem;
						if (outFlow[leftIndex] > leftRem)
							outFlow[leftIndex] = leftRem;
						if (x < w - 1)
						{
							contrib[leftIndex + 1] += leftRem;
							if (outFlow[leftIndex + 1] > rightRem)
								outFlow[leftIndex + 1] = rightRem;
							for (int i = 0; i < 3; i++)
							{
								ptrTemp[i] += ptrImage[i] * rightRem;
								ptrTemp[i + 4] += ptrImage[i] * leftRem;
							}
						}
						else
						{
							for (int i = 0; i < 3; i++)
								ptrTemp[i] += ptrImage[i] * rightRem;
						}
					}
					else
					{
						float fraction = vcg::math::Abs(value - left);
						if (outFlow[offset + left] > fraction)
						{
							outFlow[offset + left] = fraction;
							contrib[offset + left] = 1;
							for (int i = 0; i < 3; i++)
								ptrTemp[i] += ptrImage[i];
						}
					}
				}
			}
			ptrImage += 4;
		}
	}
	
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			offset = y*w + x;
			if (contrib[offset] > 0)
			{
				for (int i = 0; i < 3; i++)
					outImg[offset*4 + i] = tobyte(tempImg[offset*4 + i]/contrib[offset]);
			}
			else
			{
				for (int i = 0; i < 3; i++)
					outImg[offset*4 + i] = 0;
			}
		}
	}
	
	delete[] contrib;
	delete[] tempImg;
}




QImage* MultiviewRti::createPreview(int width, int height)
{
	return images[2]->createPreview(width, height);
}


int MultiviewRti::allocateRemoteImage(QBuffer* b)
{
	return 0;
}


int MultiviewRti::loadCompressedHttp(QBuffer* b, int xinf, int yinf, int xsup, int ysup, int level)
{
	return 0;
}


void MultiviewRti::saveRemoteDescr(QString& filename, int level)
{

}
