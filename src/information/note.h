/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Weiqi Shi (weiqi.shi@yale.edu)

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

#ifndef NOTE_H
#define NOTE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QTextBrowser>
#include <QFile>
#include <QScrollArea>

#include <vtkSelectionNode.h>
#include <vtkAbstractArray.h>
#include <vtkIdTypeArray.h>
#include <vtkDoubleArray.h>
#include <vtkDataArray.h>
#include <vtkPoints.h>

#include "../mainWindow.h"
#include "imageNote.h"

class Note: public QWidget
{
	Q_OBJECT

public:

	Note(const int noteId, const ColorType type = YELLOW);
	
	ColorType getColorType() {return mColor;}

	void setColorType(const ColorType type) {mColor = type;}

	void hideNote() {mDialog->hide();}

	void showNote() {mDialog->show();}

	QString getFileName()	{return *mFileName;}

	bool checkSaved() {return isSaved;}

	void setSaved(bool status) {isSaved = status;}

	bool checkRemoved()	{return isRemoved;}

	void setRemoved(bool status)	{isRemoved = status;}

	QString getContent();

	QVector<QString> getUser()	{return mUsers;}

protected:

	void setLabel(QString text);

	void setInfo(QString text);

	void setText(QString content){mTextEdit->setPlainText(content);}

public slots:

	void save();	//Public Slot in order to be connected from the signal emitted by informationWidget

	void close() {this->hideNote();}

	void removeImage(int id);

protected slots:

	void addImage(const QString fileName = QString());

private slots:

    void remove();
	
    void textChanged();

private:
	void clearLayout(QLayout* layout, bool deleteWidgets = true);

	static MainWindow* mw(); // find the mainframe

	void updateInfo();

	void updateLabel();

signals:

	void removeNote(int mNoteId, QString* mPath);

protected:
	QString *mPath;
	QFile* mFile;
	QString* mFileName;
	double* mPosition;
	int mNoteId;
	ColorType mColor;
	QVector<ImageNote*> mImageNotes;
	QVector<QString> mUsers;

private:
	QDialog* mDialog;
	QVBoxLayout* mVbox;
	QTextEdit* mTextEdit;
	QHBoxLayout* mButtons;
    QPushButton* mSaveButton;
	QPushButton* mAddImageButton;
    QPushButton* mRemoveButton;
	QLabel* mLabel;
	QString* mInfo;
	bool isSaved;
	bool isRemoved;	// prepare for undo for the future
};

class PointNote: public Note
{
	Q_OBJECT

public:
	PointNote(QString path, double* pos, const int cellId, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	PointNote(QString path, QString fileName, const int noteId, bool& isSucceed);

	double* getPosition() { return mPosition; }

	int getCellId() { return mCellId; }

	int getNoteId() { return mNoteId; }

	// this function is called when trying to remove all the notes
	void removePointNote();

private:
	double* mPosition;
	int mCellId;
};

class SurfaceNote: public Note
{
	Q_OBJECT

public:
	SurfaceNote(QString path, vtkSmartPointer<vtkSelectionNode> cellIds, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	SurfaceNote(QString path, QString fileName, const int noteId, bool& isSucceed);

	vtkSmartPointer<vtkSelectionNode> getCellIds() { return mCellIds; }

	int getNoteId() { return mNoteId; }

	// this function is called when trying to remove all the notes
	void removeSurfaceNote();

private:
	vtkSmartPointer<vtkSelectionNode> mCellIds;
};

class FrustumNote: public Note
{
	Q_OBJECT

public:
	FrustumNote(QString path, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, const int noteId,
		const ColorType type = YELLOW, const QString user = QString());

	FrustumNote(QString path, QString fileName, const int noteId, bool& isSucceed);

	vtkSmartPointer<vtkPoints> getPoints() { return mPoints; }

	vtkSmartPointer<vtkDataArray> getNormals() { return mNormals; }

	int getNoteId() { return mNoteId; }

	// this function is called when trying to remove all the notes
	void removeFrustumNote();

private:
	vtkSmartPointer<vtkPoints> mPoints;
	vtkSmartPointer<vtkDataArray> mNormals;
};

class PointNote2D: public Note
{
	Q_OBJECT

public:
	PointNote2D(QString path, const double* point, const int* pointImage, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	PointNote2D(QString path, QString fileName, const int noteId, bool& isSucceed);

	double* getPoint() { return mPoint; }

	int getNoteId() { return mNoteId; }

	// this function is called when trying to remove all the notes
	void removePointNote2D();

private:
	double* mPoint;	// The size is three, x, y ,0
	int* mImagePoint;	// The size is three, x, y ,0, image coordinate
};

class SurfaceNote2D: public Note
{
	Q_OBJECT

public:
	SurfaceNote2D(QString path, const double* point, const int* pointImage, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	SurfaceNote2D(QString path, QString fileName, const int noteId, bool& isSucceed);

	double* getPoint() { return mPoint; }

	int getNoteId() { return mNoteId; }

	// this function is called when trying to remove all the notes
	void removeSurfaceNote2D();

private:
	double* mPoint;	// The size is four, start x, y, end x, y
	int* mImagePoint;	// The size is four, start x, y, end x, y, image coordinate
};
#endif // NOTE_H