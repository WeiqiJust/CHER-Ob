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
	dText->setPlainText(text);
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

void Information::createPointNote2D(double* point, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	updateCurrentPath();
	int size = mPointNotes2D[notePath].size();
	qDebug() << "Create Point Note 2D, current size = " << size;
	PointNote2D* newNote = new PointNote2D(notePath, point, size, color);
	newNote->showNote();
	mPointNotes2D[notePath].push_back(newNote);
	connect(mPointNotes2D[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPointNotes2D[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPointNotes2D[notePath][size], SLOT(close()));
}

void Information::createSurfaceNote2D(double* point, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	updateCurrentPath();
	int size = mSurfaceNotes2D[notePath].size();
	qDebug() << "Create Surface Note 2D, current size = " << size;
	SurfaceNote2D* newNote = new SurfaceNote2D(notePath, point, size, color);
	newNote->showNote();
	mSurfaceNotes2D[notePath].push_back(newNote);
	connect(mSurfaceNotes2D[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mSurfaceNotes2D[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mSurfaceNotes2D[notePath][size], SLOT(close()));
}

void Information::loadPointNote(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"PointNote_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		PointNote* newNote = new PointNote(notePath, fileList[i], i);
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
}

void Information::loadSurfaceNote(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"SurfaceNote_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		SurfaceNote* newNote = new SurfaceNote(notePath, fileList[i], i);
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
}

void Information::loadFrustumNote(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"FrustumNote_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		FrustumNote* newNote = new FrustumNote(notePath, fileList[i], i);
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
}

void Information::loadPointNote2D(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"PointNote2D_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		PointNote2D* newNote = new PointNote2D(notePath, fileList[i], i);
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
}

void Information::loadSurfaceNote2D(const QString path)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"SurfaceNote2D_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		SurfaceNote2D* newNote = new SurfaceNote2D(notePath, fileList[i], i);
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
	mw()->switchSubWindow(projectPath);
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
	bool isOpen = false;
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
					mPointNotes[path][i]->hideNote();
					mPointNotes[path][i]->showNote();
					isOpen = true;
					break;
				}
			}
			break;
		case 2:
			for (int i = 0; i < mSurfaceNotes[path].size(); i++)
			{
				if (mSurfaceNotes[path][i]->getFileName() == fileName)
				{
					mSurfaceNotes[path][i]->hideNote();
					mSurfaceNotes[path][i]->showNote();
					isOpen = true;
					break;
				}
			}
			break;
		case 3:
			for (int i = 0; i < mFrustumNotes[path].size(); i++)
			{
				if (mFrustumNotes[path][i]->getFileName() == fileName)
				{
					mFrustumNotes[path][i]->hideNote();
					mFrustumNotes[path][i]->showNote();
					isOpen = true;
					break;
				}
			}
			break;
		case 4:
			for (int i = 0; i < mPointNotes2D[path].size(); i++)
			{
				if (mPointNotes2D[path][i]->getFileName() == fileName)
				{
					mPointNotes2D[path][i]->hideNote();
					mPointNotes2D[path][i]->showNote();
					isOpen = true;
					break;
				}
			}
			break;
		case 5:
			for (int i = 0; i < mSurfaceNotes2D[path].size(); i++)
			{
				if (mSurfaceNotes2D[path][i]->getFileName() == fileName)
				{
					mSurfaceNotes2D[path][i]->hideNote();
					mSurfaceNotes2D[path][i]->showNote();
					isOpen = true;
					break;
				}
			}
			break;
		default: qDebug() << "Incorrect Note File!";
	}
	if (isOpen)
	{
		if (mw()->VTKA())
			mw()->VTKA()->annotate(true);
		emit updateMenu();
	}
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
	}
	mPointNotes.remove(notePath);
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[notePath][i]->getCellIds());
		mSurfaceNotes[notePath][i]->removeSurfaceNote();
	}
	mSurfaceNotes.remove(notePath);
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[notePath][i]->getPoints(), mFrustumNotes[notePath][i]->getNormals());
		mFrustumNotes[notePath][i]->removeFrustumNote();
	}
	mFrustumNotes.remove(notePath);
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		mw()->VTKA()->removePointNote2DMark(mPointNotes2D[notePath][i]->getPoint());
		mPointNotes2D[notePath][i]->removePointNote2D();
	}
	mPointNotes2D.remove(notePath);
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[notePath][i]->getPoint());
		mSurfaceNotes2D[notePath][i]->removeSurfaceNote2D();
	}
	mSurfaceNotes2D.remove(notePath);
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
			mPointNotes[notePath].remove(i);
		}
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[notePath][i]->getCellIds());
			mSurfaceNotes[notePath][i]->removeSurfaceNote();
			mSurfaceNotes[notePath].remove(i);
		}
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		if (!mFrustumNotes[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[notePath][i]->getPoints(), mFrustumNotes[notePath][i]->getNormals());
			mFrustumNotes[notePath][i]->removeFrustumNote();
			mFrustumNotes[notePath].remove(i);
		}
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		if (!mPointNotes2D[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removePointNote2DMark(mPointNotes2D[notePath][i]->getPoint());
			mPointNotes2D[notePath][i]->removePointNote2D();
			mPointNotes2D[notePath].remove(i);
		}
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes2D[notePath][i]->checkSaved())
		{
			mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[notePath][i]->getPoint());
			mSurfaceNotes2D[notePath][i]->removeSurfaceNote2D();
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
	QString fn;
	if(mw()->VTKA()) {
        notePath = mw()->VTKA()->mProjectPath;
		//
		fn = mw()->VTKA()->mFilename;
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

