/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

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

#include "informationWidget.h"
#include "../mainWindow.h"

DTextEdit::DTextEdit(QWidget *parent)
    : QWidget(parent)
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("Format Actions"));

    actionTextBold = new QAction(QIcon::fromTheme("format-text-bold", QIcon(":/images/textbold.png")),
                                 tr("&Bold"), this);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
        QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    actionTextItalic = new QAction(QIcon::fromTheme("format-text-italic", QIcon(":/images/textitalic.png")),
                                   tr("&Italic"), this);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    actionTextUnderline = new QAction(QIcon::fromTheme("format-text-underline", QIcon(":/images/textunder.png")),
                                      tr("&Underline"), this);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);


    QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    // Make sure the alignLeft  is always left of the alignRight
    if (QApplication::isLeftToRight()) {
        actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(":/images/textleft.png")),
                                      tr("&Left"), grp);
        actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(":/images/textcenter.png")), tr("C&enter"), grp);
        actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(":/images/textright.png")), tr("&Right"), grp);
    } else {
        actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(":/images/textright.png")), tr("&Right"), grp);
        actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(":/images/textcenter.png")), tr("C&enter"), grp);
        actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(":/images/textleft.png")), tr("&Left"), grp);
    }
    actionAlignJustify = new QAction(QIcon::fromTheme("format-justify-fill", QIcon(":/images/textjustify.png")), tr("&Justify"), grp);

    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    tb->addActions(grp->actions());

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = new QAction(pix, tr("&Color..."), this);
    connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
    tb->addAction(actionTextColor);

    tb = new QToolBar(this);
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    tb->setWindowTitle(tr("Format Actions"));

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    connect(comboStyle, SIGNAL(activated(int)),
            this, SLOT(textStyle(int)));

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, SIGNAL(activated(QString)),
            this, SLOT(textFamily(QString)));

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    QFontDatabase db;
    foreach(int size, db.standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(activated(QString)),
            this, SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font()
                                                                   .pointSize())));

    QHBoxLayout* hlay1 = new QHBoxLayout();
    hlay1->addWidget(tb);

    QHBoxLayout* hlay2 = new QHBoxLayout();
    QTextEdit* textBox = new QTextEdit(this);
    textBox->setReadOnly(true);
    hlay2->addWidget(tb);

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addLayout(hlay1);
    vbox->addLayout(hlay2);
    this->setLayout(vbox);
    this->show();
}

DTextEdit::~DTextEdit()
{

}

Information::Information(QWidget *parent)
    : QWidget(parent)
{
  int width = this->frameGeometry().width();
  int height = this->frameGeometry().height();

  hlay1 = new QHBoxLayout();
  dText = new QTextBrowser(this);
  dText->setReadOnly(true);
  hlay1->addWidget(dText);

  hlay2 = new QHBoxLayout();
  saveButton = new QPushButton("Save");
  removeButton = new QPushButton("Remove");
  saveButton->setEnabled(false);
  removeButton->setEnabled(false);

  connect(dText, SIGNAL(textChanged()), this, SLOT(annotationChanges()));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveAnnotation()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeAnnotation()));
  connect(this, SIGNAL(saveAll()), this, SLOT(saveAnnotation()));
  connect(this, SIGNAL(updateMenu()), mw(), SLOT(updateMenus()));

  hlay2->addWidget(saveButton);
  hlay2->addWidget(removeButton);

  vlay = new QVBoxLayout();
  vlay->addLayout(hlay1);
  vlay->addLayout(hlay2);

  this->setLayout(vlay);
  skipTextChange = false;
  this->show();
  
}

Information::~Information()
{
    delete dText;
}

void Information::refresh()
{
	dText->clear();
	content.clear();
	mPointNotes.clear();
	mSurfaceNotes.clear();
	mFrustumNotes.clear();
	mPointNotes2D.clear();
	mSurfaceNotes2D.clear();
	hasNotesRemoved.clear();
}

void Information::initAnnotation(const QString path)
{
	QString objectPath = QDir::toNativeSeparators(path);
	notePath = objectPath;
	notePath.append(QDir::separator() + QString("Note"));
	loadPointNote(objectPath);
	loadSurfaceNote(objectPath);
	loadFrustumNote(objectPath);
	loadPointNote2D(objectPath);
	loadSurfaceNote2D(objectPath);
	
	QString fileName = notePath;
	fileName.append(QDir::separator() + QString("Annotation.txt"));
	QFile file(fileName);
	//qDebug()<<"Load Note"<<notePath;
	hasNotesRemoved[notePath] = false;
	content[notePath].second = true;
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
	
	QTextStream in(&file);
	QString text = in.readAll();
	content[notePath] = std::make_pair(text, true);
	file.close();
}

void Information::annotationChanges()
{
	if (skipTextChange)
		return;
	updateCurrentPath();
	if (dText->toPlainText() != content[notePath].first)
	{
		content[notePath] = std::make_pair(dText->toPlainText(), false);
	}
}

