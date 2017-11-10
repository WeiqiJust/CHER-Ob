/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)

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
   anonymous help from various software engineering communities.

   This file defines the different classes of bookmark that the user may work
   with (corresponding to the different file types that may be displayed).

*****************************************************************************/

#include "note.h"
#include <time.h> 
#include "../mainWindow.h"
#include "../vtkEnums.h"
#include <sstream>

Note::Note(const int noteId, const ColorType type)
{
	mNoteId = noteId;
	mColor = type;
	mDialog = new QDialog();
	mVbox = new QVBoxLayout();
	mInfo = new QString();
	mTextEdit = new QTextEdit(mDialog);
	mButtons = new QHBoxLayout();
	mLabel = new QLabel(mDialog);
	mSaveButton = new QPushButton("Save");
	mRemoveButton = new QPushButton("Remove");
	mAddImageButton = new QPushButton("Add Image");
	mSaveButton->setEnabled(true);
	mAddImageButton->setEnabled(true);
    mRemoveButton->setEnabled(true);

	connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
	connect(mSaveButton, SIGNAL(clicked()), this, SLOT(save()));
	connect(mAddImageButton, SIGNAL(clicked()), this, SLOT(addImage()));
    connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(remove()));

	mVbox->addWidget(mLabel);
    mVbox->addWidget(mTextEdit);

	mButtons->addWidget(mAddImageButton);
	mButtons->addWidget(mSaveButton);
    mButtons->addWidget(mRemoveButton);

	mVbox->addLayout(mButtons);
	
    mDialog->setLayout(mVbox);
	mDialog->adjustSize();
	isSaved = false;
	isRemoved = false;
	mDialog->hide();
}

QString Note::getContent()
{
	QString content(*mInfo + "\n" + mTextEdit->toPlainText());
	content.append("\nLinked Images:\n");
	for (int i = 0; i < mImageNotes.size(); i++)
	{
		content.append(mImageNotes[i]->getPath() + "\n");
	}
	return content;
}

void Note::setLabel(QString text)
{		
	mLabel->setText(text);
}

void Note::setInfo(QString text)
{
	mInfo = new QString(text);
}

void Note::textChanged()
{
	isSaved = false;
}

void Note::save(QString spectrumCaption, QString spectrumImage)
{
	if (isRemoved || isSaved)
		return;
    if (!mFile->open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Open Note file " << this->mNoteId << " " << mFileName << " Failed"; 
        return;
	}
    QTextStream out(mFile);

	if (mUsers.indexOf(mw()->mUserName) == -1 && mw()->mUserName != QString())
	{
		int index = mUsers.indexOf(QString("Unknown"));
		if (index != -1)
			mUsers.remove(index);
		mUsers.push_back(mw()->mUserName);
		updateInfo();
		updateLabel();
	}

	out << *mInfo << "\n";
	if (!spectrumCaption.isEmpty()) {
		mTextEdit->setPlainText(spectrumCaption);
	}
	out << mTextEdit->toPlainText();
	out << "\nLinked Images:\n";
	if (!spectrumCaption.isEmpty()) {
		QString objectPath(*mPath);
		objectPath.truncate(objectPath.lastIndexOf(QDir::separator()));
		ImageNote* imageNote = new ImageNote(objectPath, spectrumImage, mImageNotes.size());
		if (imageNote->getPath() == QString())
		{
			delete imageNote;
		}
		mImageNotes.push_back(imageNote);
	}
	QDir dir(*mPath);
	for (int i = 0; i < mImageNotes.size(); i++)
	{
		
		out << dir.relativeFilePath(mImageNotes[i]->getPath()) << "\n";
	}
	mFile->close();
	isSaved = true;
}

void Note::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                //delete widget;
				widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}

MainWindow* Note::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
  return NULL;
}

void Note::updateInfo()
{
	int start = mInfo->indexOf("User: \n");
	if (start == -1)
		return;
	int end = mInfo->indexOf("Color Type:") - 1;
	mInfo->remove(start+7, end - start - 6);
	QString user;
	for (int i = 0; i < mUsers.size(); i++)
	{
		user.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			user.append(QString(";"));
		else
			user.append(QString("\n"));
	}
	mInfo->insert(start+7, user);
	mInfo->simplified();
}

void Note::updateLabel()
{
	QString label = mLabel->text();
	int start = label.indexOf("User: ");
	if (start == -1)
		return;
	label = label.left(start+6);
	QString user;
	for (int i = 0; i < mUsers.size(); i++)
	{
		user.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			user.append(QString(";"));
	}
	label.append(user);
	setLabel(label);
}

void Note::removeImage(int id)
{
	isSaved = false;
	mImageNotes.remove(id);
	clearLayout(mVbox->itemAt(2+id)->layout());
	mVbox->removeItem(mVbox->itemAt(2+id));
	for (int i = 0; i < mImageNotes.size(); i++)
	{
		mImageNotes[i]->setId(i);
	}
}

void Note::replaceUserName(const QString newName, const QString oldName)
{
	if (oldName == QString())
	{
		mUsers.push_back(newName);
	}
	else
	{
		int index = mUsers.indexOf(oldName);
		if (index == -1)
		{
			mUsers.push_back(newName);
		}
		else
		{
			mUsers[index] = newName;
		}
	}
	updateInfo();
	updateLabel();
}

