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
	 * @param  path               The absolute object path.
	 * @param  isDisplayNoteMark  Whether or not to display note marks.
	 */
	void init(const QString path, bool isDisplayNoteMark = false);

	/**
	 * @brief  Init the widget for CT 2d rendering mode (default open mode) by loading existing notes and annotation.
	 * @param  path  The absolute object path.
	 * @param  isDisplayNoteMark  Whether or not to display note marks.
	 */
	void initCT2DRendering(const QString path, bool isDisplayNoteMark = false);

	/**
	 * @brief  Init the widget for CT volume rendering mode by loading existing notes and annotation.
	 * @param  path  The absolute object path.
	 * @param  isDisplayNoteMark  Whether or not to display note marks.
	 */
	void initCTVolumeRendering(const QString path, bool isDisplayNoteMark = false);

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
	 * @brief  Clear the all the note marks.
	 */
	void removeAllNotesMark();

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
	 * @param  cellId     The vector of selected cell ids.
	 * @param  points     The vector of four corner points in world coordinate.
	 * @param  color      The color of the note.
	 * @param  isCTVolum  If the note is appiled to CT Volume. If it is, then cellIds is NULL and should not be accessed.
	 */
	void createSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, QVector<double*> points, ColorType color, bool isCTVolume);

	/**
	 * @brief  Create 3D frustum notes with location info.
	 * @param  points   The vector of center points in each plane, which defines the frustum.
	 * @param  normals  The normal vectors at the center points, which defines the frustum.
	 * @param  color    The color of the note.
	 */
	void createFrustumNote(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, ColorType color);

	/**
	 * @brief  Create 2D notes with location info.
	 * @param  point       Point position in world coordinate.
	 * @param  pointImage  The image coordinate, which is saved to mark the note on report.
	 * @param  color       The color of the note.
	 */
	void createPointNote2D(double* point, int* pointImage, ColorType color);

	/**
	 * @brief  Create 2D notes with location info.
	 * @param  point       Four corner point positions in world coordinate.
	 * @param  pointImage  The image coordinate of four corner points, which is saved to mark the note on report.
	 * @param  color       The color of the note.
	 */
	void createSurfaceNote2D(double* point, int* pointImage, ColorType color);

	/**
	 * @brief  Create 2D notes with location info.
	 * @param  polygon		Polygon vertices point positions in world coordinate.
	 * @param  polygonImage	The vector of a series of image coordinates, which is saved to mark the note on report.
	 * @param  color		The color of the note.
	 */
	void createPolygonNote2D(std::vector<std::pair<double, double> >* polygon, std::vector<std::pair<int, int> >* polygonImage, ColorType color);

	/**
	 * @brief  Open 3D point notes with location info.
	 * @param  cellId  The selected cell id.
	 */
	void openPointNote(int cellId);

	/**
	 * @brief  Open 3D surface notes with location info.
	 * @param  cellIds       The vector of selected cell ids.
	 * @param  cornerPoints  The vector of four corner points that define selected area.
	 * @param  isCTVolume    If the note is appiled to CT Volume data.
	 */
	void openSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, std::vector<double*> cornerPoints, bool isCTVolume = false);

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
	 * @param  surface Four corner point positions in world coordinate.
	 */
	void openSurfaceNote2D(double* point);

	/**
	 * @brief  Open 2D polygon notes with location info.
	 * @param  polygon polygon vertices point positions in world coordinate.
	 */
	void openPolygonNote2D(std::vector<std::pair<double, double> >* polygon);

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
	 * @brief  Replace the old user name to new user name. If old name is none, then add the new name.
	 * @param  new and old user names.
	 */
	void replaceNotesUserName(const QString newName, const QString oldName = QString());

	/**
	 * @brief  Draw 2D Note marks on the 2D images. This function is used to display CT 2D rendering note marks.
	 * @param  path  The absolute object path.
	 */
	void draw2DNoteMark(const QString path);

	/**
	 * @brief  Draw 3D Note marks on the 3D objects. This function is used to display CT Volume rendering note marks.
	 * @param  path  The absolute object path.
	 */
	void draw3DNoteMark(const QString path);

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
	 * @return The vector of pair <content, note type> of annotation and all notes file. The note type implies whether the note is
	 *         appiled to 2D or 3D object.
	 */
	QVector<QPair<QString, NoteType> > getAllNotes(const QString objectPath);

	/**
	 * @brief  Get the number of notes for each note type (annotation, point, surface, frustm, point2d, surface2d, polygon2d) for navigation.
	 * @param  objectPath  Object full path.
	 * @return The vector of note numbers of each type: annotation (0 or 1), 3d point, 3d surface, 3d frustum, 2d point, 2d surface, 2d polygon.
	 */
	QVector<int> getNoteNumber(const QString objectPath);	

	/**
	 * @brief  Get all users that appear in the notes.
	 * @return The vector of all users.
	 */
	QVector<QString> getAllUsers();
	
	/**
	 * Added by Ying to copy annotations
	 */
	QString getNoteFileName(QTreeWidgetItem* item);
	QVector<QString> getNoteModeType(QTreeWidgetItem* item);

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
	 * @brief  Replace the old user name to new user name. If old name is none, then add the new name.
	 * @param  new and old user names.
	 */
	void replaceUserName(const QString newName, const QString oldName);

	/**
	 * @brief  Trigger main frame to update menu.
	 */
	void updateMenu();

	/**
	 * @brief  Add an item in navigation tab when a note is created.
	 * @param  Path  The object absolute path.
	 * @param  type  The note mode.
	 * @param  dim   Whether the note belongs to 2D or 3D object. NONE is for annotation.
	 */
	void addNavigationItem(const QString path, const NoteMode type, const NoteType dim = NONE);

	/**
	 * @brief  Remove the corresponding item in navigation tab when a note is removed.
	 * @param  Path  The object absolute path.
	 * @param  Type  The note mode.
	 * @param  Id    The note id.
	 * @param  dim   Whether the note belongs to 2D or 3D object. NONE is for annotation.
	 */
	void removeNavigationItem(const QString path, const NoteMode type, const int id, const NoteType dim = NONE);

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
	void removePolygonNote2D(int noteId, QString* path);