void Information::startAnnotation()
{
	updateCurrentPath();
	saveButton->setEnabled(true);
	removeButton->setEnabled(true);
	dText->setReadOnly(false);
}

void Information::finishAnnotation()
{
	updateCurrentPath();
	dText->setReadOnly(true);
	saveButton->setEnabled(false);
	removeButton->setEnabled(false);
	hideNotes();
}

void Information::reloadAnnotation()
{
	updateCurrentPath();
	//bool saved = content[notePath].second;
	skipTextChange = true;
	dText->setPlainText(content[notePath].first);
	skipTextChange = false;
	//content[notePath].second = saved;
}

void Information::clearAnnotation()
{
	updateCurrentPath();
	//bool saved = content[notePath].second;
	skipTextChange = true;
	dText->clear();
	skipTextChange = false;
	//content[notePath].second = saved;
}

void Information::saveAnnotation()
{
	updateCurrentPath();
	if (dText->toPlainText() == QString())
		return;
	QString fileName = notePath;
	fileName.append(QDir::separator() + QString("Annotation.txt"));
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Open Annotation File Failed"; 
        return;
	}
    QTextStream out(&file);
    out << dText->toPlainText() << "\n";
	content[notePath].second = true;
	file.close();
}

void Information::removeAnnotation()
{
	updateCurrentPath();
	QString fileName = notePath;
	qDebug() << "removeAnnotation " << notePath;
	fileName.append(QDir::separator() + QString("Annotation.txt"));
	content.remove(notePath);
	hasNotesRemoved[notePath] = true;
	dText->clear();
	QFile file(fileName);
	file.remove();
}