void Note::remove()
{
	mFile->remove();
	mDialog->hide();
	isRemoved = true;
	emit removeNote(mNoteId, mPath);
}

void Note::addImage(QString fileName)
{
	QString objectPath(*mPath);
	objectPath.truncate(objectPath.lastIndexOf(QDir::separator()));
	ImageNote* imageNote = new ImageNote(objectPath, fileName, mImageNotes.size());
	if (imageNote->getPath() == QString())
	{
		delete imageNote;
		return;
	}
	QVBoxLayout* mImageVBox = new QVBoxLayout();
	QHBoxLayout* mInfoHBox = new QHBoxLayout();
	mImageVBox->addWidget(imageNote->getImage(), 1, Qt::AlignCenter);
	mInfoHBox->addWidget(imageNote->getName());
	mInfoHBox->addWidget(imageNote->getButton());
	mImageVBox->addLayout(mInfoHBox);
	connect(imageNote, SIGNAL(removeImage(int)), this, SLOT(removeImage(int)));
	mVbox->insertLayout(2+mImageNotes.size(), mImageVBox);
	mImageNotes.push_back(imageNote);
	mDialog->adjustSize();
	isSaved = false;
}

PointNote::PointNote(QString path, double* pos, const int cellId, const int noteId, const ColorType type, const QString user)
	: Note(noteId, type)
{
	mPath = new QString(path);
	mPosition = new double[3];
	//qsrand(time(NULL));
	//int number = qrand();
	mFileName = new QString(path);
	//mFileName->append(QDir::separator() + QString("Note") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	mFileName->append(QDir::separator() + QString("PointNote") + WORD_SEPARATOR + QString::number(cellId) + QString(".txt"));
	QString mUser = user;
	if (user == QString())
		mUsers.push_back(QString("Unknown"));
	else
	{
		mUser.simplified();
		mUser.replace("; ", ";");
		mUser.replace(" ;", ";");
		QStringList users = mUser.split(";");
		for (int i = 0; i < users.size(); i++)
			mUsers.push_back(users[i]);
	}

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
	for (int i = 0; i < 3; i++)
	{
		mPosition[i] = pos[i];
	}
	mCellId = cellId;
	QString label;
	label.append(QString("Point Note: Polygon Id (") + QString::number(cellId) + QString(")"));
	QString info(label);
	info.append(QString(" Coordinates (") + QString::number(mPosition[0]) + QString(", ") + QString::number(mPosition[1])
		+ QString(", ") + QString::number(mPosition[2]) + QString(")\n"));
	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(QString(colortype2str(mColor).c_str()));
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	qDebug() << "finish PointNote instructor";
}

PointNote::PointNote(QString path, QString fileName, const int noteId, bool& isSucceed)
	: Note(noteId)
{	
	mPath = new QString(path);
	mPosition = new double[3];
	mFileName = new QString(path);
	mFileName->append(QDir::separator() + fileName);

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
    if (!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Open Note file " << this->mNoteId << " " << mFileName << " Failed"; 
		isSucceed = false;
		return;
	}
    QTextStream in(mFile);
    QString firstLine = in.readLine();
	if (firstLine.split(" ").size() != 9)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}

	bool ok0, ok1, ok2, ok3;
	double pos[3];
	int cellId;
	cellId = firstLine.split(" ")[4].split(")")[0].split("(")[1].toInt(&ok0);
	pos[0] = firstLine.split(" ")[6].split(",")[0].split("(")[1].toDouble(&ok1);
	pos[1] = firstLine.split(" ")[7].split(",")[0].toDouble(&ok2);
	pos[2] = firstLine.split(" ")[8].split(")")[0].toDouble(&ok3);
	if (!ok0 || !ok1 || !ok2 || !ok3)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("User: "))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString userLine;
	in >> userLine;
	QStringList users = userLine.split(";");
	for (int i = 0; i < users.size(); i++)
		mUsers.push_back(users[i]);
	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Color Type:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString colorType;
	in >> colorType;
	mColor = str2colortype(colorType.toStdString());
    while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Note Start:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString content = in.readAll();
	QString text = content.split("\nLinked Images:\n")[0];
	QString imagePaths = content.split("\nLinked Images:\n")[1];
	QStringList imagePathList = imagePaths.split("\n");
	QDir dir(*mPath);
	for (int i = 0; i < imagePathList.size(); i++)
	{
		//qDebug()<<"Image Path"<<imagePathList[i];
		QFileInfo finfo(dir.absoluteFilePath(imagePathList[i]));
		if (!finfo.exists())
			continue;
		QString extension = finfo.suffix().toLower();
		if (extension != tr("png") && extension != tr("jpg") && extension != tr("jpeg") && extension != tr("tif") && extension != tr("bmp"))
			continue;
		addImage(finfo.absoluteFilePath());
	}
	this->setText(text);
	for (int i = 0; i < 3; i++)
	{
		mPosition[i] = pos[i];
	}
	mCellId = cellId;
	QString label;
	label.append(QString("Point Note: Polygon Id (") + QString::number(cellId) + QString(")"));
	QString info(label);
	info.append(QString(" Coordinates (") + QString::number(mPosition[0]) + QString(", ") + QString::number(mPosition[1])
		+ QString(", ") + QString::number(mPosition[2]) + QString(")\n"));
	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(colorType);
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	mFile->close();
	qDebug() << "finish PointNote instructor";
	isSucceed = true;
}

