/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)
 			 Weiqi Shi (weiqi.shi@yale.edu)
			 Zeyu Wang (zeyu.wang@yale.edu)
			 Ying Yang (ying.yang.yy368@yale.edu)

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
  //int width = this->frameGeometry().width();
  //int height = this->frameGeometry().height();

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
	geoinfo.clear();
	mPointNotes.clear();
	mSurfaceNotes.clear();
	mFrustumNotes.clear();
	mPointNotes2D.clear();
	mSurfaceNotes2D.clear();
	mPolygonNotes2D.clear();
	
	hasNotesRemoved.clear();
	removedAnnotation.clear();
}

void Information::init(const QString path, bool isDisplayNoteMark)
{
	QString objectPath = QDir::toNativeSeparators(path);
	notePath = objectPath;
	notePath.append(QDir::separator() + QString("Note"));
	loadPointNote(objectPath, true, isDisplayNoteMark);
	loadSurfaceNote(objectPath, true, isDisplayNoteMark);
	loadFrustumNote(objectPath, true, isDisplayNoteMark);
	loadPointNote2D(objectPath, true, isDisplayNoteMark);
	loadSurfaceNote2D(objectPath, true, isDisplayNoteMark);
	loadPolygonNote2D(objectPath, true, isDisplayNoteMark);
	
	loadAnnotation(notePath);
}

void Information::initCT2DRendering(const QString path, bool isDisplayNoteMark)
{
	QString objectPath = QDir::toNativeSeparators(path);
	notePath = objectPath;
	notePath.append(QDir::separator() + QString("Note"));
	loadPointNote2D(objectPath, true, isDisplayNoteMark);
	loadSurfaceNote2D(objectPath, true, isDisplayNoteMark);
	loadPolygonNote2D(objectPath, true, isDisplayNoteMark);
	
	loadPointNote(objectPath, false);
	loadSurfaceNote(objectPath, false);
	loadFrustumNote(objectPath, false);
	loadAnnotation(notePath);
}

void Information::initCTVolumeRendering(const QString path, bool isDisplayNoteMark)
{
	QString objectPath = QDir::toNativeSeparators(path);
	notePath = objectPath;
	notePath.append(QDir::separator() + QString("Note"));
	loadPointNote2D(objectPath, false);
	loadSurfaceNote2D(objectPath, false);
	loadPolygonNote2D(objectPath, false);
	
	loadPointNote(objectPath, true, isDisplayNoteMark);
	loadSurfaceNote(objectPath, true, isDisplayNoteMark);
	loadFrustumNote(objectPath, true, isDisplayNoteMark);
	loadAnnotation(notePath);
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
	//qDebug() << "startAnnotation() calls updateCurrentPath()\n";
	updateCurrentPath();
	saveButton->setEnabled(true);
	removeButton->setEnabled(true);
	dText->setReadOnly(false);
}

void Information::finishAnnotation()
{
	//qDebug() << "finishAnnotation() calls updateCurrentPath()\n";
	updateCurrentPath();
	dText->setReadOnly(true);
	saveButton->setEnabled(false);
	removeButton->setEnabled(false);
	hideNotes();
}

bool Information::loadAnnotation(const QString notePath)
{
	QString fileName = notePath;
	fileName.append(QDir::separator() + QString("Annotation.txt"));
	QFile file(fileName);
	//qDebug()<<"Load Note"<<notePath;
	hasNotesRemoved[notePath] = false;
	content[notePath].second = true;
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
	
	QTextStream in(&file);
	QString text = in.readAll();
	content[notePath] = std::make_pair(text, true);
	file.close();
	return true;
}

void Information::reloadAnnotation()
{
	//qDebug() << "reloadAnnotation() calls updateCurrentPath()\n";
	updateCurrentPath();
	//bool saved = content[notePath].second;
	skipTextChange = true;
	dText->setPlainText(content[notePath].first);
	skipTextChange = false;
	//content[notePath].second = saved;
}

void Information::clearAnnotation()
{
	//qDebug() << "clearAnnotation() calls updateCurrentPath()\n";
	updateCurrentPath();
	//bool saved = content[notePath].second;
	skipTextChange = true;
	dText->clear();
	skipTextChange = false;
	//content[notePath].second = saved;
}

void Information::saveAnnotation()
{
	//qDebug() << "saveAnnotation() calls updateCurrentPath()\n";
	updateCurrentPath();
	if (dText->toPlainText() == QString())
		return;
	QString fileName = notePath;
	fileName.append(QDir::separator() + QString("Annotation.txt"));
	QFile file(fileName);
	// when the annotation file is not exist and it has not been removed, then create navigation item for annotation
	if (!file.exists())
		emit addNavigationItem(notePath, ANNOTATION);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Open Annotation File Failed"; 
        return;
	}
    QTextStream out(&file);
    out << dText->toPlainText();
	content[notePath].second = true;
	file.close();
}

void Information::removeAnnotation()
{
	//qDebug() << "removeAnnotation() calls updateCurrentPath()\n";
	updateCurrentPath();
	QString fileName = notePath;
	qDebug() << "removeAnnotation " << notePath;
	fileName.append(QDir::separator() + QString("Annotation.txt"));
	//content.remove(notePath);
	removedAnnotation[fileName] = content[notePath].first;
	content[notePath].second = false;
	hasNotesRemoved[notePath] = true;
	dText->clear();
	QFile file(fileName);
	file.remove();
	emit removeNavigationItem(notePath, ANNOTATION, 0);
}