void Information::createPointNote(double *pos, int cellId, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	updateCurrentPath();
	int size = mPointNotes[notePath].size();
	qDebug() << "Create Point Note, current size = " << size;
	PointNote* newNote = new PointNote(notePath, pos, cellId, size, color);
	newNote->showNote();
	mPointNotes[notePath].push_back(newNote);
	connect(mPointNotes[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPointNotes[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPointNotes[notePath][size], SLOT(close()));
}

void Information::createSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	updateCurrentPath();
	int size = mSurfaceNotes[notePath].size();
	qDebug() << "Create Surface Note, current size = " << size;
	SurfaceNote* newNote = new SurfaceNote(notePath, cellIds, size, color);
	newNote->showNote();
	mSurfaceNotes[notePath].push_back(newNote);
	connect(mSurfaceNotes[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mSurfaceNotes[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mSurfaceNotes[notePath][size], SLOT(close()));
}

void Information::createFrustumNote(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	updateCurrentPath();
	int size = mFrustumNotes[notePath].size();
	qDebug() << "Create Frustum Note, current size = " << size;
	FrustumNote* newNote = new FrustumNote(notePath, points, normals, size, color);
	newNote->showNote();
	mFrustumNotes[notePath].push_back(newNote);
	connect(mFrustumNotes[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removeFrustumNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mFrustumNotes[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mFrustumNotes[notePath][size], SLOT(close()));

}

void Information::createPointNote2D(double* point,  int* pointImage, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	updateCurrentPath();
	int size = mPointNotes2D[notePath].size();
	qDebug() << "Create Point Note 2D, current size = " << size;
	PointNote2D* newNote = new PointNote2D(notePath, point, pointImage, size, color);
	newNote->showNote();
	mPointNotes2D[notePath].push_back(newNote);
	connect(mPointNotes2D[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPointNotes2D[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPointNotes2D[notePath][size], SLOT(close()));
}

void Information::createSurfaceNote2D(double* point, int* pointImage, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	updateCurrentPath();
	int size = mSurfaceNotes2D[notePath].size();
	qDebug() << "Create Surface Note 2D, current size = " << size;
	SurfaceNote2D* newNote = new SurfaceNote2D(notePath, point, pointImage, size, color);
	newNote->showNote();
	mSurfaceNotes2D[notePath].push_back(newNote);
	connect(mSurfaceNotes2D[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mSurfaceNotes2D[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mSurfaceNotes2D[notePath][size], SLOT(close()));
}

bool Information::loadPointNote(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"PointNote_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	bool isLoadSucceed = true;
	bool isSucceed;
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		PointNote* newNote = new PointNote(notePath, fileList[i], i, isSucceed);
		if (!isSucceed)
		{
			isLoadSucceed = false;
			continue;
		}
		mPointNotes[notePath].push_back(newNote);
		connect(mPointNotes[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote(int, QString*)));
		connect(this, SIGNAL(saveAll()), mPointNotes[notePath][i], SLOT(save()));
		connect(this, SIGNAL(closeAll()), mPointNotes[notePath][i], SLOT(close()));
		mPointNotes[notePath][i]->setSaved(true);
		if (mw()->VTKA(path))
			mw()->VTKA(path)->loadPointNoteMark(newNote->getCellId(), newNote->getColorType());
		//else
		//	qDebug()<<"Cannot find the window!!!!";

		
	}
	qDebug() << "Load "<<mPointNotes[notePath].size() <<" Point Note";
	return isLoadSucceed;
}

bool Information::loadSurfaceNote(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"SurfaceNote_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	bool isLoadSucceed = true;
	bool isSucceed;
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		SurfaceNote* newNote = new SurfaceNote(notePath, fileList[i], i, isSucceed);
		if (!isSucceed)
		{
			qDebug()<<"!!!!!";
			isLoadSucceed = false;
			continue;
		}
		mSurfaceNotes[notePath].push_back(newNote);
		connect(mSurfaceNotes[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote(int, QString*)));
		connect(this, SIGNAL(saveAll()), mSurfaceNotes[notePath][i], SLOT(save()));
		connect(this, SIGNAL(closeAll()), mSurfaceNotes[notePath][i], SLOT(close()));
		mSurfaceNotes[notePath][i]->setSaved(true);
		if (mw()->VTKA(path))
			mw()->VTKA(path)->loadSurfaceNoteMark(newNote->getCellIds(), newNote->getColorType());
		//else
		//	qDebug()<<"Cannot find the window!!!!";
	}
	qDebug() << "Load "<<mSurfaceNotes[notePath].size()<<" Surface Note";
	return isLoadSucceed;
}

bool Information::loadFrustumNote(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"FrustumNote_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	bool isLoadSucceed = true;
	bool isSucceed;
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		bool isSucceed;
		FrustumNote* newNote = new FrustumNote(notePath, fileList[i], i, isSucceed);
		if (!isSucceed)
		{
			isLoadSucceed = false;
			continue;
		}
		mFrustumNotes[notePath].push_back(newNote);
		connect(mFrustumNotes[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removeFrustumNote(int, QString*)));
		connect(this, SIGNAL(saveAll()), mFrustumNotes[notePath][i], SLOT(save()));
		mFrustumNotes[notePath][i]->setSaved(true);
		connect(this, SIGNAL(closeAll()), mFrustumNotes[notePath][i], SLOT(close()));
		if (mw()->VTKA(path))
			mw()->VTKA(path)->loadFrustumNoteMark(newNote->getPoints(), newNote->getNormals(), newNote->getColorType());
		//else
		//	qDebug()<<"Cannot find the window!!!!";

	}
	qDebug() << "Load "<<mFrustumNotes[notePath].size()<<" Frustum Note";
	return isLoadSucceed;
}

bool Information::loadPointNote2D(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"PointNote2D_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	bool isLoadSucceed = true;
	bool isSucceed;
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		PointNote2D* newNote = new PointNote2D(notePath, fileList[i], i, isSucceed);
		if (!isSucceed)
		{
			isLoadSucceed = false;
			continue;
		}
		mPointNotes2D[notePath].push_back(newNote);
		connect(mPointNotes2D[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote2D(int, QString*)));
		connect(this, SIGNAL(saveAll()), mPointNotes2D[notePath][i], SLOT(save()));
		connect(this, SIGNAL(closeAll()), mPointNotes2D[notePath][i], SLOT(close()));
		mPointNotes2D[notePath][i]->setSaved(true);
		if (mw()->VTKA(path))
			mw()->VTKA(path)->loadPointNote2DMark(newNote->getPoint(), newNote->getColorType());
		//else
		//	qDebug()<<"Cannot find the window!!!!";
	}
	qDebug()<<"Load Point Note2d "<<notePath;
	qDebug() << "Load "<<mPointNotes2D[notePath].size()<<" Point Note";
	return isLoadSucceed;
}

bool Information::loadSurfaceNote2D(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"SurfaceNote2D_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	bool isLoadSucceed = true;
	bool isSucceed;
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		SurfaceNote2D* newNote = new SurfaceNote2D(notePath, fileList[i], i, isSucceed);
		if (!isSucceed)
		{
			isLoadSucceed = false;
			continue;
		}
		mSurfaceNotes2D[notePath].push_back(newNote);
		connect(mSurfaceNotes2D[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote2D(int, QString*)));
		connect(this, SIGNAL(saveAll()), mSurfaceNotes2D[notePath][i], SLOT(save()));
		connect(this, SIGNAL(closeAll()), mSurfaceNotes2D[notePath][i], SLOT(close()));
		mSurfaceNotes2D[notePath][i]->setSaved(true);
		if (mw()->VTKA(path))
			mw()->VTKA(path)->loadSurfaceNote2DMark(newNote->getPoint(), newNote->getColorType());
		//else
		//	qDebug()<<"Cannot find the window!!!!";

	}
	qDebug() << "Load "<<mSurfaceNotes2D[notePath].size()<<" Surface Note";
	return isLoadSucceed;
}

void Information::openPointNote(int cellId)
{
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		if (mPointNotes[notePath][i]->getCellId() == cellId)
		{
			mPointNotes[notePath][i]->hideNote();
			mPointNotes[notePath][i]->showNote();
			break;
		}
	}
}

void Information::openSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds)
{
	updateCurrentPath();
	if (!cellIds->GetSelectionList())
	{
		qDebug() << "Cannot Find the Surface Note to Open!" << endl;
		return;
	}
	bool isOpen = false;
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		if (cellIds->GetSelectionList()->GetSize() != mSurfaceNotes[notePath][i]->getCellIds()->GetSelectionList()->GetSize())
			continue;
		bool elementExist = true;
	    for (int j = 0; j < mSurfaceNotes[notePath][i]->getCellIds()->GetSelectionList()->GetSize(); j++)
	    {
		    if (cellIds->GetSelectionList()->LookupValue(mSurfaceNotes[notePath][i]->getCellIds()->GetSelectionList()->GetVariantValue(j)) == -1)
		    {
			    elementExist = false;
			    break;
		    }
	    }
	    if (elementExist)
	    {
		    mSurfaceNotes[notePath][i]->hideNote();
			mSurfaceNotes[notePath][i]->showNote();
		    isOpen = true;
		    break;
	    }
	}
	if (!isOpen)	qDebug() << "Cannot Find the Exact SurfaceNote to open!"<<endl;
}

void Information::openFrustumNote(vtkSmartPointer<vtkPlanes> planes)
{
	updateCurrentPath();
	if (planes->GetNumberOfPlanes() != 6)
	{
		qDebug() << "Cannot Find the Frustum Note to Open!" << endl;
		return;
	}
	bool isFound = false;
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		bool elementExist = true;
		for (int j = 0; j < 6; j++)
	    {
			vtkSmartPointer<vtkPlane> plane = planes->GetPlane(i);
			vtkSmartPointer<vtkPoints> points = mFrustumNotes[notePath][i]->getPoints();
			vtkSmartPointer<vtkDataArray> normals = mFrustumNotes[notePath][i]->getNormals();
			bool planeExist = false;
			for (int k = 0; k < 6; k++)
			{
				double pos[3], norm[3], planeNorm[3];
				points->GetPoint(k, pos);
				norm[0] = normals->GetVariantValue(3*k).ToDouble();
				norm[1] = normals->GetVariantValue(3*k + 1).ToDouble();
				norm[2] = normals->GetVariantValue(3*k + 2).ToDouble();
				plane->GetNormal(planeNorm);
				if (plane->EvaluateFunction(pos) == 0 && norm[0] == planeNorm[0] && norm[1] == planeNorm[1] && norm[2] == planeNorm[2])
				{
					planeExist = true;
					break;
				}
			}
			if (!planeExist)
			{
				elementExist = false;
				break;
			}
	   }
	    if (elementExist)
	    {
		    mFrustumNotes[notePath][i]->hideNote();
			mFrustumNotes[notePath][i]->showNote();
		    isFound = true;
		    break;
	    }
	}
	if (!isFound)	qDebug() << "Cannot Find the Exact FrustumNote to open!"<<endl;
}