void PointNote::removePointNote()
{
	mFile->remove();
	this->hideNote(); 
}

SurfaceNote::SurfaceNote(QString path, vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> points,
						 const int noteId, const ColorType type, bool CTVolume, const QString user)
	: Note(noteId, type)
{
	mPath = new QString(path);
    mCellIds = vtkSmartPointer<vtkSelectionNode>::New();
	mCellIds->SetFieldType(vtkSelectionNode::CELL);
    mCellIds->SetContentType(vtkSelectionNode::INDICES);
	mFileName = new QString(path);
	isCTVolume = CTVolume;
	if (!isCTVolume)
		qsrand(time(NULL) * cellIds->GetSelectionList()->GetSize());
	else
		qsrand(time(NULL));
	int number = qrand();
	mFileName->append(QDir::separator() + QString("SurfaceNote") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	QString mUser = user;
	if (user == QString())
		mUsers.push_back(QString("Unknown"));
	else
	{
		mUser.simplified();
		mUser.replace("; ", ";");
		mUser.replace(" ;", ";");
		QStringList users = mUser.split(";");
		for (int i = 0; i < users.size(); i++)
			mUsers.push_back(users[i]);
	}

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);

	if (!isCTVolume)
		mCellIds->DeepCopy(cellIds);
	for (int i = 0; i < points.size(); i++)
	{
		double* point = new double[3];
		point[0] = points[i][0];
		point[1] = points[i][1];
		point[2] = points[i][2];
		mPoints.push_back(point);
	}

	QString label;
	if (!isCTVolume)
		label.append(QString("Surface Note: Number of Selected Polygon ( ") + QString::number(mCellIds->GetSelectionList()->GetSize()) + QString(" )"));
	else
		label.append(QString("Surface Note: Number of Selected Polygon ( 0 )"));
	QString info(label);

	info.append(QString("\nCorner Points:\n"));
	for (int i = 0; i < 4; i++)
	{
		info = info + "(" + QString::number(mPoints[i][0]) + ", " + QString::number(mPoints[i][1]) + ", " + QString::number(mPoints[i][2]) + ")\n";
	}
	info.append(QString("Polygon Ids:\n"));
	if (!isCTVolume)
	{
		for (int i = 0; i < mCellIds->GetSelectionList()->GetSize(); i++)
		{
			info.append(QString::number(mCellIds->GetSelectionList()->GetVariantValue(i).ToInt()));
			if (i != mCellIds->GetSelectionList()->GetSize() - 1)
				info.append(QString(' '));
			else
				info.append(QString("\n"));
		}
	}
	QString userLabel= QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(QString(colortype2str(mColor).c_str()));
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	qDebug() << "finish Surface Note instructor";
}

SurfaceNote::SurfaceNote(QString path, QString fileName, const int noteId, bool& isSucceed)
	: Note(noteId)
{	
	mPath = new QString(path);
	mCellIds = vtkSmartPointer<vtkSelectionNode>::New();
	mCellIds->Initialize();
	mCellIds->SetFieldType(vtkSelectionNode::CELL);
    mCellIds->SetContentType(vtkSelectionNode::INDICES);
	vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
	isCTVolume = false;
	mFileName = new QString(path);
	mFileName->append(QDir::separator() + fileName);

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
    if (!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Open Note file " << this->mNoteId << " " << mFileName << " Failed"; 
		isSucceed = false;
		return;
	}
	
    QTextStream in(mFile);
    QString firstLine = in.readLine();
	if (firstLine.split(" ").size() != 9)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}
		
	bool ok0;
	int cellNum;
	cellNum = firstLine.split(" ")[7].toInt(&ok0);
	
	if (!ok0)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}

	if (cellNum == 0)
		isCTVolume = true;

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Corner Points:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	bool ok1, ok2;
	for (int i = 0; i < 4; i++)
	{
		QString pointLine = in.readLine();
		double* cornerPoint = new double[3];
		cornerPoint[0] = pointLine.split(" ")[0].split(",")[0].split("(")[1].toDouble(&ok0);
		cornerPoint[1] = pointLine.split(" ")[1].split(",")[0].toDouble(&ok1);
		cornerPoint[2] = pointLine.split(" ")[2].split(")")[0].toDouble(&ok2);
		if (!ok0 || !ok1 || !ok2)
		{
			qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
			isSucceed = false;
			return;
		}
		mPoints.push_back(cornerPoint);
	}

    while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Polygon Ids:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	ids->Resize(cellNum);
	for (int i = 0; i < cellNum; i++)
	{
		int cellId;
		in >> cellId;
		ids->InsertNextValue(cellId);
	}
	mCellIds->SetSelectionList(ids);

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("User: "))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString userLine;
	in >> userLine;
	QStringList users = userLine.split(";");
	for (int i = 0; i < users.size(); i++)
		mUsers.push_back(users[i]);
	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Color Type:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString colorType;
	in >> colorType;

	mColor = str2colortype(colorType.toStdString());
	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Note Start:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}

	QString content = in.readAll();
	QString text = content.split("\nLinked Images:\n")[0];
	QString imagePaths = content.split("\nLinked Images:\n")[1];
	QStringList imagePathList = imagePaths.split("\n");
	QDir dir(*mPath);
	for (int i = 0; i < imagePathList.size(); i++)
	{
		qDebug()<<"Image Path"<<imagePathList[i];
		QFileInfo finfo(dir.absoluteFilePath(imagePathList[i]));
		if (!finfo.exists())
			continue;
		QString extension = finfo.suffix().toLower();
		if (extension != tr("png") && extension != tr("jpg") && extension != tr("jpeg") && extension != tr("tif") && extension != tr("bmp"))
			continue;
		addImage(finfo.absoluteFilePath());
	}
	this->setText(text);

	QString label;
	label.append(QString("Surface Note: Number of Selected Polygon ( ") + QString::number(cellNum) + QString(" )"));
	QString info(label);
	info.append(QString("\nCorner Points:\n"));
	for (int i = 0; i < 4; i++)
	{
		info = info + "(" + QString::number(mPoints[i][0]) + ", " + QString::number(mPoints[0][1]) + ", " + QString::number(mPoints[0][2]) + ")\n";
	}
	info.append(QString("\nPolygon Ids:\n"));

	for (int i = 0; i < cellNum; i++)
	{
		info.append(QString::number(mCellIds->GetSelectionList()->GetVariantValue(i).ToInt()));
		if (i != mCellIds->GetSelectionList()->GetSize() - 1)
			info.append(QString(' '));
		else
			info.append(QString("\n"));
	}

	QString userLabel= QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(colorType);
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	mFile->close();
	qDebug() << "finish Surface instructor";
	isSucceed = true;
}