void Information::createPointNote(double *pos, int cellId, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	//qDebug() << "createPointNote() calls updateCurrentPath()\n";
	updateCurrentPath();
	int size = mPointNotes[notePath].size();
	qDebug() << "Create Point Note, current size = " << size;
	PointNote* newNote = new PointNote(notePath, pos, cellId, size, color, mw()->mUserName);
	newNote->showNote();
	mPointNotes[notePath].push_back(newNote);
	connect(mPointNotes[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPointNotes[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPointNotes[notePath][size], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPointNotes[notePath][size], SLOT(replaceUserName(const QString, const QString)));
	emit addNavigationItem(notePath, POINTNOTE, NOTE3D);
}

void Information::createSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> points, ColorType color, bool isCTVolume)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	//qDebug() << "createSurfaceNote() calls updateCurrentPath()\n";
	updateCurrentPath();
	int size = mSurfaceNotes[notePath].size();
	qDebug() << "Create Surface Note, current size = " << size;
	SurfaceNote* newNote = new SurfaceNote(notePath, cellIds, points, size, color, isCTVolume, mw()->mUserName);
	newNote->showNote();
	mSurfaceNotes[notePath].push_back(newNote);
	connect(mSurfaceNotes[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mSurfaceNotes[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mSurfaceNotes[notePath][size], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mSurfaceNotes[notePath][size], SLOT(replaceUserName(const QString, const QString)));
	emit addNavigationItem(notePath, SURFACENOTE, NOTE3D);
}

void Information::createFrustumNote(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	//qDebug() << "createFrustumNote() calls updateCurrentPath()\n";
	updateCurrentPath();
	int size = mFrustumNotes[notePath].size();
	qDebug() << "Create Frustum Note, current size = " << size;
	FrustumNote* newNote = new FrustumNote(notePath, points, normals, size, color, mw()->mUserName);
	newNote->showNote();
	mFrustumNotes[notePath].push_back(newNote);
	connect(mFrustumNotes[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removeFrustumNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mFrustumNotes[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mFrustumNotes[notePath][size], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mFrustumNotes[notePath][size], SLOT(replaceUserName(const QString, const QString)));
	emit addNavigationItem(notePath, FRUSTUMNOTE, NOTE3D);
}

void Information::createPointNote2D(double* point,  int* pointImage, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	//qDebug() << "createPointNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	int size = mPointNotes2D[notePath].size();
	qDebug() << "Create Point Note 2D, current size = " << size;
	PointNote2D* newNote = new PointNote2D(notePath, point, pointImage, size, color, mw()->mUserName);
	newNote->showNote();
	mPointNotes2D[notePath].push_back(newNote);
	connect(mPointNotes2D[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPointNotes2D[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPointNotes2D[notePath][size], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPointNotes2D[notePath][size], SLOT(replaceUserName(const QString, const QString)));
	emit addNavigationItem(notePath, POINTNOTE, NOTE2D);
}

void Information::createSurfaceNote2D(double* point, int* pointImage, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	//qDebug() << "createSurfaceNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	int size = mSurfaceNotes2D[notePath].size();
	qDebug() << "Create Surface Note 2D, current size = " << size;
	SurfaceNote2D* newNote = new SurfaceNote2D(notePath, point, pointImage, size, color, mw()->mUserName);
	newNote->showNote();
	mSurfaceNotes2D[notePath].push_back(newNote);
	connect(mSurfaceNotes2D[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mSurfaceNotes2D[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mSurfaceNotes2D[notePath][size], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mSurfaceNotes2D[notePath][size], SLOT(replaceUserName(const QString, const QString)));
	emit addNavigationItem(notePath, SURFACENOTE, NOTE2D);
}

void Information::createPolygonNote2D(std::vector<std::pair<double, double> >* polygon, std::vector<std::pair<int, int> >* polygonImage, ColorType color)
{
	//qDebug() <<mw()->VTKA()->mFilename;
	//qDebug() << "createPolygonNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	int size = mPolygonNotes2D[notePath].size();
	qDebug() << "Create Polygon Note 2D, current size = " << size;
	PolygonNote2D* newNote = new PolygonNote2D(notePath, polygon, polygonImage, size, color, mw()->mUserName);
	newNote->showNote();
	mPolygonNotes2D[notePath].push_back(newNote);
	connect(mPolygonNotes2D[notePath][size], SIGNAL(removeNote(int, QString*)), this, SLOT(removePolygonNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPolygonNotes2D[notePath][size], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPolygonNotes2D[notePath][size], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPolygonNotes2D[notePath][size], SLOT(replaceUserName(const QString, const QString)));
	emit addNavigationItem(notePath, POLYGONNOTE, NOTE2D);
	
}

bool Information::loadPointNote(const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
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
		bool duplicated = false;
		for (int j = 0; j < mPointNotes[notePath].size(); j++)
		{
			QString file = mPointNotes[notePath][j]->getFileName();
			file.truncate(file.lastIndexOf(QDir::separator()));
			if (file == fileList[i])
			{
				duplicated = true;
				break;
			}
		}
		if (duplicated)
			continue;
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
		connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPointNotes[notePath][i], SLOT(replaceUserName(const QString, const QString)));
		mPointNotes[notePath][i]->setSaved(true);
		if (mw()->VTKA(path) && isLoadNoteMark)
			mw()->VTKA(path)->loadPointNoteMark(newNote->getCellId(), newNote->getColorType(), newNote->getPosition(), isDisplayNoteMark);
		//else
		//	qDebug()<<"Cannot find the window!!!!";

		
	}
	qDebug() << "Load "<<mPointNotes[notePath].size() <<" Point Note";
	return isLoadSucceed;
}

bool Information::loadPointNoteFromFile(const QString noteFileName, const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
{
	bool isLoadSucceed = true;
	bool isSucceed;
	bool duplicated = false;

	int i = mPointNotes[notePath].size(); 
	PointNote* newNote = new PointNote(notePath, noteFileName, i, isSucceed);
	
	mPointNotes[notePath].push_back(newNote);
	connect(mPointNotes[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPointNotes[notePath][i], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPointNotes[notePath][i], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPointNotes[notePath][i], SLOT(replaceUserName(const QString, const QString)));
	mPointNotes[notePath][i]->setSaved(true);
	if (mw()->VTKA(path) && isLoadNoteMark)
		mw()->VTKA(path)->loadPointNoteMark(newNote->getCellId(), newNote->getColorType(), newNote->getPosition(), isDisplayNoteMark);
	
	qDebug() << "Load "<<mPointNotes[notePath].size() <<" Point Note";
	return isLoadSucceed;
}

bool Information::loadSurfaceNote(const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
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
		bool duplicated = false;
		for (int j = 0; j < mSurfaceNotes[notePath].size(); j++)
		{
			QString file = mSurfaceNotes[notePath][j]->getFileName();
			file.truncate(file.lastIndexOf(QDir::separator()));
			if (file == fileList[i])
			{
				duplicated = true;
				break;
			}
		}
		if (duplicated)
			continue;
		SurfaceNote* newNote = new SurfaceNote(notePath, fileList[i], i, isSucceed);
		if (!isSucceed)
		{
			isLoadSucceed = false;
			continue;
		}
		mSurfaceNotes[notePath].push_back(newNote);
		connect(mSurfaceNotes[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote(int, QString*)));
		connect(this, SIGNAL(saveAll()), mSurfaceNotes[notePath][i], SLOT(save()));
		connect(this, SIGNAL(closeAll()), mSurfaceNotes[notePath][i], SLOT(close()));
		connect(this, SIGNAL(replaceUserName(const QString, const QString)), mSurfaceNotes[notePath][i], SLOT(replaceUserName(const QString, const QString)));
		mSurfaceNotes[notePath][i]->setSaved(true);
		if (mw()->VTKA(path) && isLoadNoteMark)
			mw()->VTKA(path)->loadSurfaceNoteMark(newNote->getCellIds(), newNote->getCornerPoints(), newNote->getColorType(), isDisplayNoteMark);
		//else
		//	qDebug()<<"Cannot find the window!!!!";
	}
	qDebug() << "Load "<<mSurfaceNotes[notePath].size()<<" Surface Note";
	return isLoadSucceed;
}

bool Information::loadSurfaceNoteFromFile(const QString noteFileName, const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
{
	bool isLoadSucceed = true;
	bool isSucceed;
	bool duplicated = false;

	int i = mSurfaceNotes[notePath].size();
		
	SurfaceNote* newNote = new SurfaceNote(notePath, noteFileName, i, isSucceed);
	
	mSurfaceNotes[notePath].push_back(newNote);
	connect(mSurfaceNotes[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mSurfaceNotes[notePath][i], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mSurfaceNotes[notePath][i], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mSurfaceNotes[notePath][i], SLOT(replaceUserName(const QString, const QString)));
	mSurfaceNotes[notePath][i]->setSaved(true);
	if (mw()->VTKA(path) && isLoadNoteMark)
		mw()->VTKA(path)->loadSurfaceNoteMark(newNote->getCellIds(), newNote->getCornerPoints(), newNote->getColorType(), isDisplayNoteMark);
	
	qDebug() << "Load "<<mSurfaceNotes[notePath].size()<<" Surface Note";
	return isLoadSucceed;
}

bool Information::loadFrustumNote(const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
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
		bool duplicated = false;
		for (int j = 0; j < mFrustumNotes[notePath].size(); j++)
		{
			QString file = mFrustumNotes[notePath][j]->getFileName();
			file.truncate(file.lastIndexOf(QDir::separator()));
			if (file == fileList[i])
			{
				duplicated = true;
				break;
			}
		}
		if (duplicated)
			continue;
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
		connect(this, SIGNAL(replaceUserName(const QString, const QString)), mFrustumNotes[notePath][i], SLOT(replaceUserName(const QString, const QString)));
		if (mw()->VTKA(path) && isLoadNoteMark)
			mw()->VTKA(path)->loadFrustumNoteMark(newNote->getPoints(), newNote->getNormals(), newNote->getColorType(), isDisplayNoteMark);
		//else
		//	qDebug()<<"Cannot find the window!!!!";

	}
	qDebug() << "Load "<<mFrustumNotes[notePath].size()<<" Frustum Note";
	return isLoadSucceed;
}

bool Information::loadFrustumNoteFromFile(const QString noteFileName, const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
{
	bool isLoadSucceed = true;
	bool isSucceed;
	bool duplicated = false;

	int i = mFrustumNotes[notePath].size();
	FrustumNote* newNote = new FrustumNote(notePath, noteFileName, i, isSucceed);
	
	mFrustumNotes[notePath].push_back(newNote);
	connect(mFrustumNotes[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removeFrustumNote(int, QString*)));
	connect(this, SIGNAL(saveAll()), mFrustumNotes[notePath][i], SLOT(save()));
	mFrustumNotes[notePath][i]->setSaved(true);
	connect(this, SIGNAL(closeAll()), mFrustumNotes[notePath][i], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mFrustumNotes[notePath][i], SLOT(replaceUserName(const QString, const QString)));
	if (mw()->VTKA(path) && isLoadNoteMark)
		mw()->VTKA(path)->loadFrustumNoteMark(newNote->getPoints(), newNote->getNormals(), newNote->getColorType(), isDisplayNoteMark);
	qDebug() << "Load "<<mFrustumNotes[notePath].size()<<" Frustum Note";
	return isLoadSucceed;
}

bool Information::loadPointNote2D(const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
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
		bool duplicated = false;
		for (int j = 0; j < mPointNotes2D[notePath].size(); j++)
		{
			QString file = mPointNotes2D[notePath][j]->getFileName();
			file.truncate(file.lastIndexOf(QDir::separator()));
			if (file == fileList[i])
			{
				duplicated = true;
				break;
			}
		}
		if (duplicated)
			continue;
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
		connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPointNotes2D[notePath][i], SLOT(replaceUserName(const QString, const QString)));
		mPointNotes2D[notePath][i]->setSaved(true);

		if (mw()->VTKA(path) && isLoadNoteMark)
			mw()->VTKA(path)->loadPointNote2DMark(newNote->getPoint(), newNote->getColorType(), isDisplayNoteMark);
		//else
		//	qDebug()<<"Cannot find the window!!!!";
	}
	qDebug() << "Load "<<mPointNotes2D[notePath].size()<<" Point Note";
	return isLoadSucceed;
}

bool Information::loadPointNote2DFromFile(const QString noteFileName, const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
{
	bool isLoadSucceed = true;
	bool isSucceed;
	bool duplicated = false;

	int i = mPointNotes2D[notePath].size();
	PointNote2D* newNote = new PointNote2D(notePath, noteFileName, i, isSucceed);
	
	mPointNotes2D[notePath].push_back(newNote);
	connect(mPointNotes2D[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removePointNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPointNotes2D[notePath][i], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPointNotes2D[notePath][i], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPointNotes2D[notePath][i], SLOT(replaceUserName(const QString, const QString)));
	mPointNotes2D[notePath][i]->setSaved(true);

	if (mw()->VTKA(path) && isLoadNoteMark)
		mw()->VTKA(path)->loadPointNote2DMark(newNote->getPoint(), newNote->getColorType(), isDisplayNoteMark);
	
	qDebug() << "Load "<<mPointNotes2D[notePath].size()<<" Point Note";
	return isLoadSucceed;
}

bool Information::loadSurfaceNote2D(const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
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
		bool duplicated = false;
		for (int j = 0; j < mSurfaceNotes2D[notePath].size(); j++)
		{
			QString file = mSurfaceNotes2D[notePath][j]->getFileName();
			file.truncate(file.lastIndexOf(QDir::separator()));
			if (file == fileList[i])
			{
				duplicated = true;
				break;
			}
		}
		if (duplicated)
			continue;
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
		connect(this, SIGNAL(replaceUserName(const QString, const QString)), mSurfaceNotes2D[notePath][i], SLOT(replaceUserName(const QString, const QString)));
		mSurfaceNotes2D[notePath][i]->setSaved(true);
		if (mw()->VTKA(path) && isLoadNoteMark)
			mw()->VTKA(path)->loadSurfaceNote2DMark(newNote->getPoint(), newNote->getColorType(), isDisplayNoteMark);
		//else
		//	qDebug()<<"Cannot find the window!!!!";

	}
	qDebug() << "Load "<<mSurfaceNotes2D[notePath].size()<<" Surface Note";
	return isLoadSucceed;
}

bool Information::loadSurfaceNote2DFromFile(const QString noteFileName, const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
{
	bool isLoadSucceed = true;
	bool isSucceed;
	bool duplicated = false;

	int i = mSurfaceNotes2D[notePath].size();
	SurfaceNote2D* newNote = new SurfaceNote2D(notePath, noteFileName, i, isSucceed);

	mSurfaceNotes2D[notePath].push_back(newNote);
	connect(mSurfaceNotes2D[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removeSurfaceNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mSurfaceNotes2D[notePath][i], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mSurfaceNotes2D[notePath][i], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mSurfaceNotes2D[notePath][i], SLOT(replaceUserName(const QString, const QString)));
	mSurfaceNotes2D[notePath][i]->setSaved(true);
	
	if (mw()->VTKA(path) && isLoadNoteMark)
		mw()->VTKA(path)->loadSurfaceNote2DMark(newNote->getPoint(), newNote->getColorType(), isDisplayNoteMark);

	qDebug() << "Path = " << path << endl;
	qDebug() << "Load "<<mSurfaceNotes2D[notePath].size()<<" Surface Note";
	return isLoadSucceed;
}

bool Information::loadPolygonNote2D(const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
{
	QDir dir(notePath);
	dir.setNameFilters(QStringList()<<"PolygonNote2D_*.txt");
	dir.setSorting(QDir::Name|QDir::LocaleAware);
	QStringList fileList = dir.entryList();
	bool isLoadSucceed = true;
	bool isSucceed;
	for (int i = 0; i < fileList.size(); ++i)
	{
		qDebug() << fileList[i];
		bool duplicated = false;
		for (int j = 0; j < mPolygonNotes2D[notePath].size(); j++)
		{
			QString file = mPolygonNotes2D[notePath][j]->getFileName();
			file.truncate(file.lastIndexOf(QDir::separator()));
			if (file == fileList[i])
			{
				duplicated = true;
				break;
			}
		}
		if (duplicated)
			continue;
		PolygonNote2D* newNote = new PolygonNote2D(notePath, fileList[i], i, isSucceed);
		if (!isSucceed)
		{
			isLoadSucceed = false;
			continue;
		}
		mPolygonNotes2D[notePath].push_back(newNote);
		connect(mPolygonNotes2D[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removePolygonNote2D(int, QString*)));
		connect(this, SIGNAL(saveAll()), mPolygonNotes2D[notePath][i], SLOT(save()));
		connect(this, SIGNAL(closeAll()), mPolygonNotes2D[notePath][i], SLOT(close()));
		connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPolygonNotes2D[notePath][i], SLOT(replaceUserName(const QString, const QString)));
		mPolygonNotes2D[notePath][i]->setSaved(true);
		if (mw()->VTKA(path) && isLoadNoteMark)
			mw()->VTKA(path)->loadPolygonNote2DMark(newNote->getPolygon(), newNote->getColorType(), isDisplayNoteMark);
		//else
		//	qDebug()<<"Cannot find the window!!!!";

	}
	qDebug() << "Load "<<mPolygonNotes2D[notePath].size()<<" Polygon Note";
	return isLoadSucceed;

	//// TO BE TESETED
}

bool Information::loadPolygonNote2DFromFile(const QString noteFileName, const QString path, bool isLoadNoteMark, bool isDisplayNoteMark)
{
	bool isLoadSucceed = true;
	bool isSucceed;
	bool duplicated = false;

	int i = mPolygonNotes2D[notePath].size();
	PolygonNote2D* newNote = new PolygonNote2D(notePath, noteFileName, i, isSucceed);

	mPolygonNotes2D[notePath].push_back(newNote);
	connect(mPolygonNotes2D[notePath][i], SIGNAL(removeNote(int, QString*)), this, SLOT(removePolygonNote2D(int, QString*)));
	connect(this, SIGNAL(saveAll()), mPolygonNotes2D[notePath][i], SLOT(save()));
	connect(this, SIGNAL(closeAll()), mPolygonNotes2D[notePath][i], SLOT(close()));
	connect(this, SIGNAL(replaceUserName(const QString, const QString)), mPolygonNotes2D[notePath][i], SLOT(replaceUserName(const QString, const QString)));
	mPolygonNotes2D[notePath][i]->setSaved(true);
	
	if (mw()->VTKA(path) && isLoadNoteMark)
		mw()->VTKA(path)->loadPolygonNote2DMark(newNote->getPolygon(), newNote->getColorType(), isDisplayNoteMark);

	qDebug() << "Path = " << path << endl;
	qDebug() << "Load "<<mPolygonNotes2D[notePath].size()<<" Polygon Note";
	return isLoadSucceed;
}

void Information::openPointNote(int cellId)
{
	//qDebug() << "openPointNote() calls updateCurrentPath()\n";
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

void Information::openSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, std::vector<double*> cornerPoints, bool isCTVolume)
{
	//qDebug() << "openSurfaceNote() calls updateCurrentPath()\n";
	updateCurrentPath();
	bool isOpen = false;
	if (!isCTVolume)
	{
		if (!cellIds->GetSelectionList())
		{
			qDebug() << "Cannot Find the Surface Note to Open!" << endl;
			return;
		}
		
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
	}
	else
	{
		for (int i = 0; i < mSurfaceNotes[notePath].size(); i++)
		{
			bool isSame = true;
			QVector<double*> points = mSurfaceNotes[notePath][i]->getCornerPoints();
			for (int j = 0; j < 4; j++)
			{
				if (cornerPoints[j][0] != points[j][0] || cornerPoints[j][1] != points[j][1]
					|| cornerPoints[j][2] != points[j][2])
				{
					isSame = false;
					break;
				}
			}
			if (!isSame)
				continue;

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
	//qDebug() << "openFrustumNote() calls updateCurrentPath()\n";
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
	//qDebug() << "openPointNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		double* select = mPointNotes2D[notePath][i]->getPoint();
		bool isSame = true;
		for (int j = 0; j  < 3; j++)
		{
			if (select[j] != point[j]) //// HAZARD TEST DOUBLE
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
	//qDebug() << "openSurfaceNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		double* select = mSurfaceNotes2D[notePath][i]->getPoint();
		bool isSame = true;
		for (int j = 0; j  < 4; j++)
		{
			if (select[j] != point[j]) //// HAZARD TEST DOUBLE
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

void Information::openPolygonNote2D(std::vector<std::pair<double, double> >* polygon)
{
	//qDebug() << "openPolygonNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i) 
	{
		std::vector<std::pair<double, double> >* select = mPolygonNotes2D[notePath][i]->getPolygon();
		bool isSame = true;
		std::vector<std::pair<double, double> >::iterator it1, it2;
		for (it1 = select->begin(), it2 = polygon->begin(); it1 != select->end() && it2 != polygon->end(); ++it1, ++it2)
		{
			if (abs(it1->first - it2->first) > 0.000001 || abs(it1->second - it2->second) > 0.000001)
			{
				isSame = false;
				break;
			}
		}
		if (isSame)
		{
			mPolygonNotes2D[notePath][i]->hideNote();
			mPolygonNotes2D[notePath][i]->showNote();
			break;
		}
	}
	
}

void Information::setNotePath(QString path)
{
	notePathPre = notePath;
	notePath = path;
}

void Information::recoverNotePath()
{
	notePath = notePathPre;
}

void Information::setGeoInfo(const QString objectName, QPair<double, double> latlong)
{
	geoinfo[objectName] = latlong;
}

QPair<double, double> Information::getGeoInfo(const QString objectName)
{
	return geoinfo[objectName];
}

void Information::removePointNote(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	//qDebug() << "removePointNote() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[*path].size(); ++i) 
	{
		if (mPointNotes[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removePointNoteMark(mPointNotes[*path][i]->getCellId());
			//mPointNotes[*path].remove(i);
			mPointNotes[*path][i]->setRemoved(true);
			hasNotesRemoved[notePath] = true;
			emit removeNavigationItem(notePath, POINTNOTE, i, NOTE3D);
			break;
		}
	}
}

void Information::removeSurfaceNote(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	//qDebug() << "removeSurfaceNote() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mSurfaceNotes[*path].size(); ++i) 
	{
		if (mSurfaceNotes[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[*path][i]->getCellIds(), 
				mSurfaceNotes[*path][i]->getCornerPoints(), mSurfaceNotes[*path][i]->checkCTVolume());
			//mSurfaceNotes[*path].remove(i);
			mSurfaceNotes[*path][i]->setRemoved(true);
			hasNotesRemoved[notePath] = true;
			emit removeNavigationItem(notePath, SURFACENOTE, i, NOTE3D);
			break;
		}
	}
}

void Information::removeFrustumNote(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	//qDebug() << "removeFrustumNote() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mFrustumNotes[*path].size(); ++i) 
	{
		if (mFrustumNotes[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[*path][i]->getPoints(), mFrustumNotes[*path][i]->getNormals());
			//mFrustumNotes[*path].remove(i);
			mFrustumNotes[*path][i]->setRemoved(true);
			hasNotesRemoved[notePath] = true;
			emit removeNavigationItem(notePath, FRUSTUMNOTE, i, NOTE3D);
			break;
		}
	}
}

void Information::removePointNote2D(int noteId, QString* path)
{
	//qDebug() << "remove Note "<<noteId;
	//qDebug() << "removePointNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mPointNotes2D[*path].size(); ++i) 
	{
		if (mPointNotes2D[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removePointNote2DMark(mPointNotes2D[*path][i]->getPoint());
			//mPointNotes2D[*path].remove(i);
			mPointNotes2D[*path][i]->setRemoved(true);
			hasNotesRemoved[notePath] = true;
			emit removeNavigationItem(notePath, POINTNOTE, i, NOTE2D);
			break;
		}
	}
}

void Information::removeSurfaceNote2D(int noteId, QString* path)
{
	//qDebug() << "remove surface Note "<<noteId;
	//qDebug() << "removeSurfaceNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mSurfaceNotes2D[*path].size(); ++i) 
	{
		if (mSurfaceNotes2D[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[*path][i]->getPoint());
			//mSurfaceNotes2D[*path].remove(i);
			mSurfaceNotes2D[*path][i]->setRemoved(true);
			hasNotesRemoved[notePath] = true;
			emit removeNavigationItem(notePath, SURFACENOTE, i, NOTE2D);
			break;
		}
	}
}

void Information::removePolygonNote2D(int noteId, QString* path)
{
	//qDebug() << "remove polygon Note "<<noteId;
	//qDebug() << "removePolygonNote2D() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mPolygonNotes2D[*path].size(); ++i) 
	{
		if (mPolygonNotes2D[*path][i]->getNoteId() == noteId)
		{
			mw()->VTKA()->removePolygonNote2DMark(mPolygonNotes2D[*path][i]->getPolygon());
			//mPolygonNotes2D[*path].remove(i);
			mPolygonNotes2D[*path][i]->setRemoved(true);
			hasNotesRemoved[notePath] = true;
			emit removeNavigationItem(notePath, POLYGONNOTE, i, NOTE2D);
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
	options << "Annotation.txt" << "PointNote" << "SurfaceNote" << "FrustumNote" << "PointNote2D" << "SurfaceNote2D" << "PolygonNote2D";
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
		case 6:
			for (int i = 0; i < mPolygonNotes2D[path].size(); i++)
			{
				if (mPolygonNotes2D[path][i]->getFileName() == fileName)
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mPolygonNotes2D[path][i]->hideNote();
					mPolygonNotes2D[path][i]->showNote();
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
	//qDebug()<<item->text(0)<<item->text(1)<<item->text(2);
	if (item->text(2).toInt() == 0)
		return;
	QString file = item->text(0);
	QString path = item->text(1);
	QString projectPath = path;
	
	path.append(QDir::separator() + QString("Note"));
	QString type = file.split("_")[0];
	int number;
	if (type != QString("Annotation"))
		number = file.split("_")[1].toInt();
	QStringList options;
	options << "Annotation" << "PointNote" << "SurfaceNote" << "FrustumNote" << "PointNote2D" << "SurfaceNote2D" << "PolygonNote2D";
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
		case 4:
			if (mPointNotes2D[path].size() >= number)
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
		case 5:
			if (mSurfaceNotes2D[path].size() >= number)
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
		case 6:
			if (mPolygonNotes2D[path].size() >= number)
			{
				if (!mPolygonNotes2D[path][number-1]->checkRemoved())
				{
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
					mPolygonNotes2D[path][number-1]->hideNote();
					mPolygonNotes2D[path][number-1]->showNote();
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


void Information::openNotesByUsers(const QVector<QString> users)
{
	//qDebug() << "openNotesByUsers() calls updateCurrentPath()\n";
	if (!updateCurrentPath())
		return;
	QVector<QString> mUsers;
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		mUsers = mPointNotes[notePath][i]->getUser();
		for (int j = 0; j < users.size(); j++)
		{
			if (mUsers.indexOf(users[j]) != -1)
			{
				mw()->VTKA()->openPointNoteMark(mPointNotes[notePath][i]->getCellId());
				break;
			}
		}
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		mUsers = mSurfaceNotes[notePath][i]->getUser();
		for (int j = 0; j < users.size(); j++)
		{
			if (mUsers.indexOf(users[j]) != -1)
			{
				mw()->VTKA()->openSurfaceNoteMark(mSurfaceNotes[notePath][i]->getCellIds(),
					mSurfaceNotes[notePath][i]->getCornerPoints(), mSurfaceNotes[notePath][i]->checkCTVolume());
				break;
			}
		}
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		mUsers = mFrustumNotes[notePath][i]->getUser();
		for (int j = 0; j < users.size(); j++)
		{
			if (mUsers.indexOf(users[j]) != -1)
			{
				mw()->VTKA()->openFrustumNoteMark(mFrustumNotes[notePath][i]->getPoints(), mFrustumNotes[notePath][i]->getNormals());
				break;
			}
		}
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		mUsers = mPointNotes2D[notePath][i]->getUser();
		for (int j = 0; j < users.size(); j++)
		{
			if (mUsers.indexOf(users[j]) != -1)
			{
				mw()->VTKA()->openPointNote2DMark(mPointNotes2D[notePath][i]->getPoint());
				break;
			}
		}
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		mUsers = mSurfaceNotes2D[notePath][i]->getUser();
		for (int j = 0; j < users.size(); j++)
		{
			if (mUsers.indexOf(users[j]) != -1)
			{
				mw()->VTKA()->openSurfaceNote2DMark(mSurfaceNotes2D[notePath][i]->getPoint());
				break;
			}
		}
	}
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i)
	{
		mUsers = mPolygonNotes2D[notePath][i]->getUser();
		for (int j = 0; j < users.size(); j++)
		{
			if (mUsers.indexOf(users[j]) != -1)
			{
				mw()->VTKA()->openPolygonNote2DMark(mPolygonNotes2D[notePath][i]->getPolygon());
				break;
			}
		}
		
	}
}

void Information::replaceNotesUserName(const QString newName, const QString oldName)
{
	if (newName != oldName)
		emit replaceUserName(newName, oldName);
}

void Information::draw2DNoteMark(const QString path)
{
	QString objectPath = QDir::toNativeSeparators(path);
	QString noteFolder = objectPath;
	noteFolder.append(QDir::separator() + QString("Note"));

	if (mw()->VTKA(objectPath))
	{
		for (int i = 0; i < mPointNotes2D[noteFolder].size(); i++)
		{
			mw()->VTKA(objectPath)->loadPointNote2DMark(mPointNotes2D[noteFolder][i]->getPoint(), 
				mPointNotes2D[noteFolder][i]->getColorType());
		}
		for (int i = 0; i < mSurfaceNotes2D[noteFolder].size(); i++)
		{
			mw()->VTKA(objectPath)->loadSurfaceNote2DMark(mSurfaceNotes2D[noteFolder][i]->getPoint(), 
				mSurfaceNotes2D[noteFolder][i]->getColorType());
		}
		for (int i = 0; i < mPolygonNotes2D[noteFolder].size(); i++)
		{
			mw()->VTKA(objectPath)->loadPolygonNote2DMark(mPolygonNotes2D[noteFolder][i]->getPolygon(), 
				mPolygonNotes2D[noteFolder][i]->getColorType());
		}
		
	}
}

void Information::draw3DNoteMark(const QString path)
{
	QString objectPath = QDir::toNativeSeparators(path);
	QString noteFolder = objectPath;
	noteFolder.append(QDir::separator() + QString("Note"));
	if (mw()->VTKA(objectPath))
	{
		for (int i = 0; i < mPointNotes[noteFolder].size(); i++)
		{
			mw()->VTKA(objectPath)->loadPointNoteMark(mPointNotes[noteFolder][i]->getCellId(), 
				mPointNotes[noteFolder][i]->getColorType(), mPointNotes[noteFolder][i]->getPosition());
		}
		for (int i = 0; i < mSurfaceNotes[noteFolder].size(); i++)
		{
			mw()->VTKA(objectPath)->loadSurfaceNoteMark(mSurfaceNotes[noteFolder][i]->getCellIds(),
				mSurfaceNotes[noteFolder][i]->getCornerPoints(), mSurfaceNotes[noteFolder][i]->getColorType());
		}
		for (int i = 0; i < mFrustumNotes[noteFolder].size(); i++)
		{
			mw()->VTKA(objectPath)->loadFrustumNoteMark(mFrustumNotes[noteFolder][i]->getPoints(), 
				mFrustumNotes[noteFolder][i]->getNormals(), mFrustumNotes[noteFolder][i]->getColorType());
		}
	}
}

// Added by Ying to copy annotations
QVector<QString> Information::getNoteModeType(QTreeWidgetItem* item)
{
	QVector<QString> res;

	QString file = item->text(0);

	QString path = item->text(1);
	
	path.append(QDir::separator() + QString("Note"));

	QString type = file.split("_")[0];
	int number;
	if (type != QString("Annotation"))
		number = file.split("_")[1].toInt();

	QStringList options;
	options << "Annotation" << "PointNote" << "SurfaceNote" << "FrustumNote" << "PointNote2D" << "SurfaceNote2D" << "PolygonNote2D";

	switch(options.indexOf(type))
	{
		case 0:
			{ break; return res; }
		case 1:
			if (mPointNotes[path].size() >= number) {
				res.push_back(QString("PointNote"));
				res.push_back(QString("3D"));
			}
			else {
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 2:
			if (mSurfaceNotes[path].size() >= number) {
				res.push_back(QString("SurfaceNote"));
				res.push_back(QString("3D"));
			}
			else {
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 3:
			if (mFrustumNotes[path].size() >= number) {
				res.push_back(QString("FrustumNote"));
				res.push_back(QString("3D"));
			}
			else {
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 4:
			if (mPointNotes2D[path].size() >= number) {
				res.push_back(QString("PointNote"));
				res.push_back(QString("2D"));
			}
			else {
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 5: 
			if (mSurfaceNotes2D[path].size() >= number) {
				res.push_back(QString("SurfaceNote"));
				res.push_back(QString("2D"));
			}
			else {
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 6: 
			if (mPolygonNotes2D[path].size() >= number) {
				res.push_back(QString("PolygonNote"));
				res.push_back(QString("2D"));
			}
			else {
				qDebug() << "Did not find the Notes!";
			}
			break;
		default: { qDebug() << "Did not find the Notes!"; }
	}

	return res;
}

QString Information::getNoteFileName(QTreeWidgetItem* item)
{
	QString file = item->text(0);

	QString path = item->text(1);
	
	path.append(QDir::separator() + QString("Note"));

	QString type = file.split("_")[0];
	int number;
	if (type != QString("Annotation"))
		number = file.split("_")[1].toInt();

	QStringList options;
	options << "Annotation" << "PointNote" << "SurfaceNote" << "FrustumNote" << "PointNote2D" << "SurfaceNote2D" << "PolygonNote2D";

	switch(options.indexOf(type))
	{
		case 0:
			break;
		case 1:
			if (mPointNotes[path].size() >= number)
			{
				return mPointNotes[path][number-1]->getFileName();
			}
			else
			{
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 2:
			if (mSurfaceNotes[path].size() >= number)
			{
				return mSurfaceNotes[path][number-1]->getFileName();
			}
			else
			{
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 3:
			if (mFrustumNotes[path].size() >= number)
			{
				return mFrustumNotes[path][number-1]->getFileName();
			}
			else
			{
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 4:
			if (mPointNotes2D[path].size() >= number)
			{
				return mPointNotes2D[path][number-1]->getFileName();
			}
			else
			{
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 5:
			if (mSurfaceNotes2D[path].size() >= number)
			{
				return mSurfaceNotes2D[path][number-1]->getFileName();
			}
			else
			{
				qDebug() << "Did not find the Notes!";
			}
			break;
		case 6:
			if (mPolygonNotes2D[path].size() >= number)
			{
				return mPolygonNotes2D[path][number-1]->getFileName();
			}
			else
			{
				qDebug() << "Did not find the Notes!";
			}
			break;
		default: qDebug() << "Did not find the Notes!";
	}
}

void Information::undoRemoveNote(QTreeWidgetItem* item)
{
	//qDebug()<<item->text(0)<<item->text(1)<<item->text(2);
	if (item->text(2).toInt() != 0)
		return;
	QString file = item->text(0);
	file = file.split("</font>")[0].split("<font color=\"gray\">")[1];
	QString path = item->text(1);
	QString projectPath = path;
	
	path.append(QDir::separator() + QString("Note"));
	QString type = file.split("_")[0];
	int number;
	if (type != QString("Annotation"))
		number = file.split("_")[1].toInt();
	QStringList options;
	options << "Annotation" << "PointNote" << "SurfaceNote" << "FrustumNote" << "PointNote2D" << "SurfaceNote2D" << "PolygonNote2D";
	switch(options.indexOf(type))
	{
		case 0:
			mw()->switchSubWindow(projectPath);
			if (dText->toPlainText() != QString())
			{
				int ret = QMessageBox::warning(this, tr("Warning"),
                               tr("Current annotation is not saved.\"Undo\" will replace it with previously removed annotation."),
                               QMessageBox::Yes | QMessageBox::No,
                               QMessageBox::No);
				if(ret == QMessageBox::No)
					return;
			}
			path.append(QDir::separator() + QString("Annotation.txt"));
			dText->setText(removedAnnotation[path]);
			saveAnnotation();
			break;
		case 1:
			if (mPointNotes[path].size() >= number)
			{
				if (mPointNotes[path][number-1]->checkRemoved())
				{
					mPointNotes[path][number-1]->setRemoved(false);
					mPointNotes[path][number-1]->setSaved(false);  // To process save() correctly;
					mPointNotes[path][number-1]->save();
					if (mw()->VTKA(projectPath) && mw()->VTKA(projectPath)->getWidgetMode() != CTSTACK) // 2D CTSACK cannot display 3D notes
						mw()->VTKA(projectPath)->loadPointNoteMark(mPointNotes[path][number-1]->getCellId(),
							mPointNotes[path][number-1]->getColorType(), mPointNotes[path][number-1]->getPosition());
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
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
				if (mSurfaceNotes[path][number-1]->checkRemoved())
				{
					mSurfaceNotes[path][number-1]->setRemoved(false);
					mSurfaceNotes[path][number-1]->setSaved(false);  // To process save() correctly;
					mSurfaceNotes[path][number-1]->save();
					if (mw()->VTKA(projectPath) && mw()->VTKA(projectPath)->getWidgetMode() != CTSTACK) // 2D CTSACK cannot display 3D notes
						mw()->VTKA(projectPath)->loadSurfaceNoteMark(mSurfaceNotes[path][number-1]->getCellIds(),
							mSurfaceNotes[path][number-1]->getCornerPoints(), mSurfaceNotes[path][number-1]->getColorType());
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
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
				if (mFrustumNotes[path][number-1]->checkRemoved())
				{
					mFrustumNotes[path][number-1]->setRemoved(false);
					mFrustumNotes[path][number-1]->setSaved(false);  // To process save() correctly;
					mFrustumNotes[path][number-1]->save();
					if (mw()->VTKA(projectPath) && mw()->VTKA(projectPath)->getWidgetMode() != CTSTACK) // 2D CTSACK cannot display 3D notes
						mw()->VTKA(projectPath)->loadFrustumNoteMark(mFrustumNotes[path][number-1]->getPoints(),
							mFrustumNotes[path][number-1]->getNormals(), mFrustumNotes[path][number-1]->getColorType());
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
				}
			}
			else
			{
				qDebug()<<"Incorrect Notes!";
			}
			break;
		case 4:
			if (mPointNotes2D[path].size() >= number)
			{
				if (mPointNotes2D[path][number-1]->checkRemoved())
				{
					mPointNotes2D[path][number-1]->setRemoved(false);
					mPointNotes2D[path][number-1]->setSaved(false);  // To process save() correctly;
					mPointNotes2D[path][number-1]->save();
					if (mw()->VTKA(projectPath) && mw()->VTKA(projectPath)->getWidgetMode() != CTVOLUME) // 3D CTVOLUME cannot display 2D notes
						mw()->VTKA(projectPath)->loadPointNote2DMark(mPointNotes2D[path][number-1]->getPoint(),
							mPointNotes2D[path][number-1]->getColorType());
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
				}
			}
			else
			{
				qDebug()<<"Incorrect Notes!";
			}
			break;
		case 5:
			if (mSurfaceNotes2D[path].size() >= number)
			{
				if (mSurfaceNotes2D[path][number-1]->checkRemoved())
				{
					mSurfaceNotes2D[path][number-1]->setRemoved(false);
					mSurfaceNotes2D[path][number-1]->setSaved(false);  // To process save() correctly;
					mSurfaceNotes2D[path][number-1]->save();
					if (mw()->VTKA(projectPath) && mw()->VTKA(projectPath)->getWidgetMode() != CTVOLUME) // 3D CTVOLUME cannot display 2D notes
						mw()->VTKA(projectPath)->loadSurfaceNote2DMark(mSurfaceNotes2D[path][number-1]->getPoint(),
							mSurfaceNotes2D[path][number-1]->getColorType());
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
				}
			}
			else
			{
				qDebug()<<"Incorrect Notes!";
			}
			break;
		case 6:
			if (mPolygonNotes2D[path].size() >= number)
			{
				if (mPolygonNotes2D[path][number-1]->checkRemoved())
				{
					mPolygonNotes2D[path][number-1]->setRemoved(false);
					mPolygonNotes2D[path][number-1]->setSaved(false);  // To process save() correctly;
					mPolygonNotes2D[path][number-1]->save();
					if (mw()->VTKA(projectPath) && mw()->VTKA(projectPath)->getWidgetMode() != CTVOLUME) // 3D CTVOLUME cannot display 2D notes
						mw()->VTKA(projectPath)->loadPolygonNote2DMark(mPolygonNotes2D[path][number-1]->getPolygon(),
							mPolygonNotes2D[path][number-1]->getColorType());
					if (mw()->VTKA())
						mw()->VTKA()->annotate(true);
					emit updateMenu();
				}
			}
			else
			{
				qDebug()<<"Incorrect Notes!";
			}
			
			break;
		default: qDebug() << "Incorrect Note File!";
	}
	item->setText(0, file);
	item->setText(2, QString::number(1));
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
	//qDebug() << "saveObjectNotes() calls updateCurrentPath()\n";
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
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i)
	{
		mPolygonNotes2D[notePath][i]->save();
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
	//qDebug() << "closeObjectNotes() calls updateCurrentPath()\n";
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
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i)
	{
		mPolygonNotes2D[notePath][i]->hideNote();
	}
	
}

void Information::removeAllNotes()
{
	//qDebug() << "remove ALL Note ";
	//qDebug() << "removeAllNotes() calls updateCurrentPath()\n";
	updateCurrentPath();
	hasNotesRemoved[notePath] = true;
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		// If current widget mode is in CTSTACK, no point note mark are drawn.
		if (mw()->VTKA()->getWidgetMode() != CTSTACK)
			mw()->VTKA()->removePointNoteMark(mPointNotes[notePath][i]->getCellId());
		mPointNotes[notePath][i]->removePointNote();
		mPointNotes[notePath][i]->setRemoved(true);
		emit removeNavigationItem(notePath, POINTNOTE, i, NOTE3D);
	}
	//mPointNotes.remove(notePath);
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		// If current widget mode is in CTSTACK, no surface note mark are drawn.
		if (mw()->VTKA()->getWidgetMode() != CTSTACK)
			mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[notePath][i]->getCellIds(),
				mSurfaceNotes[notePath][i]->getCornerPoints(), mSurfaceNotes[notePath][i]->checkCTVolume());
		mSurfaceNotes[notePath][i]->removeSurfaceNote();
		mSurfaceNotes[notePath][i]->setRemoved(true);
		emit removeNavigationItem(notePath, SURFACENOTE, i, NOTE3D);
	}
	//mSurfaceNotes.remove(notePath);
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		// If current widget mode is in CTSTACK, no frustm note mark are drawn. Frustum note should be deleted and it should not run into here.
		if (mw()->VTKA()->getWidgetMode() != CTSTACK)
			mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[notePath][i]->getPoints(), mFrustumNotes[notePath][i]->getNormals());
		mFrustumNotes[notePath][i]->removeFrustumNote();
		mFrustumNotes[notePath][i]->setRemoved(true);
		emit removeNavigationItem(notePath, FRUSTUMNOTE, i, NOTE3D);
	}
	//mFrustumNotes.remove(notePath);
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		// If current widget mode is in CTVOLUME, no point 2D note mark are drawn.
		if (mw()->VTKA()->getWidgetMode() != CTVOLUME)
			mw()->VTKA()->removePointNote2DMark(mPointNotes2D[notePath][i]->getPoint());
		mPointNotes2D[notePath][i]->removePointNote2D();
		mPointNotes2D[notePath][i]->setRemoved(true);
		emit removeNavigationItem(notePath, POINTNOTE, i, NOTE2D);
	}
	//mPointNotes2D.remove(notePath);
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		// If current widget mode is in CTVOLUME, no surface 2D note mark are drawn.
		if (mw()->VTKA()->getWidgetMode() != CTVOLUME)
			mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[notePath][i]->getPoint());
		mSurfaceNotes2D[notePath][i]->removeSurfaceNote2D();
		mSurfaceNotes2D[notePath][i]->setRemoved(true);
		emit removeNavigationItem(notePath, SURFACENOTE, i, NOTE2D);
	}
	//mSurfaceNotes2D.remove(notePath);
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i) 
	{
		// If current widget mode is in CTVOLUME, no polygon 2D note mark are drawn.
		if (mw()->VTKA()->getWidgetMode() != CTVOLUME)
			mw()->VTKA()->removePolygonNote2DMark(mPolygonNotes2D[notePath][i]->getPolygon());
		mPolygonNotes2D[notePath][i]->removePolygonNote2D();
		mPolygonNotes2D[notePath][i]->setRemoved(true);
		emit removeNavigationItem(notePath, POLYGONNOTE, i, NOTE2D);
	}
	//mPolygonNotes2D.remove(notePath);
	
}

void Information::removeAllNotes(QString path)
{
	qDebug() << "remove ALL Note "<<path;
	QString removePath = path;
	removePath.append(QDir::separator() + QString("Note"));
	hasNotesRemoved[removePath] = true;
	for (int i = 0; i < mPointNotes[removePath].size(); ++i) 
	{
		// If current widget mode is in CTSTACK, no point note mark are drawn.
		if (mw()->VTKA(path)->getWidgetMode() != CTSTACK)
			mw()->VTKA(path)->removePointNoteMark(mPointNotes[removePath][i]->getCellId());
		mPointNotes[removePath][i]->removePointNote();
		mPointNotes[removePath][i]->setRemoved(true);
		emit removeNavigationItem(removePath, POINTNOTE, i, NOTE3D);
	}
	//mPointNotes.remove(removePath);
	for (int i = 0; i < mSurfaceNotes[removePath].size(); ++i) 
	{
		// If current widget mode is in CTSTACK, no surface note mark are drawn.
		if (mw()->VTKA(path)->getWidgetMode() != CTSTACK)
			mw()->VTKA(path)->removeSurfaceNoteMark(mSurfaceNotes[removePath][i]->getCellIds(),
				mSurfaceNotes[removePath][i]->getCornerPoints(), mSurfaceNotes[removePath][i]->checkCTVolume());
		mSurfaceNotes[removePath][i]->removeSurfaceNote();
		mSurfaceNotes[removePath][i]->setRemoved(true);
		emit removeNavigationItem(removePath, SURFACENOTE, i, NOTE3D);
	}
	//mSurfaceNotes.remove(removePath);
	for (int i = 0; i < mFrustumNotes[removePath].size(); ++i) 
	{
		// If current widget mode is in CTSTACK, no frustm note mark are drawn. Frustum note should be deleted and it should not run into here.
		if (mw()->VTKA(path)->getWidgetMode() != CTSTACK)
			mw()->VTKA(path)->removeFrustumNoteMark(mFrustumNotes[removePath][i]->getPoints(), mFrustumNotes[removePath][i]->getNormals());
		mFrustumNotes[removePath][i]->removeFrustumNote();
		mFrustumNotes[removePath][i]->setRemoved(true);
		emit removeNavigationItem(removePath, FRUSTUMNOTE, i, NOTE3D);
	}
	//mFrustumNotes.remove(removePath);
	for (int i = 0; i < mPointNotes2D[removePath].size(); ++i) 
	{
		// If current widget mode is in CTVOLUME, no point 2D note mark are drawn.
		if (mw()->VTKA(path)->getWidgetMode() != CTVOLUME)
			mw()->VTKA(path)->removePointNote2DMark(mPointNotes2D[removePath][i]->getPoint());
		mPointNotes2D[removePath][i]->removePointNote2D();
		mPointNotes2D[removePath][i]->setRemoved(true);
		emit removeNavigationItem(removePath, POINTNOTE, i, NOTE2D);
	}
	//mPointNotes2D.remove(removePath);
	for (int i = 0; i < mSurfaceNotes2D[removePath].size(); ++i) 
	{
		// If current widget mode is in CTVOLUME, no surface 2D note mark are drawn.
		if (mw()->VTKA(path)->getWidgetMode() != CTVOLUME)
			mw()->VTKA(path)->removeSurfaceNote2DMark(mSurfaceNotes2D[removePath][i]->getPoint());
		mSurfaceNotes2D[removePath][i]->removeSurfaceNote2D();
		mSurfaceNotes2D[removePath][i]->setRemoved(true);
		emit removeNavigationItem(removePath, SURFACENOTE, i, NOTE2D);
	}
	//mSurfaceNotes2D.remove(removePath);
	for (int i = 0; i < mPolygonNotes2D[removePath].size(); ++i) 
	{
		// If current widget mode is in CTVOLUME, no polygon 2D note mark are drawn.
		if (mw()->VTKA(path)->getWidgetMode() != CTVOLUME)
			mw()->VTKA(path)->removePolygonNote2DMark(mPolygonNotes2D[removePath][i]->getPolygon());
		mPolygonNotes2D[removePath][i]->removePolygonNote2D();
		mPolygonNotes2D[removePath][i]->setRemoved(true);
		emit removeNavigationItem(removePath, POLYGONNOTE, i, NOTE2D);
	}
	//mPolygonNotes2D.remove(removePath);
	
}

void Information::removeUnSavedNotes()
{
	//qDebug() << "removeUnSavedNotes() calls updateCurrentPath()\n";
	updateCurrentPath();
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		if (!mPointNotes[notePath][i]->checkSaved())
		{
			// If current widget mode is in CTSTACK, no point note mark are drawn.
			if (mw()->VTKA()->getWidgetMode() != CTSTACK)
				mw()->VTKA()->removePointNoteMark(mPointNotes[notePath][i]->getCellId());
			mPointNotes[notePath][i]->removePointNote();
			mPointNotes[notePath][i]->setRemoved(true);
			emit removeNavigationItem(notePath, POINTNOTE, i, NOTE3D);
			//mPointNotes[notePath].remove(i);
		}
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes[notePath][i]->checkSaved())
		{
			// If current widget mode is in CTSTACK, no surface note mark are drawn.
			if (mw()->VTKA()->getWidgetMode() != CTSTACK)
				mw()->VTKA()->removeSurfaceNoteMark(mSurfaceNotes[notePath][i]->getCellIds(),
					mSurfaceNotes[notePath][i]->getCornerPoints(), mSurfaceNotes[notePath][i]->checkCTVolume());
			mSurfaceNotes[notePath][i]->removeSurfaceNote();
			mSurfaceNotes[notePath][i]->setRemoved(true);
			emit removeNavigationItem(notePath, SURFACENOTE, i, NOTE3D);
			//mSurfaceNotes[notePath].remove(i);
		}
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		if (!mFrustumNotes[notePath][i]->checkSaved())
		{
			// If current widget mode is in CTSTACK, no frustm note mark are drawn. Frustum note should be deleted and it should not run into here.
			if (mw()->VTKA()->getWidgetMode() != CTSTACK)
				mw()->VTKA()->removeFrustumNoteMark(mFrustumNotes[notePath][i]->getPoints(), mFrustumNotes[notePath][i]->getNormals());
			mFrustumNotes[notePath][i]->removeFrustumNote();
			mFrustumNotes[notePath][i]->setRemoved(true);
			emit removeNavigationItem(notePath, FRUSTUMNOTE, i, NOTE3D);
			//mFrustumNotes[notePath].remove(i);		
		}
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		if (!mPointNotes2D[notePath][i]->checkSaved())
		{
			// If current widget mode is in CTVOLUME, no point 2D note mark are drawn.
			if (mw()->VTKA()->getWidgetMode() != CTVOLUME)
				mw()->VTKA()->removePointNote2DMark(mPointNotes2D[notePath][i]->getPoint());
			mPointNotes2D[notePath][i]->removePointNote2D();
			mPointNotes2D[notePath][i]->setRemoved(true);
			emit removeNavigationItem(notePath, POINTNOTE, i, NOTE2D);
			//mPointNotes2D[notePath].remove(i);
		}
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		if (!mSurfaceNotes2D[notePath][i]->checkSaved())
		{
			// If current widget mode is in CTVOLUME, no surface 2D note mark are drawn.
			if (mw()->VTKA()->getWidgetMode() != CTVOLUME)
				mw()->VTKA()->removeSurfaceNote2DMark(mSurfaceNotes2D[notePath][i]->getPoint());
			mSurfaceNotes2D[notePath][i]->removeSurfaceNote2D();
			mSurfaceNotes2D[notePath][i]->setRemoved(true);
			emit removeNavigationItem(notePath, SURFACENOTE, i, NOTE2D);
			//mSurfaceNotes2D[notePath].remove(i);
		}
	}
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i) 
	{
		if (!mPolygonNotes2D[notePath][i]->checkSaved())
		{
			// If current widget mode is in CTVOLUME, no polygon 2D note mark are drawn.
			if (mw()->VTKA()->getWidgetMode() != CTVOLUME)
				mw()->VTKA()->removePolygonNote2DMark(mPolygonNotes2D[notePath][i]->getPolygon());
			mPolygonNotes2D[notePath][i]->removePolygonNote2D();
			mPolygonNotes2D[notePath][i]->setRemoved(true);
			emit removeNavigationItem(notePath, POLYGONNOTE, i, NOTE2D);
			//mPolygonNotes2D[notePath].remove(i);
		}
	}
	
}

void Information::removeAllNotesMark()
{
	QList<QString> objectNotePath = content.keys();
	for (int i = 0; i < objectNotePath.size(); i++)
	{
		QString path = objectNotePath[i];
		path.truncate(path.lastIndexOf(QDir::separator()));
		qDebug() << "remove ALL Note "<<path;
		QString removePath = objectNotePath[i];
		hasNotesRemoved[removePath] = true;
		for (int i = 0; i < mPointNotes[removePath].size(); ++i)
		{
			// If current widget mode is in CTSTACK, no point note mark are drawn.
			if (mw()->VTKA(path)->getWidgetMode() != CTSTACK)
				mw()->VTKA(path)->removePointNoteMark(mPointNotes[removePath][i]->getCellId());
		}
		for (int i = 0; i < mSurfaceNotes[removePath].size(); ++i)
		{
			// If current widget mode is in CTSTACK, no surface note mark are drawn.
			if (mw()->VTKA(path)->getWidgetMode() != CTSTACK)
				mw()->VTKA(path)->removeSurfaceNoteMark(mSurfaceNotes[removePath][i]->getCellIds(),
					mSurfaceNotes[removePath][i]->getCornerPoints(), mSurfaceNotes[removePath][i]->checkCTVolume());
		}
		for (int i = 0; i < mFrustumNotes[removePath].size(); ++i)
		{
			// If current widget mode is in CTSTACK, no frustm note mark are drawn.
			//Frustum note should be deleted and it should not run into here.
			if (mw()->VTKA(path)->getWidgetMode() != CTSTACK)
				mw()->VTKA(path)->removeFrustumNoteMark(mFrustumNotes[removePath][i]->getPoints(), mFrustumNotes[removePath][i]->getNormals());
		}
		for (int i = 0; i < mPointNotes2D[removePath].size(); ++i)
		{
			// If current widget mode is in CTVOLUME, no point 2D note mark are drawn.
			if (mw()->VTKA(path)->getWidgetMode() != CTVOLUME)
				mw()->VTKA(path)->removePointNote2DMark(mPointNotes2D[removePath][i]->getPoint());
		}
		for (int i = 0; i < mSurfaceNotes2D[removePath].size(); ++i)
		{
			// If current widget mode is in CTVOLUME, no surface 2D note mark are drawn.
			if (mw()->VTKA(path)->getWidgetMode() != CTVOLUME)
				mw()->VTKA(path)->removeSurfaceNote2DMark(mSurfaceNotes2D[removePath][i]->getPoint());
		}
		for (int i = 0; i < mPolygonNotes2D[removePath].size(); ++i)
		{
			// If current widget mode is in CTVOLUME, no polygon 2D note mark are drawn.
			if (mw()->VTKA(path)->getWidgetMode() != CTVOLUME)
				mw()->VTKA(path)->removePolygonNote2DMark(mPolygonNotes2D[removePath][i]->getPolygon());
		}
		
	}
}

void Information::hideNotes()
{
	//qDebug() << "hideNotes() calls updateCurrentPath()\n";
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
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i) 
	{
		mPolygonNotes2D[notePath][i]->hideNote();
	}
	
}

void Information::showNotes()
{
	//qDebug() << "showNotes() calls updateCurrentPath()\n";
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
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i) 
	{
		mPolygonNotes2D[notePath][i]->showNote();
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
		for (int i = 0; i < mPolygonNotes2D[path].size(); ++i) 
		{
			if (!mPolygonNotes2D[path][i]->checkSaved())
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
	//qDebug() << "checkObjectSaved() calls updateCurrentPath()\n";
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
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i) 
	{
		if (!mPolygonNotes2D[notePath][i]->checkSaved())
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

QVector<QPair<QString, NoteType> > Information::getAllNotes(const QString objectPath)
{
	QString path = objectPath;
	path = QDir::toNativeSeparators(path);
	path.append(QDir::separator() + QString("Note"));
	QVector<QPair<QString, NoteType> > notes;
	if (content.find(path) == content.end())	// cannot find the object, return empty vector
		return notes;
	QString annotation = content[path].first;
	notes.push_back(qMakePair(annotation, NONE));
	if (mPointNotes.find(path) != mPointNotes.end())
	{
		for (int i = 0; i < mPointNotes[path].size(); i++)
		{
			if (!mPointNotes[path][i]->checkRemoved())
				notes.push_back(qMakePair(mPointNotes[path][i]->getContent(), NOTE3D));
		}
	}
	if (mSurfaceNotes.find(path) != mSurfaceNotes.end())
	{
		for (int i = 0; i < mSurfaceNotes[path].size(); i++)
		{
			if (!mSurfaceNotes[path][i]->checkRemoved())
				notes.push_back(qMakePair(mSurfaceNotes[path][i]->getContent(), NOTE3D));
		}
	}
	if (mFrustumNotes.find(path) != mFrustumNotes.end())
	{
		for (int i = 0; i < mFrustumNotes[path].size(); i++)
		{
			if (!mFrustumNotes[path][i]->checkRemoved())
				notes.push_back(qMakePair(mFrustumNotes[path][i]->getContent(), NOTE3D));
		}
	}
	if (mPointNotes2D.find(path) != mPointNotes2D.end())
	{
		for (int i = 0; i < mPointNotes2D[path].size(); i++)
		{
			if (!mPointNotes2D[path][i]->checkRemoved())
				notes.push_back(qMakePair(mPointNotes2D[path][i]->getContent(), NOTE2D));
		}
	}
	if (mSurfaceNotes2D.find(path) != mSurfaceNotes2D.end())
	{
		for (int i = 0; i < mSurfaceNotes2D[path].size(); i++)
		{
			if (!mSurfaceNotes2D[path][i]->checkRemoved())
				notes.push_back(qMakePair(mSurfaceNotes2D[path][i]->getContent(), NOTE2D));
		}
	}
	if (mPolygonNotes2D.find(path) != mPolygonNotes2D.end())
	{
		for (int i = 0; i < mPolygonNotes2D[path].size(); i++)
		{
			if (!mPolygonNotes2D[path][i]->checkRemoved())
				notes.push_back(qMakePair(mPolygonNotes2D[path][i]->getContent(), NOTE2D));
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

	if (mPolygonNotes2D.find(path) != mPolygonNotes2D.end())
		notes.push_back(mPolygonNotes2D[path].size());
	else
		notes.push_back(0);
	
	return notes;
}

QVector<QString> Information::getAllUsers()
{
	QVector<QString> mUsers;
	//qDebug() << "getAllUsers() calls updateCurrentPath()\n";
	if (!updateCurrentPath())
		return mUsers;
	std::vector<std::string> users;	// need to use std::unique to remove redundency
	for (int i = 0; i < mPointNotes[notePath].size(); ++i) 
	{
		QVector<QString> noteUser = mPointNotes[notePath][i]->getUser();
		for (int j = 0; j < noteUser.size(); j++)
			users.push_back(noteUser[j].toStdString());
	}
	for (int i = 0; i < mSurfaceNotes[notePath].size(); ++i) 
	{
		QVector<QString> noteUser = mSurfaceNotes[notePath][i]->getUser();
		for (int j = 0; j < noteUser.size(); j++)
			users.push_back(noteUser[j].toStdString());
	}
	for (int i = 0; i < mFrustumNotes[notePath].size(); ++i) 
	{
		QVector<QString> noteUser = mFrustumNotes[notePath][i]->getUser();
		for (int j = 0; j < noteUser.size(); j++)
			users.push_back(noteUser[j].toStdString());
	}
	for (int i = 0; i < mPointNotes2D[notePath].size(); ++i) 
	{
		QVector<QString> noteUser = mPointNotes2D[notePath][i]->getUser();
		for (int j = 0; j < noteUser.size(); j++)
			users.push_back(noteUser[j].toStdString());
	}
	for (int i = 0; i < mSurfaceNotes2D[notePath].size(); ++i) 
	{
		QVector<QString> noteUser = mSurfaceNotes2D[notePath][i]->getUser();
		for (int j = 0; j < noteUser.size(); j++)
			users.push_back(noteUser[j].toStdString());
	}
	for (int i = 0; i < mPolygonNotes2D[notePath].size(); ++i) 
	{
		QVector<QString> noteUser = mPolygonNotes2D[notePath][i]->getUser();
		for (int j = 0; j < noteUser.size(); j++)
			users.push_back(noteUser[j].toStdString());
	}
	
	std::sort(users.begin(), users.end());
	std::vector<std::string>::iterator end_unique = std::unique(users.begin(), users.end());
	users.erase(end_unique, users.end());
	for (int i = 0; i < users.size(); i++)
		mUsers.push_back(QString(users[i].c_str()));
	return mUsers;
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
  return NULL;
}