void Information::openPointNote2D(double* point)
{
	updateCurrentPath();
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		double* select = mPointNotes2D[notePath][i]->getPoint();
		bool isSame = true;
		for (int j = 0; j  < 3; j++)
		{
			if (select[j] != point[j])
			{
				isSame = false;
				break;
			}
		}
		if (isSame)
		{
			mPointNotes2D[notePath][i]->hideNote();
			mPointNotes2D[notePath][i]->showNote();
			break;
		}
	}
}

void Information::openSurfaceNote2D(double* point)
{
	updateCurrentPath();
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		double* select = mSurfaceNotes2D[notePath][i]->getPoint();
		bool isSame = true;
		for (int j = 0; j  < 4; j++)
		{
			if (select[j] != point[j])
			{
				isSame = false;
				break;
			}
		}
		if (isSame)
		{
			mSurfaceNotes2D[notePath][i]->hideNote();
			mSurfaceNotes2D[notePath][i]->showNote();
			break;
		}
	}
}

void Information::removePointNote(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[*path].size(); ++i) 
	{
		if (mPointNotes[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removePointNoteMark(mPointNotes[*path][i]->getCellId());
			mPointNotes[*path].remove(i);
			hasNotesRemoved[notePath] = true;
			break;
		}
	}
}

void Information::removeSurfaceNote(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	updateCurrentPath();
	for (int i = 0; i < mSurfaceNotes[*path].size(); ++i) 
	{
		if (mSurfaceNotes[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[*path][i]->getCellIds());
			mSurfaceNotes[*path].remove(i);
			hasNotesRemoved[notePath] = true;
			break;
		}
	}
}