SurfaceNote::~SurfaceNote()
{
	for (int i = 0; i < mPoints.size(); i++)
	{
		delete mPoints[i];
	}
}

void SurfaceNote::removeSurfaceNote()
{
	mFile->remove();
	this->hideNote(); 
}


FrustumNote::FrustumNote(QString path, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, const int noteId, const ColorType type, const QString user)
	: Note(noteId, type)
{
	mPath = new QString(path);
	mFileName = new QString(path);
	mPoints  = vtkSmartPointer<vtkPoints>::New();
	mNormals = normals;
	//mFileName->append(QDir::separator() + QString("Note") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	qsrand(time(NULL));
	int number = qrand();
	mFileName->append(QDir::separator() + QString("FrustumNote") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	QString mUser = user;
	if (user == QString())
		mUsers.push_back(QString("Unknown"));
	else
	{
		mUser.simplified();
		mUser.replace("; ", ";");
		mUser.replace(" ;", ";");
		QStringList users = mUser.split(";");
		for (int i = 0; i < users.size(); i++)
			mUsers.push_back(users[i]);
	}

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);

	mPoints->DeepCopy(points);
	mNormals->DeepCopy(normals);

	QString label;
	label.append(QString("Frustum Note: Origin Point of Each Plane:\n"));
	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		double point[3];
		points->GetPoint(i, point);
		label.append(QString("(") + QString::number(point[0]) + ", " + QString::number(point[1]) + ", " + QString::number(point[2]) + ")\n");
	}
	QString info(label);
	info.append(QString("Normal Point of Each Plane:\n"));
	for (int i = 0; i < 6; i++)
	{
		info.append(QString("(") + QString::number(mNormals->GetVariantValue(i*3).ToDouble()) + ", " 
			+ QString::number(mNormals->GetVariantValue(i*3 + 1).ToDouble()) + ", " + QString::number(mNormals->GetVariantValue(i*3 + 2).ToDouble()) + ")\n");
	}
	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(QString(colortype2str(mColor).c_str()));
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	qDebug() << "finish Frustum Note instructor";
}

