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
   annonymous help by various software engineering communities.

*****************************************************************************/
#ifndef INFORMATION_H
#define INFORMATION_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QToolBar>
#include <QTextBrowser>
#include <QVector>
#include <vtkPlanes.h>
#include <vtkPlane.h>
#include <QTabWidget>
#include "note.h"

// Class list
class MainWindow;

// DT: DTextEdit class extends QTextEdit to provide basic rich text editing capabilities.

class DTextEdit : public QWidget
{
    Q_OBJECT

public:
    DTextEdit(QWidget *parent = 0);
    ~DTextEdit();

private:

    QAction *actionSave;
    QAction *actionTextBold;
    QAction *actionTextUnderline,
    *actionTextItalic,
    *actionTextColor,
    *actionAlignLeft,
    *actionAlignCenter,
    *actionAlignRight,
    *actionAlignJustify,
    *actionUndo,
    *actionRedo,
    *actionCut,
    *actionCopy,
    *actionPaste;

  QComboBox *comboStyle;
  QFontComboBox *comboFont;
  QComboBox *comboSize;

  QToolBar *tb;
  QString fileName;
  QTextEdit *textEdit;
};

/**
 * This class is designed for imformation widget, including various types of note and annotation.
 */

class Information : public QWidget
{
  Q_OBJECT

public:
	/**
	 * @brief Constructor and Deconstructor.
	 */
	Information(QWidget *parent = 0);
	~Information();

	/**
	 * @brief  find the mainframe.
	 */
	static MainWindow* mw();

	/**
	 * @brief  Clear the all the vectors and annotation.
	 *         It is triggered when a project/CHE is closed.
	 */
	void refresh();

	/**
	 * @brief  Init the widget by loading existing notes and annotation.
	 * @param  path  The absolute object path.
	 */
	void initAnnotation(const QString path);

	/**
	 * @brief  Enable annotation when annotation button in the main frame is checked.
	 */
	void startAnnotation();

	/**
	 * @brief  Disable annotation when annotation button in the main frame is unchecked.
	 */
	void finishAnnotation();

	/**
	 * @brief  Load the correct content of annotation when a different object window is active.
	 */
	void reloadAnnotation();

	/**
	 * @brief  Remove the content of current object's annotation.
	 */
	void clearAnnotation();

	/**
	 * @brief  Save all the notes and set all "hasNoteRemoved" flags as false.
	 */
	void saveAllNotes();

	/**
	 * @brief  Save current object's notes and set its "hasNoteRemoved" flag as false.
	 */
	void saveObjectNotes();

	/**
	 * @brief  Remove all the current object's notes by setting their "removed" flags as true. Not delete the note instance.
	 */
	void removeAllNotes();

	/**
	 * @brief  Remove all the specified object's notes by setting their "removed" flags as true. Not delete the note instance.
	 * @param  path  The object full path.
	 */
	void removeAllNotes(QString path);

	/**
	 * @brief  Remove current object's unsaved notes by setting their "removed" flags as true. Not delete the note instance.
	 */
	void removeUnSavedNotes();

	/**
	 * @brief  Close all objects' notes by sending the signal. All the note dialogs are hidden.
	 */
	void closeAllNotes();

	/**
	 * @brief  Close current object's notes by sending the signal. All the note dialogs are hidden.
	 */
	void closeObjectNotes();

	/**
	 * @brief  Create 3D point notes with location info.
	 * @param  pos     Point position in world coordinate.
	 * @param  cellId  The cell of id the point belongs to.
	 * @param  color   The color of the note.
	 */
	void createPointNote(double *pos, int cellId, ColorType color);