void Information::removeFrustumNote(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	updateCurrentPath();
	for (int i = 0; i < mFrustumNotes[*path].size(); ++i) 
	{
		if (mFrustumNotes[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[*path][i]->getPoints(), mFrustumNotes[*path][i]->getNormals());
			mFrustumNotes[*path].remove(i);
			hasNotesRemoved[notePath] = true;
			break;
		}
	}
}

void Information::removePointNote2D(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	updateCurrentPath();
	for (int i = 0; i < mPointNotes2D[*path].size(); ++i) 
	{
		if (mPointNotes2D[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removePointNote2DMark(mPointNotes2D[*path][i]->getPoint());
			mPointNotes2D[*path].remove(i);
			hasNotesRemoved[notePath] = true;
			break;
		}
	}
}

void Information::removeSurfaceNote2D(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	updateCurrentPath();
	for (int i = 0; i < mSurfaceNotes2D[*path].size(); ++i) 
	{
		if (mSurfaceNotes2D[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[*path][i]->getPoint());
			mSurfaceNotes2D[*path].remove(i);
			hasNotesRemoved[notePath] = true;
			break;
		}
	}
}

void Information::openNoteFromTreeWidget(QTreeWidgetItem* item)
{
	QString path = item->text(3);
	QString file = item->text(4);
	int slashpos = path.lastIndexOf(QDir::separator());
	QString projectPath = path;
	projectPath.chop(path.length() - slashpos);
	//mw()->switchSubWindow(projectPath);
	if (path == QString("Project Info"))
	{
		mw()->showProjectInfo();
		return;
	}
	else if (path == QString("File Info"))
	{
		mw()->showFileInfo();
		return;
	}
	QString fileName = path;
	fileName.append(QDir::separator() + file);
	QString type = file.split("_")[0];
	QStringList options;
	options << "Annotation.txt" << "PointNote" << "SurfaceNote" << "FrustumNote" << "PointNote2D" << "SurfaceNote2D";
	QTabWidget* tab = new QTabWidget();

	switch(options.indexOf(type))
	{
		case 0:	
			tab = mw()->getSearchTabWidget();
			tab->setCurrentIndex(0);
			break;
		case 1:
			for (int i = 0; i < mPointNotes[path].size(); i++)
			{
				if (mPointNotes[path][i]->getFileName() == fileName)
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mPointNotes[path][i]->hideNote();
					mPointNotes[path][i]->showNote();
					break;
				}
			}
			break;
		case 2:
			for (int i = 0; i < mSurfaceNotes[path].size(); i++)
			{
				if (mSurfaceNotes[path][i]->getFileName() == fileName)
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mSurfaceNotes[path][i]->hideNote();
					mSurfaceNotes[path][i]->showNote();
					break;
				}
			}
			break;
		case 3:
			for (int i = 0; i < mFrustumNotes[path].size(); i++)
			{
				if (mFrustumNotes[path][i]->getFileName() == fileName)
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mFrustumNotes[path][i]->hideNote();
					mFrustumNotes[path][i]->showNote();
					break;
				}
			}
			break;
		case 4:
			for (int i = 0; i < mPointNotes2D[path].size(); i++)
			{
				if (mPointNotes2D[path][i]->getFileName() == fileName)
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mPointNotes2D[path][i]->hideNote();
					mPointNotes2D[path][i]->showNote();
					break;
				}
			}
			break;
		case 5:
			for (int i = 0; i < mSurfaceNotes2D[path].size(); i++)
			{
				if (mSurfaceNotes2D[path][i]->getFileName() == fileName)
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mSurfaceNotes2D[path][i]->hideNote();
					mSurfaceNotes2D[path][i]->showNote();
					break;
				}
			}
			break;
		default: qDebug() << "Incorrect Note File!";
	}
	mw()->switchSubWindow(projectPath);
}

