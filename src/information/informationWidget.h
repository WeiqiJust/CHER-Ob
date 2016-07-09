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
  // Constructor and Deconstructor.
  Information(QWidget *parent = 0);
  ~Information();

  // find the mainframe
  static MainWindow* mw();

  /**
   * Clear the all the vectors and annotation.
   * It is triggered when a project/CHE is closed.
   */
  void refresh();

  /**
   * Init the widget by loading existing notes and annotation.
   * Input is the absolute object path.
   */
  void initAnnotation(const QString path);

  /**
   * Enable annotation when annotation button in the main frame is checked.
   */
  void startAnnotation();

  /**
   * Disable annotation when annotation button in the main frame is unchecked.
   */
  void finishAnnotation();

  /**
   * Load the correct content of annotation when a different object window is active.
   */
  void reloadAnnotation();

  /**
   * Remove the content of current object's annotation.
   */
  void clearAnnotation();

  /**
   * Save all the notes and set all "hasNoteRemoved" flags as false.
   */
  void saveAllNotes();

  /**
   * Save current object's notes and set its "hasNoteRemoved" flag as false.
   */
  void saveObjectNotes();

  /**
   * Remove all the current object's notes by setting their "removed" flags as true. Not delete the note instance.
   */
  void removeAllNotes();

  /**
   * Remove all the specified object's notes by setting their "removed" flags as true. Not delete the note instance.
   * Input is the object full path.
   */
  void removeAllNotes(QString path);

  /**
   * Remove current object's unsaved notes by setting their "removed" flags as true. Not delete the note instance.
   */
  void removeUnSavedNotes();

  /**
   * Close all objects' notes by sending the signal. All the note dialogs are hidden.
   */
  void closeAllNotes();

  /**
   * Close current object's notes by sending the signal. All the note dialogs are hidden.
   */
  void closeObjectNotes();

  /**
   * Create 3D notes with location info.
   */
  void createPointNote(double *pos, int cellId, ColorType color);

  void createSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, ColorType color);

  void createFrustumNote(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, ColorType color);

  /**
   * Create 2D notes with location info.
   * Point is world coordinate.
   * PointImage is the image coordinate, which is saved to mark the note on report.
   * Color is the color for the note mark.
   */
  void createPointNote2D(double* point, int* pointImage, ColorType color);

  void createSurfaceNote2D(double* point, int* pointImage, ColorType color);

  /**
   * Open 3D notes with location info.
   */
  void openPointNote(int cellId);

  void openSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds);

  void openFrustumNote(vtkSmartPointer<vtkPlanes> planes);

  /**
   * Open 2D notes with location info.
   */
  void openPointNote2D(double* point);

  void openSurfaceNote2D(double* point);

  /**
   * Open notes from the tree widget in search widget when the item is double clicked.
   * Input is the double clicked item.
   */
  void openNoteFromTreeWidget(QTreeWidgetItem* item);

  /**
   * Open notes from the navigation tab when the item is double clicked.
   * Input is the double clicked item.
   */
  void openNoteFromNavigation(QTreeWidgetItem* item);

  /**
   * Open the notes with the given users.
   */
  void openNotesByUsers(const QVector<QString> users);

  /**
   * Undo the remove operation from navigation tab.
   * Item is the note that should be undo for removal.
   */
  void undoRemoveNote(QTreeWidgetItem* item);

  /**
   * Check whether all the objects' notes are saved.
   * If there are notes that are not saved, return false, otherwise return true.
   */
  bool checkAllSaved();

  /**
   * Check whether current object's notes are saved.
   * If there are notes that are not saved, return false, otherwise return true.
   */
  bool checkObjectSaved();

  /**
   * Get the note data of specified object, including note info and content. Annotation is saved as the first element.
   */
  QVector<QString> getAllNotes(const QString objectPath);

  /**
   * Get the number of notes for each note type (annotation, point, surface, frustm, point2d, surface2d)for navigation.
   */
  QVector<int> getNoteNumber(const QString objectPath);	

  /**
   * Get all users that appear in the notes.
   */
  QVector<QString> getAllUsers();

signals:
  /**
   * Save all the notes.
   */
  void saveAll();

  /**
   * Close all the notes.
   */
  void closeAll();

  /**
   * Trigger main frame to update menu.
   */
  void updateMenu();

  /**
   * Add an item in navigation tab when a note is created.
   * Path is the object absolute path.
   * Type is the note mode.
   */
  void addNavigationItem(const QString path, const NoteMode type);

  /**
   * Remove the corresponding item in navigation tab when a note is removed.
   * Path is the object absolute path.
   * Type is the note mode.
   * Id is the note id.
   */
  void removeNavigationItem(const QString path, const NoteMode type, const int id);

public slots:

  /**
   * Remove the 3D/2D note in curret object by using noteId.
   */
  void removePointNote(int noteId, QString* path);

  void removeSurfaceNote(int noteId, QString* path);

  void removeFrustumNote(int noteId, QString* path);

  void removePointNote2D(int noteId, QString* path);

  void removeSurfaceNote2D(int noteId, QString* path);

private:

  /**
   * Load the 3D/2D note from the absolute path.
   */
  bool loadPointNote(const QString path);

  bool loadSurfaceNote(const QString path);

  bool loadFrustumNote(const QString path);

  bool loadPointNote2D(const QString path);

  bool loadSurfaceNote2D(const QString path);

  /**
   * Hide note dialog.
   */
  void hideNotes();

  /**
   * Show note dialog.
   */
  void showNotes();

  /**
   * Set the notepath to the current object's note path.
   * Return true if success.
   * Return false if current object cannot be processed.
   */
  bool updateCurrentPath();

private slots:

  /**
   * If skipTextChange flag is set as false, update and cache the annotation content.
   */
  void annotationChanges();

  /**
   * Save current annotation.
   */
  void saveAnnotation();

  /**
   * Remove current annotation, including files.
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