FrustumNote::FrustumNote(QString path, QString fileName, const int noteId, bool& isSucceed)
	: Note(noteId)
{	
	mPath = new QString(path);
	mFileName = new QString(path);
	mFileName->append(QDir::separator() + fileName);
	mPoints  = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkDoubleArray> normalArray = vtkSmartPointer<vtkDoubleArray>::New();
	mNormals = vtkDataArray::SafeDownCast(normalArray);

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
    if (!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Open Note file " << this->mNoteId << " " << mFileName << " Failed"; 
		isSucceed = false;
		return;
	}
	
    QTextStream in(mFile);
    in.readLine();
	for (int i = 0; i < 6; i++)
	{
		QString line = in.readLine();
		if (line.split(" ").size() != 3)
		{
			qDebug() << "The Syntax of is incorrect. The First Line is " << line;
			isSucceed = false;
			return;
		}
		bool ok0, ok1, ok2;
	    double pos[3];
		pos[0] = line.split(" ")[0].split("(")[1].split(",")[0].toDouble(&ok0);
		pos[1] = line.split(" ")[1].split(",")[0].toDouble(&ok1);
		pos[2] = line.split(" ")[2].split(")")[0].toDouble(&ok2);
		if (!ok0 || !ok1 || !ok2)
		{
			qDebug() << "The Syntax of Line is incorrect. The First Line is " << line;
			isSucceed = false;
			return;
		}
		mPoints->InsertNextPoint(pos);
	}

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Normal Point of Each Plane:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	mNormals->SetNumberOfComponents(3);
	mNormals->SetNumberOfTuples(6);
	for (int i = 0; i < 6; i++)
	{
		QString line = in.readLine();
		if (line.split(" ").size() != 3)
		{
			qDebug() << "The Syntax of First Line is incorrect. The First Line is " << line;
			isSucceed = false;
			return;
		}
		bool ok0, ok1, ok2;
	    double pos[3];
		pos[0] = line.split(" ")[0].split("(")[1].split(",")[0].toDouble(&ok0);
		pos[1] = line.split(" ")[1].split(",")[0].toDouble(&ok1);
		pos[2] = line.split(" ")[2].split(")")[0].toDouble(&ok2);
		if (!ok0 || !ok1 || !ok2)
		{
			qDebug() << "The Syntax of input note is incorrect. The First Line is " << line;
			isSucceed = false;
			return;
		}
		mNormals->InsertComponent(i, 0, pos[0]);
		mNormals->InsertComponent(i, 1, pos[1]);
		mNormals->InsertComponent(i, 2, pos[2]);
	}

	mNormals->Resize(6);

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("User: "))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString userLine;
	in >> userLine;
	QStringList users = userLine.split(";");
	for (int i = 0; i < users.size(); i++)
		mUsers.push_back(users[i]);

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Color Type:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString colorType;
	in >> colorType;

	mColor = str2colortype(colorType.toStdString());
	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Note Start:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}

	QString content = in.readAll();
	QString text = content.split("\nLinked Images:\n")[0];
	QString imagePaths = content.split("\nLinked Images:\n")[1];
	QStringList imagePathList = imagePaths.split("\n");
	QDir dir(*mPath);
	for (int i = 0; i < imagePathList.size(); i++)
	{
		qDebug()<<"Image Path"<<imagePathList[i];
		QFileInfo finfo(dir.absoluteFilePath(imagePathList[i]));
		if (!finfo.exists())
			continue;
		QString extension = finfo.suffix().toLower();
		if (extension != tr("png") && extension != tr("jpg") && extension != tr("jpeg") && extension != tr("tif") && extension != tr("bmp"))
			continue;
		addImage(finfo.absoluteFilePath());
	}
	this->setText(text);

	QString label;
	label.append(QString("Frustum Note: Origin Point of Each Plane:\n"));
	for (int i = 0; i < mPoints->GetNumberOfPoints(); i++)
	{
		double point[3];
		mPoints->GetPoint(i, point);
		label.append(QString("(") + QString::number(point[0]) + ", " + QString::number(point[1]) + ", " + QString::number(point[2]) + ")\n");
	}
	QString info(label);
	info.append(QString("Normal Point of Each Plane:\n"));
	for (int i = 0; i < 6; i++)
	{
		info.append(QString("(") + QString::number(mNormals->GetVariantValue(i*3).ToDouble()) + ", " 
			+ QString::number(mNormals->GetVariantValue(i*3 + 1).ToDouble()) + ", " + QString::number(mNormals->GetVariantValue(i*3 + 2).ToDouble()) + ")\n");
	}

	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(colorType);
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	mFile->close();
	qDebug() << "finish Frustum instructor";
	isSucceed = true;
}

void FrustumNote::removeFrustumNote()
{
	mFile->remove();
	this->hideNote(); 
}

PointNote2D::PointNote2D(QString path, const double* point, const int* pointImage, const int noteId, const ColorType type, const QString user)
	: Note(noteId, type)
{
	mPath = new QString(path);
    mPoint = new double[3];
	mImagePoint = new int[3];
	mFileName = new QString(path);
	//mFileName->append(QDir::separator() + QString("Note") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	qsrand(time(NULL));
	int number = qrand();
	mFileName->append(QDir::separator() + QString("PointNote2D") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	QString mUser = user;
	if (user == QString())
		mUsers.push_back(QString("Unknown"));
	else
	{
		mUser.simplified();
		mUser.replace("; ", ";");
		mUser.replace(" ;", ";");
		QStringList users = mUser.split(";");
		for (int i = 0; i < users.size(); i++)
			mUsers.push_back(users[i]);
	}

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
	for (int i = 0; i < 3; i++)
	{
		mPoint[i] = point[i];
		mImagePoint[i] = pointImage[i];
	}

	QString label;
	label.append(QString("Point Note: World Coordinate (") + QString::number(point[0]) + QString(", ") + QString::number(point[1]) + QString(")")); 
	QString info;
	info.append(QString("Point Note: World Coordinate (") + QString::number(point[0]) + QString(", ") + QString::number(point[1]) + QString(") Image Coordinate (")
		+ QString::number(pointImage[0]) + QString(", ") + QString::number(pointImage[1]) + QString(")\n"));
	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(QString(colortype2str(mColor).c_str()));
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	qDebug() << "finish Point Note 2D instructor";
}

PointNote2D::PointNote2D(QString path, QString fileName, const int noteId, bool& isSucceed)
	: Note(noteId)
{	
	mPath = new QString(path);
	mPoint = new double[3];
	mImagePoint = new int[3];
	vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
	
	mFileName = new QString(path);
	mFileName->append(QDir::separator() + fileName);

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
    if (!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Open Note file " << this->mNoteId << " " << mFileName << " Failed"; 
		isSucceed = false;
		return;
	}
	
    QTextStream in(mFile);
    QString firstLine = in.readLine();
	if (firstLine.split(" ").size() != 10)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}

	bool ok0, ok1;
	double pos[3];
	pos[0] = firstLine.split(" ")[4].split(",")[0].split("(")[1].toDouble(&ok0);
	pos[1] = firstLine.split(" ")[5].split(")")[0].toDouble(&ok1);
	pos[2] = 0;
	
	if (!ok0 || !ok1)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}

	int posImage[3];
	posImage[0] = firstLine.split(" ")[8].split(",")[0].split("(")[1].toInt(&ok0);
	posImage[1] = firstLine.split(" ")[9].split(")")[0].toInt(&ok1);
	posImage[2] = 0;
	
	if (!ok0 || !ok1)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}
	for (int i = 0; i < 3; i++)
	{
		mPoint[i] = pos[i];
		mImagePoint[i] = posImage[i];
	}

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("User: "))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString userLine;
	in >> userLine;
	QStringList users = userLine.split(";");
	for (int i = 0; i < users.size(); i++)
		mUsers.push_back(users[i]);

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Color Type:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString colorType;
	in >> colorType;

	mColor = str2colortype(colorType.toStdString());
	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Note Start:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}

	QString content = in.readAll();
	QString text = content.split("\nLinked Images:\n")[0];
	QString imagePaths = content.split("\nLinked Images:\n")[1];
	QStringList imagePathList = imagePaths.split("\n");
	QDir dir(*mPath);
	for (int i = 0; i < imagePathList.size(); i++)
	{
		qDebug()<<"Image Path"<<imagePathList[i];
		QFileInfo finfo(dir.absoluteFilePath(imagePathList[i]));
		if (!finfo.exists())
			continue;
		QString extension = finfo.suffix().toLower();
		if (extension != tr("png") && extension != tr("jpg") && extension != tr("jpeg") && extension != tr("tif") && extension != tr("bmp"))
			continue;
		addImage(finfo.absoluteFilePath());
	}
	this->setText(text);

	QString label;
	label.append(QString("Point Note: Center (") + QString::number(mPoint[0]) + QString(", ") + QString::number(mPoint[1]) + QString(")"));
	QString info;
	info.append(QString("Point Note: World Coordinate (") + QString::number(mPoint[0]) + QString(", ") + QString::number(mPoint[1]) + QString(") Image Coordinate (")
		+ QString::number(mImagePoint[0]) + QString(", ") + QString::number(mImagePoint[1]) + QString(")\n"));
	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));

	info.append(QString("Color Type:\n"));
	info.append(colorType);
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	mFile->close();
	qDebug() << "finish Point instructor";
	isSucceed = true;
}