void Information::openNoteFromNavigation(QTreeWidgetItem* item)
{
	qDebug()<<item->text(0)<<item->text(1);
	QString file = item->text(0);
	QString path = item->text(1);
	QString projectPath = path;
	
	path.append(QDir::separator() + QString("Note"));
	QString type = file.split(" ")[0];
	int number;
	if (type != QString("Annotation"))
		number = file.split(" ")[1].toInt();
	QStringList options;
	options << "Annotation" << "PointNote" << "SurfaceNote" << "FrustumNote";
	QTabWidget* rightTab = new QTabWidget();
	QTabWidget* functionTab = new QTabWidget();

	switch(options.indexOf(type))
	{
		case 0:	
			rightTab = mw()->getRightTabWidget();
			rightTab->setCurrentIndex(0);
			functionTab = mw()->getSearchTabWidget();
			functionTab->setCurrentIndex(0);
			break;
		case 1:
			if (mPointNotes[path].size() >= number)
			{
				if (!mPointNotes[path][number-1]->checkRemoved())
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mPointNotes[path][number-1]->hideNote();
					mPointNotes[path][number-1]->showNote();
				}
			}
			else if (mPointNotes2D[path].size() >= number)
			{
				if (!mPointNotes2D[path][number-1]->checkRemoved())
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mPointNotes2D[path][number-1]->hideNote();
					mPointNotes2D[path][number-1]->showNote();
				}
			}
			else
			{
				qDebug()<<"Incorrect Notes!";
			}
			break;
		case 2:
			if (mSurfaceNotes[path].size() >= number)
			{
				if (!mSurfaceNotes[path][number-1]->checkRemoved())
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mSurfaceNotes[path][number-1]->hideNote();
					mSurfaceNotes[path][number-1]->showNote();
				}
			}
			else if (mSurfaceNotes2D[path].size() >= number)
			{
				if (!mSurfaceNotes2D[path][number-1]->checkRemoved())
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mSurfaceNotes2D[path][number-1]->hideNote();
					mSurfaceNotes2D[path][number-1]->showNote();
				}
			}
			else
			{
				qDebug()<<"Incorrect Notes!";
			}
			break;
		case 3:
			if (mFrustumNotes[path].size() >= number)
			{
				if (!mFrustumNotes[path][number-1]->checkRemoved())
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mFrustumNotes[path][number-1]->hideNote();
					mFrustumNotes[path][number-1]->showNote();
				}
			}
			else
			{
				qDebug()<<"Incorrect Notes!";
			}
			break;
		default: qDebug() << "Incorrect Note File!";
	}
	mw()->switchSubWindow(projectPath);
}


void Information::saveAllNotes()
{
	emit saveAll();
	for (QMap<QString, bool>::iterator it = hasNotesRemoved.begin(); it != hasNotesRemoved.end(); it++)
	{
		it.value() = false;
	}
}

void Information::saveObjectNotes()
{
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		mPointNotes[notePath][i]->save();
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		mSurfaceNotes[notePath][i]->save();
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		mFrustumNotes[notePath][i]->save();
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		mPointNotes2D[notePath][i]->save();
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		mSurfaceNotes2D[notePath][i]->save();
	}
	saveAnnotation();
	hasNotesRemoved[notePath] = false;
}

void Information::closeAllNotes()
{
	emit closeAll();
}

void Information::closeObjectNotes()
{
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		mPointNotes[notePath][i]->hideNote();
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		mSurfaceNotes[notePath][i]->hideNote();
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		mFrustumNotes[notePath][i]->hideNote();
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		mPointNotes2D[notePath][i]->hideNote();
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		mSurfaceNotes2D[notePath][i]->hideNote();
	}

}

void Information::removeAllNotes()
{
	qDebug() << "remove ALL Note ";
	updateCurrentPath();
	hasNotesRemoved[notePath] = true;
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		mw()->VTKA()->removePointNoteMark(mPointNotes[notePath][i]->getCellId());
		mPointNotes[notePath][i]->removePointNote();
		mPointNotes[notePath][i]->setRemoved(true);
	}
	mPointNotes.remove(notePath);
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[notePath][i]->getCellIds());
		mSurfaceNotes[notePath][i]->removeSurfaceNote();
		mSurfaceNotes[notePath][i]->setRemoved(true);
	}
	mSurfaceNotes.remove(notePath);
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[notePath][i]->getPoints(), mFrustumNotes[notePath][i]->getNormals());
		mFrustumNotes[notePath][i]->removeFrustumNote();
		mFrustumNotes[notePath][i]->setRemoved(true);
	}
	mFrustumNotes.remove(notePath);
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		mw()->VTKA()->removePointNote2DMark(mPointNotes2D[notePath][i]->getPoint());
		mPointNotes2D[notePath][i]->removePointNote2D();
		mPointNotes2D[notePath][i]->setRemoved(true);
	}
	mPointNotes2D.remove(notePath);
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[notePath][i]->getPoint());
		mSurfaceNotes2D[notePath][i]->removeSurfaceNote2D();
		mSurfaceNotes2D[notePath][i]->setRemoved(true);
	}
	mSurfaceNotes2D.remove(notePath);
}

