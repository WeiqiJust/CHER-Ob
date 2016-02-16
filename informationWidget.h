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


class Information : public QWidget
{
  Q_OBJECT

public:
  Information(QWidget *parent = 0);
  ~Information();

  static MainWindow* mw(); // find the mainframe

  void initAnnotation(const QString path);

  void startAnnotation();

  void finishAnnotation();

  void reloadAnnotation();

  void saveAllNotes();

  void removeAllNotes();

  void closeAllNotes();

  void closeObjectNotes();

  void createPointNote(double *pos, int cellId, ColorType color);

  void createSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds, ColorType color);

  void createFrustumNote(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkDataArray> normals, ColorType color);

  void createPointNote2D(double* point, ColorType color);

  void createSurfaceNote2D(double* point, ColorType color);

  void openPointNote(int cellId);

  void openSurfaceNote(vtkSmartPointer<vtkSelectionNode> cellIds);

  void openFrustumNote(vtkSmartPointer<vtkPlanes> planes);

  void openPointNote2D(double* point);

  void openSurfaceNote2D(double* point);

  void openNoteFromTreeWidget(QTreeWidgetItem* item);

signals:
  void saveAll();

  void closeAll();

  void updateMenu();

public slots:

  void removePointNote(int noteId, QString* path);

  void removeSurfaceNote(int noteId, QString* path);

  void removeFrustumNote(int noteId, QString* path);

  void removePointNote2D(int noteId, QString* path);

  void removeSurfaceNote2D(int noteId, QString* path);

private:

  void loadPointNote(const QString path);

  void loadSurfaceNote(const QString path);

  void loadFrustumNote(const QString path);

  void loadPointNote2D(const QString path);

  void loadSurfaceNote2D(const QString path);

  void hideNotes();

  void showNotes();

  bool updateCurrentPath();

private slots:

  void enableButtons();

  void editingAnnotation();

  void saveAnnotation();

  void removeAnnotation();

private:
  
  QHBoxLayout* hlay1;
  QHBoxLayout* hlay2;
  QVBoxLayout* vlay;
  QTextBrowser* dText;
  QPushButton* editButton;
  QPushButton* saveButton;
  QPushButton* removeButton;
  QString notePath;
  QMap<QString, QString> content;
  QMap<QString, QVector<PointNote*>> mPointNotes;
  QMap<QString, QVector<SurfaceNote*>> mSurfaceNotes;
  QMap<QString, QVector<FrustumNote*>> mFrustumNotes;
  QMap<QString, QVector<PointNote2D*>> mPointNotes2D;
  QMap<QString, QVector<SurfaceNote2D*>> mSurfaceNotes2D;
};
#endif // INFORMATION_H