void PointNote2D::removePointNote2D()
{
	qDebug() << "Remove Point Note 2D" << mFile->fileName();
	mFile->remove();
	this->hideNote(); 
}

SurfaceNote2D::SurfaceNote2D(QString path, const double* point, const int* pointImage, const int noteId, const ColorType type, const QString user)
	: Note(noteId, type)
{
	mPath = new QString(path);
    mPoint = new double[4];
	mImagePoint = new int[4];
	mFileName = new QString(path);
	//mFileName->append(QDir::separator() + QString("Note") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	qsrand(time(NULL));
	int number = qrand();
	mFileName->append(QDir::separator() + QString("SurfaceNote2D") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	QString mUser = user;
	if (user == QString())
		mUsers.push_back(QString("Unknown"));
	else
	{
		mUser.simplified();
		mUser.replace("; ", ";");
		mUser.replace(" ;", ";");
		QStringList users = mUser.split(";");
		for (int i = 0; i < users.size(); i++)
			mUsers.push_back(users[i]);
	}

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
	for (int i = 0; i < 4; i++)
	{
		mPoint[i] = point[i];
		mImagePoint[i] = pointImage[i];
	}

	QString label;
	label.append(QString("Surface Note: Start (") + QString::number(point[0]) + QString(", ") + QString::number(point[1]) + QString(") End (") 
		+ QString::number(point[2]) + QString(", ") + QString::number(point[3]) + QString(")"));
	QString info(label);
	info.append(" Image Coordinate Start (" + QString::number(pointImage[0]) + QString(", ") + QString::number(pointImage[1]) + QString(") End (") 
		+ QString::number(pointImage[2]) + QString(", ") + QString::number(pointImage[3]) + QString(")\n"));
	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));

	info.append(QString("Color Type:\n"));
	info.append(QString(colortype2str(mColor).c_str()));
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	qDebug() << "finish Surface Note 2D instructor";
}