void Information::removeAllNotes(QString path)
{
	qDebug() << "remove ALL Note ";
	QString removePath = path;
	removePath.append(QDir::separator() + QString("Note"));
	hasNotesRemoved[removePath] = true;
	for (int i = 0; i < mPointNotes[removePath].size(); ++i) 
	{
		mw()->VTKA()->removePointNoteMark(mPointNotes[removePath][i]->getCellId());
		mPointNotes[removePath][i]->removePointNote();
		mPointNotes[removePath][i]->setRemoved(true);
	}
	mPointNotes.remove(removePath);
	for (int i = 0; i < mSurfaceNotes[removePath].size(); ++i) 
	{
		mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[removePath][i]->getCellIds());
		mSurfaceNotes[removePath][i]->removeSurfaceNote();
		mSurfaceNotes[removePath][i]->setRemoved(true);
	}
	mSurfaceNotes.remove(removePath);
	for (int i = 0; i < mFrustumNotes[removePath].size(); ++i) 
	{
		mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[removePath][i]->getPoints(), mFrustumNotes[removePath][i]->getNormals());
		mFrustumNotes[removePath][i]->removeFrustumNote();
		mFrustumNotes[removePath][i]->setRemoved(true);
	}
	mFrustumNotes.remove(removePath);
	for (int i = 0; i < mPointNotes2D[removePath].size(); ++i) 
	{
		mw()->VTKA()->removePointNote2DMark(mPointNotes2D[removePath][i]->getPoint());
		mPointNotes2D[removePath][i]->removePointNote2D();
		mPointNotes2D[removePath][i]->setRemoved(true);
	}
	mPointNotes2D.remove(removePath);
	for (int i = 0; i < mSurfaceNotes2D[removePath].size(); ++i) 
	{
		mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[removePath][i]->getPoint());
		mSurfaceNotes2D[removePath][i]->removeSurfaceNote2D();
		mSurfaceNotes2D[removePath][i]->setRemoved(true);
	}
	mSurfaceNotes2D.remove(removePath);
}

void Information::removeUnSavedNotes()
{
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		if (!mPointNotes[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removePointNoteMark(mPointNotes[notePath][i]->getCellId());
			mPointNotes[notePath][i]->removePointNote();
			mPointNotes[notePath][i]->setRemoved(true);
			mPointNotes[notePath].remove(i);
		}
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[notePath][i]->getCellIds());
			mSurfaceNotes[notePath][i]->removeSurfaceNote();
			mSurfaceNotes[notePath][i]->setRemoved(true);
			mSurfaceNotes[notePath].remove(i);
		}
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		if (!mFrustumNotes[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[notePath][i]->getPoints(), mFrustumNotes[notePath][i]->getNormals());
			mFrustumNotes[notePath][i]->removeFrustumNote();
			mFrustumNotes[notePath][i]->setRemoved(true);
			mFrustumNotes[notePath].remove(i);		
		}
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		if (!mPointNotes2D[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removePointNote2DMark(mPointNotes2D[notePath][i]->getPoint());
			mPointNotes2D[notePath][i]->removePointNote2D();
			mPointNotes2D[notePath][i]->setRemoved(true);
			mPointNotes2D[notePath].remove(i);
		}
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes2D[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[notePath][i]->getPoint());
			mSurfaceNotes2D[notePath][i]->removeSurfaceNote2D();
			mSurfaceNotes2D[notePath][i]->setRemoved(true);
			mSurfaceNotes2D[notePath].remove(i);
		}
	}
}

void Information::hideNotes()
{
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		mPointNotes[notePath][i]->hideNote();
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		mSurfaceNotes[notePath][i]->hideNote();
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		mFrustumNotes[notePath][i]->hideNote();
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		mPointNotes2D[notePath][i]->hideNote();
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		mSurfaceNotes2D[notePath][i]->hideNote();
	}
}

void Information::showNotes()
{
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		mPointNotes[notePath][i]->showNote();
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		mSurfaceNotes[notePath][i]->showNote();
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		mFrustumNotes[notePath][i]->showNote();
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		mPointNotes2D[notePath][i]->showNote();
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		mSurfaceNotes2D[notePath][i]->showNote();
	}
}

bool Information::updateCurrentPath()
{
	//QString fn;
	if(mw()->VTKA()) {
        notePath = mw()->VTKA()->mProjectPath;
		//fn = mw()->VTKA()->mFilename;
		//qDebug() << "current window: " << fn;
	}
	else
	{
		qDebug() << "Error: cannot find Project Path for Information";
		notePath = QString("");
		return false;
	}
	//QFileInfo finfo(fn);
    //notePath.append(QDir::separator() + QString("Note") + WORD_SEPARATOR + finfo.fileName().simplified().replace(" ", WORD_SEPARATOR));
	notePath.append(QDir::separator() + QString("Note"));
    //if(!QDir(notePath).exists()) QDir().mkdir(notePath);
	//qDebug()<<"note Path "<<notePath;
	return true;
}

bool Information::checkAllSaved()
{
	for (QMap<QString, bool>::iterator it = hasNotesRemoved.begin(); it != hasNotesRemoved.end(); it++)
	{
		QString path = it.key();
		// Test whether there are removed notes and annotation
		if (hasNotesRemoved[path])
			return false;
		// Test whether there are modifications in annotation and notes.
		for (int i = 0; i < mPointNotes[path].size(); ++i) 
		{
			if (!mPointNotes[path][i]->checkSaved())
			{	
				return false;
			}
		}
		for (int i = 0; i < mSurfaceNotes[path].size(); ++i) 
		{
			if (!mSurfaceNotes[path][i]->checkSaved())
			{	
				return false;
			}
		}
		for (int i = 0; i < mFrustumNotes[path].size(); ++i) 
		{
			if (!mFrustumNotes[path][i]->checkSaved())
			{	
				return false;
			}
		}
		for (int i = 0; i < mPointNotes2D[path].size(); ++i) 
		{
			if (!mPointNotes2D[path][i]->checkSaved())
			{	
				return false;
			}
		}
		for (int i = 0; i < mSurfaceNotes2D[path].size(); ++i) 
		{
			if (!mSurfaceNotes2D[path][i]->checkSaved())
			{	
				return false;
			}
		}
		if (!content[path].second)
		{
			return false;
		}
	}
	return true;

}

