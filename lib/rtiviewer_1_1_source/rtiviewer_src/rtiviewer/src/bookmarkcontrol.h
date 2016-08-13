/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright    (C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR                                  /\/|      *
* and                                                                 |      *
* Cultural Heritage Imaging                                             \      *
*                                                                            *
* This program is free software: you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                        *
*                                                                            *
* This program is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU General Public License for more details.                                *
*                                                                            *
* You should have received a copy of the GNU General Public License            *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.        *
****************************************************************************/
#ifndef BOOKMARKCONTROL_H
#define BOOKMARKCONTROL_H

#include "rtiBrowser.h"
#include "gui.h"
#include "lightControl.h"
#include "renderingdialog.h"
#include "rti.h"

#include <vcg/space/point3.h>

#include <QDateTime>
#include <QFileInfo>
#include <QMap>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QToolTip>

#include <QGroupBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>

// BookmarkControl and RtiViewerDlg both point to each other. Forward declare
// RtiViewerDlg to resolve the circular dependency.

class RtiViewerDlg;

// BookmarkControl points to RtiBrowser, which points to RtiViewerDlg, which
// points to BookmarkControl. Forward declare RtiBrowser to resolve the
// circular dependency.

class RtiBrowser;

/*====================================================================
 *
 * RenderingParam class header
 *
 *===================================================================*/

//! Rendering parameter class.
/*!
  The RenderingParam class stores a value for a rendering parameter.
*/

class RenderingParam
{
public:

    /*====================================================================
     * Public methods
     *===================================================================*/

    //! Constructor
    /*!
      Constructs the rendering parameter.
      \param s Rendering parameter name.
      \param v Rendering parameter value.
    */

    RenderingParam(QString s, float v);

    /*====================================================================
     * Public data members
     *===================================================================*/

    QString name; /*!< Rendering parameter name. */
    float value; /*!< Rendering parameter value. */
};

/*====================================================================
 *
 * Note class header
 *
 *===================================================================*/

//! Note class.
/*!
  The Note class stores a single note for a bookmark.
*/

class Note
{
public:

    /*====================================================================
     * Public methods
     *===================================================================*/

    //! Constructor
    /*!
      Constructs a note.
    */
    Note();

    //! Constructor
    /*!
      Constructs a note.
      \param i ID of the note
      \param s Subject of the note
      \param a Author of the note
      \param t Timestamp of the note
      \param c The in-depth note
    */
    Note(int i, QString s, QString a, QDateTime t, QString c);

    /*====================================================================
     * Public data members
     *===================================================================*/

    int id; /*!< ID of the note. */
    QString subject; /*!< Subject of the note. */
    QString author; /*!< Author of the note. */
    QDateTime timestamp; /*!< Timestamp of the note. */
    QString comment; /*!< The in-depth note. */
};

/*====================================================================
 *
 * Bookmark class header
 *
 *===================================================================*/

//! Bookmark class.
/*!
  The Bookmark class stores a single bookmark.
*/

class Bookmark
{
public:

    /*====================================================================
     * Public methods
     *===================================================================*/

    //! Constructor
    /*!
      Constructs a bookmark.
    */

    Bookmark();

    //! Constructor
    /*!
      Constructs a bookmark and sets the ID.
      \param id The bookmark ID
    */

    Bookmark(int id);

    /*====================================================================
     * Public data members
     *===================================================================*/

    int id; /*!< ID of the bookmark. */
    QString name; /*!< Name of the bookmark. */
    QString rtiViewerID; /*!< ID of the RTIViewer that created the bookmark. */
    bool hasHighlightBox; /*!< Whether the bookmark has a highlight box. */
    QRectF highlightBox; /*!< The highlight box. Undefined if hasHighlightBox is false. */
    int zoom; /*!< The zoom factor as a percent. */
    QPointF pan; /*!< Coordinates of the origin of the current view. */
    QPointF incidence; /*!< Coordinates of the light incidence. */
    int renderingMode; /*!< The rendering mode. See RenderingRTI enum in rti.h. */
    QList<RenderingParam> params; /*!< A list of render parameters. May be empty. */
    QMap<int, Note> notes; /*!< A list of notes. May be empty. */
};

/*====================================================================
 *
 * NoteDialog class header
 *
 *===================================================================*/

//! NoteDialog class.
/*!
  The NoteDialog class displays a dialog to enter or edit a note.
*/

class NoteDialog: public QDialog
{
    Q_OBJECT

public:

    /*====================================================================
     * Public methods
     *===================================================================*/

    //! Constructor
    /*!
      Constructs a note dialog.
      \param parent The parent widget
      \param username Name of the user writing the note
      \param subject Subject of the note
      \param comment In-depth note
      \param i ID of the note. -1 for new notes; > -1 when editing an existing note.
    */

    NoteDialog(QWidget * parent, QString username, QString subject = QString(), QString comment = QString(), int i = -1);

private:

    /*====================================================================
     * Private data members
     *===================================================================*/

    int id;
    QLineEdit * subjectEdit;
    QLineEdit * authorEdit;
    QTextEdit * commentEdit;
    QPushButton * okButton;
    QPushButton * cancelButton;

    /*====================================================================
     * Private methods
     *===================================================================*/

    QFormLayout * initForm(QString subject, QString username, QString comment);
    QHBoxLayout * initButtons();
    void initLayout(QFormLayout * formLayout, QHBoxLayout * buttonLayout);
    void setConnections();

    /*====================================================================
     * Signals
     *===================================================================*/

signals:

    void noteCreated(QString subject, QString author, QDateTime dateTime, QString comment);

    void noteEdited(int id, QString subject, QString author, QDateTime dateTime, QString comment);

    /*====================================================================
     * Private slots
     *===================================================================*/

private slots:

    void okButtonClicked();

};

/*====================================================================
 *
 * BookmarkControl class header
 *
 *===================================================================*/

class BookmarkControl: public QFrame
{

    Q_OBJECT

public:

    /*====================================================================
     * Public methods
     *===================================================================*/

    //! Constructor
    /*!
      Constructs bookmark control.
      \param d The calling RtiViewerDlg object
      \param b The RtiBrowser used by the RtiViewerDlg
      \param l The LightControl used by the RtiViewerDlg
      \param r The RenderingDialog used by the RtiViewerDlg
      \param parent The parent widget
    */

    BookmarkControl(RtiViewerDlg * d, RtiBrowser * b, LightControl * l, RenderingDialog * r, QWidget *parent = 0);

    //! Load the bookmark file
    /*!
      Loads the bookmark file, if any.
      \param info The QFileInfo object containing the path to the bookmark file
    */

    void loadBookmarks(const QFileInfo &info);

private:

    /*====================================================================
     * Private data members
     *===================================================================*/

    // Variables used by the GUI

    QGroupBox * bmSelectorGroup;
    QComboBox * bmComboBox;
    QPushButton * addBookmarkButton;
    QPushButton * deleteBookmarkButton;

    QGroupBox * notesGroup;
    QListWidget * notesList;
    QPushButton * addNoteButton;
    QPushButton * editNoteButton;
    QPushButton * deleteNoteButton;

    QGroupBox * highlightBoxGroup;
    QPushButton * addBoxButton;
    QPushButton * editBoxButton;
    QPushButton * deleteBoxButton;

    QGroupBox * lightingGroup;
    QPushButton * updateLightingButton;

    // Variables used to manage bookmarks

    QMap<int, Bookmark> bookmarks;
    int currentID;
    QString path;

    // Member variables that point to controls. These are used to
    // get the light incidence, zoom, pan, and rendering parameters.

    RtiViewerDlg * gui;
    RtiBrowser * browser;
    LightControl * light;
    RenderingDialog * renderingDialog;

    /*====================================================================
     * Private methods
     *===================================================================*/

    // Methods for building the GUI

    void initSelector();

    void initNotes();

    void initHighlightBox();

    void initLighting();

    void initLayout();

    void setConnections();

    // Methods for working with the GUI

    void populateBookmarkComboBox();

    void displayBookmark();

    vcg::Point3f getPoint3f(QPointF point);

    void displayRendering(const Bookmark &bookmark);

    void displayNotes();

    // Methods for managing bookmarks

    void setPath(const QFileInfo &info);

    bool loadBookmarkFile();

    void saveBookmarkFile();

    int getNextID(QList<int> keys);

    void getRenderingMode(Bookmark &bm);

    /*====================================================================
     * Signals
     *===================================================================*/

signals:

    //! Emitted when the zoom changes.
    /*!
      This signal is emitted when loading a bookmark.
      \param zoom The new zoom value, as a percent.
    */

    void zoomChanged(int zoom);

    //! Emitted when the pan changes.
    /*!
      This signal is emitted when loading a bookmark.
      \param point The new pan origin.
    */

    void panChanged(QPointF point);

    //! Emitted when the light incidence changes.
    /*!
      This signal is emitted when loading a bookmark.
      \param light The new light incidence.
      \param refresh Whether to refresh the image
    */

    void lightChanged(const vcg::Point3f& light, bool refresh);

    //! Emitted when the rendering mode changes.
    /*!
      This signal is emitted when loading a bookmark.
      \param mode The new rendering mode.
    */

    void renderingModeChanged(int mode);

    //! Emitted when the highlight box changes.
    /*!
      This signal is emitted when loading a bookmark.
      \param haveHighlightBox Whether there is a highlight box.
      \param box The highlight box. Undefined if haveHighlightBox is false.
    */

    void highlightBoxChanged(bool haveHighlightBox, QRectF box);

    /*====================================================================
     * Public slots
     *===================================================================*/

public slots:

    //! Add a highlight box to the bookmark.
    /*!
      \param box The highlight box.
    */

    void addHighlightBox(QRectF box);

    //! Add a new note to the bookmark.
    /*!
      \param subject The subject of the note
      \param author The author of the note
      \param timestamp The timestamp of the note
      \param comment The in-depth note
    */

    void addNote(QString subject, QString author, QDateTime timestamp, QString comment);

    /*====================================================================
     * Private slots
     *===================================================================*/

private slots:

    void setBookmark(int index);

    void addBookmark();

    void deleteBookmark();

    void addNote();

    void editNote();

    void editNote(QListWidgetItem * item);

    void editNote(int id, QString subject, QString author, QDateTime dateTime, QString comment);

    void deleteNote();

    void showToolTip(QListWidgetItem * item);

    void deleteHighlightBox();

    void updateLighting();

    void setButtonState();

};

#endif // BOOKMARKCONTROL_H