SurfaceNote2D::SurfaceNote2D(QString path, QString fileName, const int noteId, bool& isSucceed)
	: Note(noteId)
{	
	mPath = new QString(path);
	mPoint = new double[4];
	mImagePoint = new int[4];
	vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
	
	mFileName = new QString(path);
	mFileName->append(QDir::separator() + fileName);

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
    if (!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Open Note file " << this->mNoteId << " " << mFileName << " Failed"; 
		isSucceed = false;
		return;
	}
	
    QTextStream in(mFile);
    QString firstLine = in.readLine();
	if (firstLine.split(" ").size() != 16)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}
	bool ok0, ok1, ok2, ok3;
	double pos[4];
	pos[0] = firstLine.split(" ")[3].split(",")[0].split("(")[1].toDouble(&ok0);
	pos[1] = firstLine.split(" ")[4].split(")")[0].toDouble(&ok1);
	pos[2] = firstLine.split(" ")[6].split(",")[0].split("(")[1].toDouble(&ok2);
	pos[3] = firstLine.split(" ")[7].split(")")[0].toDouble(&ok3);
	
	if (!ok0 || !ok1 || !ok2 || !ok3)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}
	int posImage[4];
	posImage[0] = firstLine.split(" ")[11].split(",")[0].split("(")[1].toInt(&ok0);
	posImage[1] = firstLine.split(" ")[12].split(")")[0].toInt(&ok1);
	posImage[2] = firstLine.split(" ")[14].split(",")[0].split("(")[1].toInt(&ok2);
	posImage[3] = firstLine.split(" ")[15].split(")")[0].toInt(&ok3);
	
	if (!ok0 || !ok1 || !ok2 || !ok3)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		mPoint[i] = pos[i];
		mImagePoint[i] = posImage[i];
	}

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("User: "))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString userLine;
	in >> userLine;
	QStringList users = userLine.split(";");
	for (int i = 0; i < users.size(); i++)
		mUsers.push_back(users[i]);

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Color Type:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString colorType;
	in >> colorType;

	mColor = str2colortype(colorType.toStdString());
	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Note Start:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}

	QString content = in.readAll();
	QString text = content.split("\nLinked Images:\n")[0];
	QString imagePaths = content.split("\nLinked Images:\n")[1];
	QStringList imagePathList = imagePaths.split("\n");
	QDir dir(*mPath);
	for (int i = 0; i < imagePathList.size(); i++)
	{
		qDebug()<<"Image Path"<<imagePathList[i];
		QFileInfo finfo(dir.absoluteFilePath(imagePathList[i]));
		if (!finfo.exists())
			continue;
		QString extension = finfo.suffix().toLower();
		if (extension != tr("png") && extension != tr("jpg") && extension != tr("jpeg") && extension != tr("tif") && extension != tr("bmp"))
			continue;
		addImage(finfo.absoluteFilePath());
	}
	this->setText(text);

	QString label;
	label.append(QString("Surface Note: Start (") + QString::number(mPoint[0]) + QString(", ") + QString::number(mPoint[1]) + QString(") End (") 
		+ QString::number(mPoint[2]) + QString(", ") + QString::number(mPoint[3]) + QString(")"));
	this->setLabel(label);
	QString info(label);
	info.append(" Image Coordinate Start (" + QString::number(mImagePoint[0]) + QString(", ") + QString::number(mImagePoint[1]) + QString(") End (") 
		+ QString::number(mImagePoint[2]) + QString(", ") + QString::number(mImagePoint[3]) + QString(")\n"));

	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));

	info.append(QString("Color Type:\n"));
	info.append(colorType);
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	mFile->close();
	qDebug() << "finish Surface instructor";
	isSucceed = true;
}

void SurfaceNote2D::removeSurfaceNote2D()
{
	qDebug() << "Remove Surface Note 2D" << mFile->fileName();
	mFile->remove();
	this->hideNote();
}