bool Information::checkObjectSaved()
{
	if (!updateCurrentPath())
		return true;
	// Test whether there are removed notes and annotation
	if (hasNotesRemoved[notePath])
		return false;
	// Test whether there are modifications in annotation and notes.
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		if (!mPointNotes[notePath][i]->checkSaved())
		{	
			return false;
		}
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes[notePath][i]->checkSaved())
		{	
			return false;
		}
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		if (!mFrustumNotes[notePath][i]->checkSaved())
		{	
			return false;
		}
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		if (!mPointNotes2D[notePath][i]->checkSaved())
		{	
			return false;
		}
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes2D[notePath][i]->checkSaved())
		{	
			return false;
		}
	}
	if (!content[notePath].second)
	{
		qDebug()<<"content";
		return false;
	}
	return true;
}

QVector<QString> Information::getAllNotes(const QString objectPath)
{
	QString path = objectPath;
	path = QDir::toNativeSeparators(path);
	path.append(QDir::separator() + QString("Note"));
	QVector<QString> notes;
	if (content.find(path) == content.end())	// cannot find the object, return empty vector
		return notes;
	QString annotation = content[path].first;
	notes.push_back(annotation);
	if (mPointNotes.find(path) != mPointNotes.end())
	{
		for (int i = 0; i < mPointNotes[path].size(); i++)
		{
			if (!mPointNotes[path][i]->checkRemoved())
				notes.push_back(mPointNotes[path][i]->getContent());
		}
	}
	if (mSurfaceNotes.find(path) != mSurfaceNotes.end())
	{
		for (int i = 0; i < mSurfaceNotes[path].size(); i++)
		{
			if (!mSurfaceNotes[path][i]->checkRemoved())
				notes.push_back(mSurfaceNotes[path][i]->getContent());
		}
	}
	if (mFrustumNotes.find(path) != mFrustumNotes.end())
	{
		for (int i = 0; i < mFrustumNotes[path].size(); i++)
		{
			if (!mFrustumNotes[path][i]->checkRemoved())
				notes.push_back(mFrustumNotes[path][i]->getContent());
		}
	}
	if (mPointNotes2D.find(path) != mPointNotes2D.end())
	{
		for (int i = 0; i < mPointNotes2D[path].size(); i++)
		{
			if (!mPointNotes2D[path][i]->checkRemoved())
				notes.push_back(mPointNotes2D[path][i]->getContent());
		}
	}
	if (mSurfaceNotes2D.find(path) != mSurfaceNotes2D.end())
	{
		for (int i = 0; i < mSurfaceNotes2D[path].size(); i++)
		{
			if (!mSurfaceNotes2D[path][i]->checkRemoved())
				notes.push_back(mSurfaceNotes2D[path][i]->getContent());
		}
	}
	return notes;
}

QVector<int> Information::getNoteNumber(const QString objectPath)
{
	//qDebug()<<"get note number"<<objectPath;
	QString path = objectPath;
	path = QDir::toNativeSeparators(path);
	path.append(QDir::separator() + QString("Note"));
	QVector<int> notes;
	if (content.find(path) == content.end())	// cannot find the object, return empty vector
		return notes;
	if(content[path].first == QString())
		notes.push_back(0);
	else
		notes.push_back(1);

	if (mPointNotes.find(path) != mPointNotes.end())
		notes.push_back(mPointNotes[path].size());
	else
		notes.push_back(0);

	if (mSurfaceNotes.find(path) != mSurfaceNotes.end())
		notes.push_back(mSurfaceNotes[path].size());
	else
		notes.push_back(0);

	if (mFrustumNotes.find(path) != mFrustumNotes.end())
		notes.push_back(mFrustumNotes[path].size());
	else
		notes.push_back(0);

	if (mPointNotes2D.find(path) != mPointNotes2D.end())
		notes.push_back(mPointNotes2D[path].size());
	else
		notes.push_back(0);
	
	if (mSurfaceNotes2D.find(path) != mSurfaceNotes2D.end())
		notes.push_back(mSurfaceNotes2D[path].size());
	else
		notes.push_back(0);
	qDebug()<<"!!!!get note number"<<notes.size();
	return notes;
}

MainWindow* Information::mw()
{
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
    if (mainwindow)
    {
      return mainwindow;
    }
  }
}