public:
	/**
	 * @brief  Load the 3D/2D note from the absolute path.
	 * @param  path               The absolute note path.
	 * @param  isLoadNoteMark     Whether or not to draw the note mark.
	 * @param  isDisplayNoteMark  Whether or not to display note marks. If isLoadNoteMark is set false, then isDisplayNoteMark should also be set false.
	 * @return If note is correctly loaded return true, otherwise false.
	 */
	bool loadPointNote(const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadSurfaceNote(const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadFrustumNote(const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadPointNote2D(const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadSurfaceNote2D(const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadPolygonNote2D(const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);

	/**
	 * Added by Ying to copy annotations
	 */
	bool loadPointNoteFromFile(const QString fileName, const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadSurfaceNoteFromFile(const QString fileName, const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadFrustumNoteFromFile(const QString fileName, const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadPointNote2DFromFile(const QString fileName, const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadSurfaceNote2DFromFile(const QString fileName, const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	bool loadPolygonNote2DFromFile(const QString fileName, const QString path, bool isLoadNoteMark = true, bool isDisplayNoteMark = false);
	
	void setNotePath(QString path);
	void recoverNotePath();

	/**
	 * @brief  Set geographical information for an object
	 */
	void setGeoInfo(const QString objectName, QPair<double, double> latlong);

	/**
	 * @brief  Get geographical information of an object
	 */
	QPair<double, double> getGeoInfo(const QString objectName);

private:
	/**
	 * @brief  Load Annotation from the absolute path.
	 * @param  path    The absolute note path.
	 * @return If annotation is correctly loaded return true, otherwise false.
	 */
	bool loadAnnotation(const QString notePath);

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
	QString notePathPre;
	QMap<QString, std::pair<QString, bool > > content;	// Annotation
	QMap<QString, QPair<double, double> > geoinfo;	// GeoInfo
    QMap<QString, QVector<PointNote*> > mPointNotes;
    QMap<QString, QVector<SurfaceNote*> > mSurfaceNotes;
    QMap<QString, QVector<FrustumNote*> > mFrustumNotes;
    QMap<QString, QVector<PointNote2D*> > mPointNotes2D;
    QMap<QString, QVector<SurfaceNote2D*> > mSurfaceNotes2D;
    QMap<QString, QVector<PolygonNote2D*> > mPolygonNotes2D;
	QMap<QString, bool> hasNotesRemoved;
	QMap<QString, QString> removedAnnotation;
	bool skipTextChange;
};
#endif // INFORMATION_H