PolygonNote2D::PolygonNote2D(QString path, const std::vector<std::pair<double, double> >* polygon, const std::vector<std::pair<int, int> >* polygonImage, const int noteId, const ColorType type, const QString user)
	: Note(noteId, type)
{
	mPath = new QString(path);
    mPolygon = new std::vector<std::pair<double, double> >;
	mPolygonImage = new std::vector<std::pair<int, int> >;
	mFileName = new QString(path);
	//mFileName->append(QDir::separator() + QString("Note") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	qsrand(time(NULL));
	int number = qrand();
	mFileName->append(QDir::separator() + QString("PolygonNote2D") + WORD_SEPARATOR + QString::number(qHash(QString::number(number))) + QString(".txt"));
	QString mUser = user;
	if (user == QString())
		mUsers.push_back(QString("Unknown"));
	else
	{
		mUser.simplified();
		mUser.replace("; ", ";");
		mUser.replace(" ;", ";");
		QStringList users = mUser.split(";");
		for (int i = 0; i < users.size(); i++)
			mUsers.push_back(users[i]);
	}

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);

	std::vector<std::pair<double, double> >::const_iterator it;
	for (it = polygon->begin(); it != polygon->end(); it++)
	{
		mPolygon->push_back(std::pair<double, double>(it->first, it->second));
	}
	std::vector<std::pair<int, int> >::const_iterator itImage;
	for (itImage = polygonImage->begin(); itImage != polygonImage->end(); itImage++)
	{
		mPolygonImage->push_back(std::pair<int, int>(itImage->first, itImage->second));
	}

	QString label;
	label.append(QString("Polygon Note: World Coordinate ") + QString::number(polygon->size()));
	for (it = polygon->begin(); it != polygon->end(); it++)
	{
		label.append(QString("\n(") + QString::number(it->first) + QString(", ") + QString::number(it->second) + QString(")"));
	}
	QString info;
	info.append(QString("Polygon Note: World Coordinate ") + QString::number(polygon->size()));
	for (it = polygon->begin(); it != polygon->end(); it++)
	{
		info.append(QString(" (") + QString::number(it->first) + QString(", ") + QString::number(it->second) + QString(")"));
	}
	info.append(QString(" Image Coordinate ") + QString::number(polygonImage->size()));
	for (itImage = polygonImage->begin(); itImage != polygonImage->end(); itImage++)
	{
		info.append(QString(" (") + QString::number(itImage->first) + QString(", ") + QString::number(itImage->second) + QString(")"));
	}
	info.append(QString("\n"));

	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(QString(colortype2str(mColor).c_str()));
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	qDebug() << "finish Polygon Note 2D instructor";
	
}
PolygonNote2D::PolygonNote2D(QString path, QString fileName, const int noteId, bool& isSucceed)
	: Note(noteId)
{
	mPath = new QString(path);
	mPolygon = new std::vector<std::pair<double, double> >;
	mPolygonImage = new std::vector<std::pair<int, int> >;
	vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
	
	mFileName = new QString(path);
	mFileName->append(QDir::separator() + fileName);

	qDebug(mFileName->toLatin1());
	mFile = new QFile(*mFileName);
    if (!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Open Note file " << this->mNoteId << " " << mFileName << " Failed"; 
		isSucceed = false;
		return;
	}

    QTextStream in(mFile);
    QString firstLine = in.readLine();
	bool okSize;
	int polygonSize = firstLine.split(" ")[4].toInt(&okSize);
	if (firstLine.split(" ").size() != 4 * polygonSize + 8)
	{
		qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
		isSucceed = false;
		return;
	}

	for (int i = 1; i <= polygonSize; i++)
	{
		bool okX, okY;
		double posX, posY;
		posX = firstLine.split(" ")[2*i+3].split(",")[0].split("(")[1].toDouble(&okX);
		posY = firstLine.split(" ")[2*i+4].split(")")[0].toDouble(&okY);
		if (!okX || !okY)
		{
			qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
			isSucceed = false;
			return;
		} 
		mPolygon->push_back(std::pair<double, double>(posX, posY));
		bool okImgX, okImgY;
		int posImageX, posImageY;
		posImageX = firstLine.split(" ")[2*i + 2*polygonSize + 6].split(",")[0].split("(")[1].toInt(&okImgX);
		posImageY = firstLine.split(" ")[2*i + 2*polygonSize + 7].split(")")[0].toInt(&okImgY);
		if (!okImgX || !okImgY)
		{
			qDebug() << "The Syntax of First Line is incorrect. The First Line is " << firstLine;
			isSucceed = false;
			return;
		} 
		mPolygonImage->push_back(std::pair<int, int>(posImageX, posImageY));
	}

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("User: "))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString userLine;
	in >> userLine;
	QStringList users = userLine.split(";");
	for (int i = 0; i < users.size(); i++)
		mUsers.push_back(users[i]);

	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Color Type:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}
	QString colorType;
	in >> colorType;

	mColor = str2colortype(colorType.toStdString());
	while(!in.atEnd())
	{
		QString signal = in.readLine();
		if (signal == QString("Note Start:"))
			break;
	}
	if (in.atEnd())
	{
		isSucceed = false;
		return;
	}

	QString content = in.readAll();
	QString text = content.split("\nLinked Images:\n")[0];
	QString imagePaths = content.split("\nLinked Images:\n")[1];
	QStringList imagePathList = imagePaths.split("\n");
	QDir dir(*mPath);
	for (int i = 0; i < imagePathList.size(); i++)
	{
		qDebug()<<"Image Path"<<imagePathList[i];
		QFileInfo finfo(dir.absoluteFilePath(imagePathList[i]));
		if (!finfo.exists())
			continue;
		QString extension = finfo.suffix().toLower();
		if (extension != tr("png") && extension != tr("jpg") && extension != tr("jpeg") && extension != tr("tif") && extension != tr("bmp"))
			continue;
		addImage(finfo.absoluteFilePath());
	}
	this->setText(text);

	QString label;
	label.append(QString("Polygon Note: World Coordinate ") + QString::number(mPolygon->size()));
	std::vector<std::pair<double, double> >::const_iterator it;
	for (it = mPolygon->begin(); it != mPolygon->end(); it++)
	{
		label.append(QString("\n(") + QString::number(it->first) + QString(", ") + QString::number(it->second) + QString(")"));
	}
	QString info;
	info.append(QString("Polygon Note: World Coordinate ") + QString::number(mPolygon->size()));
	for (it = mPolygon->begin(); it != mPolygon->end(); it++)
	{
		info.append(QString(" (") + QString::number(it->first) + QString(", ") + QString::number(it->second) + QString(")"));
	}
	info.append(QString(" Image Coordinate ") + QString::number(mPolygonImage->size()));
	std::vector<std::pair<int, int> >::const_iterator itImage;
	for (itImage = mPolygonImage->begin(); itImage != mPolygonImage->end(); itImage++)
	{
		info.append(QString(" (") + QString::number(itImage->first) + QString(", ") + QString::number(itImage->second) + QString(")"));
	}
	info.append(QString("\n"));
	
	QString userLabel = QString("User: ");
	QString userInfo;
	for (int i = 0; i < mUsers.size(); i++)
	{
		userInfo.append(mUsers[i]);
		if (i != mUsers.size() - 1)
			userInfo.append(QString(";"));
	}
	label.append(QString("\n") + userLabel + userInfo);
	this->setLabel(label);
	info.append(userLabel + QString("\n") + userInfo + QString("\n"));
	info.append(QString("Color Type:\n"));
	info.append(QString(colortype2str(mColor).c_str()));
	info.append(QString("\nNote Start:"));
	this->setInfo(info);
	mFile->close();
	qDebug() << "finish Polygon Note 2D instructor";
	isSucceed = true;

	
}
void PolygonNote2D::removePolygonNote2D()
{
	qDebug() << "Remove Polygon Note 2D" << mFile->fileName();
	mFile->remove();
	this->hideNote();
	
}