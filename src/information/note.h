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

/**
 * This class is the basic note class. Different types of note inherit from it.
 * It provides the note dialog and also maintain the note file.
 */

class Note: public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor.
	 * @param  noteId  The Id assigned to this note. Each type of note maintain their own ids.
	 * @param  type    The color type of the note.
	 */
	Note(const int noteId, const ColorType type = YELLOW);
	
	/**
	 * @brief  Get the color type.
	 * @return Color type.
	 */
	ColorType getColorType() {return mColor;}

	/**
	 * @brief  Set the color type.
	 * @param  type    The color type of the note.
	 */
	void setColorType(const ColorType type) {mColor = type;}

	/**
	 * @brief  Hide the Note dialog.
	 */
	void hideNote() {mDialog->hide();}

	/**
	 * @brief  Show the Note dialog.
	 */
	void showNote() {mDialog->show();}

	/**
	 * @brief  Each note is saved as a .txt file. Get the note file.
	 * @return The .txt note file.
	 */
	QString getFileName()	{return *mFileName;}

	/**
	 * @brief  Test whether the note is saved.
	 * @return True if saved otherwise false.
	 */
	bool checkSaved() {return isSaved;}

	/**
	 * @brief  Set the save-status of note. 
	 * @param  status  Boolen value of the save-status.
	 */
	void setSaved(bool status) {isSaved = status;}

	/**
	 * @brief  Test whether the note is removed.
	 * @return True if removed otherwise false.
	 */
	bool checkRemoved()	{return isRemoved;}

	/**
	 * @brief  Set the remove-status of note. The note instance is not removed due to the existance of undo operation.
	 * @param  status  Boolen value of the remove-status.
	 */
	void setRemoved(bool status)	{isRemoved = status;}

	/**
	 * @brief  Get the content of note file, including note location, users, color type, note content and linked images.
	 * @return The content of the file.
	 */
	QString getContent();

	/**
	 * @brief  Get the users that once created or modified this note.
	 * @return The vector of user names.
	 */
	QVector<QString> getUser()	{return mUsers;}

protected:

	/**
	 * @brief  Set note label that shows on the dialog.
	 * @param  text  The label text.
	 */
	void setLabel(QString text);

	/**
	 * @brief  Set note info that appears in the file.
	 * @param  text  The info text.
	 */
	void setInfo(QString text);

	/**
	 * @brief  Set content of the note.
	 * @param  content  The note content.
	 */
	void setText(QString content){mTextEdit->setPlainText(content);}

public slots:

	/**
	 * @brief  Handle Save button.
	 */
	void save();	//Public Slot in order to be connected from the signal emitted by informationWidget

	/**
	 * @brief  Handle Close button.
	 */
	void close() {this->hideNote();}

	/**
	 * @brief  Handle Remove Image button.
	 * @param  id  The removed image id.
	 */
	void removeImage(int id);

protected slots:

	/**
	 * @brief  Handle Add Image button.
	 * @param  fileName  The image absolute path. If it is specified, then add the image directly.
	                     Otherwise, ask user to specify the location.
	 */
	void addImage(const QString fileName = QString());

private slots:

	/**
	 * @brief  Handle Remove button.
	 */
    void remove();

	/**
	 * @brief  If text is changed, set save-status as false.
	 */
    void textChanged();

private:
	/**
	 * @brief  Clear the image layout to remove the attached image.
	 * @param  layout         The image layout.
	 * @param  deleteWidgets  If delete the widgets.
	 */
	void clearLayout(QLayout* layout, bool deleteWidgets = true);

	/**
	 * @brief  Find the mainframe.
	 */
	static MainWindow* mw();

	/**
	 * @brief  Update the note info in the file when users change (A new user
	 *         modify the note therefore should be added to user field).
	 */
	void updateInfo();

	/**
	 * @brief  Update the note label in the dialog when users change (A new user
	 *         modify the note therefore should be added to user field).
	 */
	void updateLabel();

signals:
	/**
	 * @brief  Emit the signal of remove note when the remove button is pressed.
	 * @param  mNoteId  The removed note id.
	 * @param  mPath    The removed note path.
	 */
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

/**
 * This class is the 3D point note.
 */
class PointNote: public Note
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor when create a new 3D point notes with location info.
	 * @param  path    The full note path.
	 * @param  pos     Point position in world coordinate.
	 * @param  noteId  The cell of id the point belongs to.
	 * @param  type    The color of the note.
	 * @param  user    The user who creates the note.
	 */
	PointNote(QString path, double* pos, const int cellId, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	/**
	 * @brief  Constructor when load a 3D point notes from note file.
	 * @param  path       The full note path.
	 * @param  fileName   The name of the note file.
	 * @param  noteId     The cell of id the point belongs to.
	 * @param  isSucceed  Whether the note is created successfully.
	 */
	PointNote(QString path, QString fileName, const int noteId, bool& isSucceed);

	/**
	 * @brief  Get the position of the point note.
	 */
	double* getPosition() { return mPosition; }

	/**
	 * @brief  Get the cell id of the point note.
	 */
	int getCellId() { return mCellId; }

	/**
	 * @brief  Get the note id of the point note.
	 */
	int getNoteId() { return mNoteId; }

	/**
	 * @brief  Remove the note from imformationWidget class. It is called when trying to remove all the notes.
	 */
	void removePointNote();

private:
	double* mPosition;
	int mCellId;
};

/**
 * This class is the 3D surface note.
 */