	/**
	 * @brief  Create 3D surface notes with location info.
	 * @param  cellId  The vector of selected cell ids.
	 * @param  color   The color of the note.
	 */
	void createSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, ColorType color);

	/**
	 * @brief  Create 3D frustum notes with location info.
	 * @param  points  The vector of center points in each plane, which defines the frustum.
	 * @param  points  The normal vectors at the center points, which defines the frustum.
	 * @param  color   The color of the note.
	 */
	void createFrustumNote(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, ColorType color);

	/**
	 * @brief  Create 2D notes with location info.
	 * @param  point       Point position in world coordinate.
	 * @param  PointImage  The image coordinate, which is saved to mark the note on report.
	 * @param  Color       The color of the note.
	 */
	void createPointNote2D(double* point, int* pointImage, ColorType color);

	/**
	 * @brief  Create 2D notes with location info.
	 * @param  point       Four corner point positions in world coordinate.
	 * @param  PointImage  The image coordinate of four corner points, which is saved to mark the note on report.
	 * @param  Color       The color of the note.
	 */
	void createSurfaceNote2D(double* point, int* pointImage, ColorType color);

	/**
	 * @brief  Open 3D point notes with location info.
	 * @param  cellId  The selected cell id.
	 */
	void openPointNote(int cellId);

	/**
	 * @brief  Open 3D surface notes with location info.
	 * @param  cellIds  The vector of selected cell ids.
	 */
	void openSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds);

	/**
	 * @brief  Open 3D frustum notes with location info.
	 * @param  planes  The planes that define the frustum.
	 */
	void openFrustumNote(vtkSmartPointer<vtkPlanes> planes);

	/**
	 * @brief  Open 2D point notes with location info.
	 * @param  point  Point position in world coordinate.
	 */
	void openPointNote2D(double* point);

	/**
	 * @brief  Open 2D surface notes with location info.
	 * @param  point  Four corner point positions in world coordinate.
	 */
	void openSurfaceNote2D(double* point);

	/**
	 * @brief  Open notes from the tree widget in search widget when the item is double clicked.
	 * @param  item  The double clicked item.
	 */
	void openNoteFromTreeWidget(QTreeWidgetItem* item);

	/**
	 * @brief  Open notes from the navigation tab when the item is double clicked.
	 * @param  item  The double clicked item.
	 */
	void openNoteFromNavigation(QTreeWidgetItem* item);

	/**
	 * @brief  Open the notes with the given users.
	 * @param  users  The vector of users that is used to filter notes.
	 */
	void openNotesByUsers(const QVector<QString> users);

	/**
	 * @brief  Undo the remove operation from navigation tab.
	 * @param  item  The note that should be undo for removal.
	 */
	void undoRemoveNote(QTreeWidgetItem* item);

	/**
	 * @brief  Check whether all the objects' notes are saved.
	 * @return If there are notes that are not saved, return false, otherwise return true.
	 */
	bool checkAllSaved();

	/**
	 * @brief  Check whether current object's notes are saved.
	 * @return If there are notes that are not saved, return false, otherwise return true.
	 */
	bool checkObjectSaved();

	/**
	 * @brief  Get the note data of specified object, including note info and content. Annotation is saved as the first element.
	 * @param  objectPath  Object full path.
	 * @return The vector of content of annotation and all notes file.
	 */
	QVector<QString> getAllNotes(const QString objectPath);

	/**
	 * @brief  Get the number of notes for each note type (annotation, point, surface, frustm, point2d, surface2d)for navigation.
	 * @param  objectPath  Object full path.
	 * @return The vector of note numbers of each type: annotation (0 or 1), 3d point, 3d surface, 3d frustum, 2d point, 2d surface.
	 */
	QVector<int> getNoteNumber(const QString objectPath);	

	/**
	 * @brief  Get all users that appear in the notes.
	 * @return The vector of all users.
	 */
	QVector<QString> getAllUsers();

signals:
	/**
	 * @brief  Save all the notes.
	 */
	void saveAll();

	/**
	 * @brief  Close all the notes.
	 */
	void closeAll();

	/**
	 * @brief  Trigger main frame to update menu.
	 */
	void updateMenu();

	/**
	 * @brief  Add an item in navigation tab when a note is created.
	 * @param  Path  The object absolute path.
	 * @param  type  The note mode.
	 */
	void addNavigationItem(const QString path, const NoteMode type);

	/**
	 * @brief  Remove the corresponding item in navigation tab when a note is removed.
	 * @param  Path  The object absolute path.
	 * @param  Type  The note mode.
	 * @param  Id    The note id.
	 */
	void removeNavigationItem(const QString path, const NoteMode type, const int id);

	public slots:

	/**
	 * @brief  Remove the 3D/2D note in curret object by using noteId.
	 * @param  noteId  The note id number.
	 * @param  path    The absolute note path.
	 */
	void removePointNote(int noteId, QString* path);
	void removeSurfaceNote(int noteId, QString* path);
	void removeFrustumNote(int noteId, QString* path);
	void removePointNote2D(int noteId, QString* path);
	void removeSurfaceNote2D(int noteId, QString* path);

private:

	/**
	 * @brief  Load the 3D/2D note from the absolute path.
	 * @param  path    The absolute note path.
	 * @return If note is correctly loaded return true, otherwise false.
	 */
	bool loadPointNote(const QString path);
	bool loadSurfaceNote(const QString path);
	bool loadFrustumNote(const QString path);
	bool loadPointNote2D(const QString path);
	bool loadSurfaceNote2D(const QString path);

	/**
	 * @brief  Hide the note.
	 */
	void hideNotes();

	/**
	 * @brief  Show note dialog.
	 */
	void showNotes();

	/**
	 * @brief  Set the notepath to the current object's note path.
	 * @return true if success, false if current object cannot be processed.
	 */
	bool updateCurrentPath();

	private slots:

	/**
	 * @brief  If skipTextChange flag is set as false, update and cache the annotation content.
	 */
	void annotationChanges();

	/**
	 * @brief  Save current annotation.
	 */
	void saveAnnotation();

	/**
	 * @brief  Remove current annotation, including files.
	 */
	void removeAnnotation();

private:

	QHBoxLayout* hlay1;
	QHBoxLayout* hlay2;
	QVBoxLayout* vlay;
	QTextBrowser* dText;
	QPushButton* saveButton;
	QPushButton* removeButton;
	QString notePath;
	QMap<QString, std::pair<QString, bool > > content;	// Annotation
	QMap<QString, QVector<PointNote*>> mPointNotes;
	QMap<QString, QVector<SurfaceNote*>> mSurfaceNotes;
	QMap<QString, QVector<FrustumNote*>> mFrustumNotes;
	QMap<QString, QVector<PointNote2D*>> mPointNotes2D;
	QMap<QString, QVector<SurfaceNote2D*>> mSurfaceNotes2D;
	QMap<QString, bool> hasNotesRemoved;
	QMap<QString, QString> removedAnnotation;
	bool skipTextChange;
};
#endif // INFORMATION_H