class SurfaceNote: public Note
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor when create a new 3D surface notes with location info.
	 * @param  path    The full note path.
	 * @param  cellIds The vector of selected cell ids.
	 * @param  type    The color of the note.
	 * @param  user    The user who creates the note.
	 */
	SurfaceNote(QString path, vtkSmartPointer<vtkSelectionNode> cellIds, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	/**
	 * @brief  Constructor when load a 3D surface notes from note file.
	 * @param  path       The full note path.
	 * @param  fileName   The name of the note file.
	 * @param  noteId     The cell of id the point belongs to.
	 * @param  isSucceed  Whether the note is created successfully.
	 */
	SurfaceNote(QString path, QString fileName, const int noteId, bool& isSucceed);

	/**
	 * @brief  Get the cell ids of the point note.
	 * @return The vector of selected cell ids.
	 */
	vtkSmartPointer<vtkSelectionNode> getCellIds() { return mCellIds; }

	/**
	 * @brief  Get the note id of the point note.
	 */
	int getNoteId() { return mNoteId; }

	/**
	 * @brief  Remove the note from imformationWidget class. It is called when trying to remove all the notes.
	 */
	void removeSurfaceNote();

private:
	vtkSmartPointer<vtkSelectionNode> mCellIds;
};

/**
 * This class is the 3D frustum note.
 */
class FrustumNote: public Note
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor when create a new 3D frustum notes with location info.
	 * @param  points   The vector of center points in each plane, which defines the frustum.
	 * @param  normals  The normal vectors at the center points, which defines the frustum.
	 * @param  type     The color of the note.
	 * @param  user     The user who creates the note.
	 */
	FrustumNote(QString path, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, const int noteId,
		const ColorType type = YELLOW, const QString user = QString());

	/**
	 * @brief  Constructor when load a 3D frustum notes from note file.
	 * @param  path       The full note path.
	 * @param  fileName   The name of the note file.
	 * @param  noteId     The cell of id the point belongs to.
	 * @param  isSucceed  Whether the note is created successfully.
	 */
	FrustumNote(QString path, QString fileName, const int noteId, bool& isSucceed);

	/**
	 * @brief  Get the center points of the planes.
	 * @return The vector of center points.
	 */
	vtkSmartPointer<vtkPoints> getPoints() { return mPoints; }

	/**
	 * @brief  Get the normal vectors at the center points of the planes.
	 * @return The vector of normal vectors.
	 */
	vtkSmartPointer<vtkDataArray> getNormals() { return mNormals; }

	/**
	 * @brief  Get the note id of the point note.
	 */
	int getNoteId() { return mNoteId; }

	/**
	 * @brief  Remove the note from imformationWidget class. It is called when trying to remove all the notes.
	 */
	void removeFrustumNote();

private:
	vtkSmartPointer<vtkPoints> mPoints;
	vtkSmartPointer<vtkDataArray> mNormals;
};

/**
 * This class is the 2D point note.
 */
class PointNote2D: public Note
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor when create a new 2D point notes with location info.
	 * @param  path        The full note path.
	 * @param  point       Point position in world coordinate.
	 * @param  pointImage  The image coordinate, which is saved to mark the note on report.
	 * @param  type        The color of the note.
	 * @param  user        The user who creates the note.
	 */
	PointNote2D(QString path, const double* point, const int* pointImage, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	/**
	 * @brief  Constructor when load a 2D point notes from note file.
	 * @param  path       The full note path.
	 * @param  fileName   The name of the note file.
	 * @param  noteId     The cell of id the point belongs to.
	 * @param  isSucceed  Whether the note is created successfully.
	 */
	PointNote2D(QString path, QString fileName, const int noteId, bool& isSucceed);

	/**
	 * @brief  Get the position of the point note.
	 */
	double* getPoint() { return mPoint; }

	/**
	 * @brief  Get the note id of the point note.
	 */
	int getNoteId() { return mNoteId; }

	/**
	 * @brief  Remove the note from imformationWidget class. It is called when trying to remove all the notes.
	 */
	void removePointNote2D();

private:
	double* mPoint;	// The size is three, x, y ,0
	int* mImagePoint;	// The size is three, x, y ,0, image coordinate
};

/**
 * This class is the 2D surface note.
 */
class SurfaceNote2D: public Note
{
	Q_OBJECT

public:
	/**
	 * @brief  Constructor when create a new 2D surface notes with location info.
	 * @param  path        The full note path.
	 * @param  point       Four corner point positions in world coordinate.
	 * @param  pointImage  The image coordinate of four corner points, which is saved to mark the note on report.
	 * @param  type        The color of the note.
	 * @param  user        The user who creates the note.
	 */
	SurfaceNote2D(QString path, const double* point, const int* pointImage, const int noteId, const ColorType type = YELLOW, const QString user = QString());

	/**
	 * @brief  Constructor when load a 2D surface notes from note file.
	 * @param  path       The full note path.
	 * @param  fileName   The name of the note file.
	 * @param  noteId     The cell of id the point belongs to.
	 * @param  isSucceed  Whether the note is created successfully.
	 */
	SurfaceNote2D(QString path, QString fileName, const int noteId, bool& isSucceed);

	/**
	 * @brief  Get four corner points of the the note.
	 */
	double* getPoint() { return mPoint; }

	/**
	 * @brief  Get the note id of the point note.
	 */
	int getNoteId() { return mNoteId; }

	/**
	 * @brief  Remove the note from imformationWidget class. It is called when trying to remove all the notes.
	 */
	void removeSurfaceNote2D();

private:
	double* mPoint;	// The size is four, start x, y, end x, y
	int* mImagePoint;	// The size is four, start x, y, end x, y, image coordinate
};
#endif // NOTE_H