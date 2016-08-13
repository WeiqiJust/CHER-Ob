/****************************************************************************
* RTIViewero o  *
* Single and Multi-View Reflectance Transformation Image Viewero  o*
* _O  _*
* Copyright    (C) 2008-2010\/)\/ *
* Visual Computing Lab - ISTI CNR                      /\/|*
* and                                            |*
* Cultural Heritage Imaging                              \*
*                                                                            *
* This program is free software: you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                        *
*                                                                            *
* This program is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU General Public License for more details.                                *
*                                                                            *
* You should have received a copy of the GNU General Public License            *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.        *
****************************************************************************/

#include "bookmarkcontrol.h"
#include "util.h"

#include <vcg/space/point3.h>

#include <QFile>
#include <QTextStream>
#include <QList>
#include <QListIterator>
#include <QMapIterator>

#include <QInputDialog>
#include <QVBoxLayout>
#include <QMessageBox>

#include <QDomText>
#include <QDomComment>
#include <QDomNodeList>

#include <QDebug>

/*====================================================================
 *
 * RenderingParam class
 *
 *===================================================================*/

RenderingParam::RenderingParam(QString s, float v) :
    name(s),
    value(v)
{
}

/*====================================================================
 *
 * Note class
 *
 *===================================================================*/

Note::Note()
{
}

Note::Note(int i, QString s, QString a, QDateTime t, QString c) :
    id(i),
    subject(s),
    author(a),
    timestamp(t),
    comment(c)
{
}

/*====================================================================
 *
 * Bookmark class
 *
 *===================================================================*/

Bookmark::Bookmark() :
    id(-1),
    hasHighlightBox(false),
    highlightBox(0.0, 0.0, 0.0, 0.0),
    params(),
    notes()
{
}

Bookmark::Bookmark(int id) :
    id(id),
    hasHighlightBox(false),
    highlightBox(0.0, 0.0, 0.0, 0.0),
    params(),
    notes()
{
}

/*====================================================================
 *
 * NoteDialog class
 *
 *===================================================================*/

/*====================================================================
 * Public methods
 *===================================================================*/

NoteDialog::NoteDialog(QWidget * parent, QString username, QString subject, QString comment, int i) :
    QDialog(parent),
    id(i)
{
    // Initialize the note dialog.

    QFormLayout * formLayout = initForm(subject, username, comment);
    QHBoxLayout * buttonLayout = initButtons();
    initLayout(formLayout, buttonLayout);
    setConnections();

    // Set the dialog to modal (blocking).

    setModal(true);
}

/*====================================================================
 * Private methods
 *===================================================================*/

QFormLayout * NoteDialog::initForm(QString subject, QString username, QString comment)
{
    // Create the fields.

    subjectEdit = new QLineEdit(subject);
    authorEdit = new QLineEdit(username);
    commentEdit = new QTextEdit(comment);
    commentEdit->setAcceptRichText(true);

    // Lay out the fields.

    QFormLayout * formLayout = new QFormLayout();
    formLayout->addRow("Subject:", subjectEdit);
    formLayout->addRow("Author:", authorEdit);
    formLayout->addRow("Comment:", commentEdit);

    // Return the form layout

    return formLayout;
}

QHBoxLayout * NoteDialog::initButtons()
{
    // Create the buttons.

    okButton = new QPushButton("OK");
    okButton->setDefault(true);
    cancelButton = new QPushButton("Cancel");

    // Lay out the buttons.

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addSpacing(100);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacing(100);

    // Return the button layout

    return buttonLayout;
}

void NoteDialog::initLayout(QFormLayout * formLayout, QHBoxLayout * buttonLayout)
{
    // Lay out the dialog.

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);
    setLayout(layout);
}

void NoteDialog::setConnections()
{
    // Connect the buttons to various slots. accept() and reject()
    // are defined by QDialog.

    connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

/*====================================================================
 * Private slots
 *===================================================================*/

void NoteDialog::okButtonClicked()
{
    // When the OK button is clicked, add or change the note,
    // depending on the note ID. (An ID of -1 means we are
    // creating a new note.)

    if (id == -1)
        emit noteCreated(subjectEdit->text(), authorEdit->text(), QDateTime::currentDateTime(), commentEdit->toHtml()/*commentEdit->toPlainText()*/);
    else
        emit noteEdited(id, subjectEdit->text(), authorEdit->text(), QDateTime::currentDateTime(), commentEdit->toHtml()/*commentEdit->toPlainText()*/);
}

/*====================================================================
 *
 * BookmarkControl class header
 *
 *===================================================================*/

/*====================================================================
 * Constructor
 *===================================================================*/

BookmarkControl::BookmarkControl(RtiViewerDlg * g, RtiBrowser * b, LightControl * l, RenderingDialog * r, QWidget *parent) :
    QFrame(parent),
    gui(g),
    browser(b),
    light(l),
    renderingDialog(r),
    bookmarks(),
    currentID(-1)
{
    setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    setFixedWidth(374);

    initSelector();
    initNotes();
    initHighlightBox();
    initLighting();
    initLayout();
    setConnections();
}

/*====================================================================
 * Methods for building the GUI
 *===================================================================*/

void BookmarkControl::initSelector()
{
    bmSelectorGroup = new QGroupBox("Bookmark");

    bmComboBox = new QComboBox();
    bmComboBox->setEnabled(false);
    bmComboBox->setFixedWidth(250);

    addBookmarkButton = new QPushButton();
    addBookmarkButton->setText("Add");
    addBookmarkButton->setMaximumWidth(30);
    addBookmarkButton->setEnabled(false);
    addBookmarkButton->setToolTip("Add a bookmark");

    deleteBookmarkButton = new QPushButton();
    deleteBookmarkButton->setText("Del");
    deleteBookmarkButton->setMaximumWidth(30);
    deleteBookmarkButton->setEnabled(false);
    deleteBookmarkButton->setToolTip("Delete the current bookmark");

    QHBoxLayout * bookmarkLayout = new QHBoxLayout();
    bookmarkLayout->addWidget(bmComboBox);
    bookmarkLayout->addWidget(addBookmarkButton);
    bookmarkLayout->addWidget(deleteBookmarkButton);

    bmSelectorGroup->setLayout(bookmarkLayout);
}

void BookmarkControl::initNotes()
{
    notesGroup = new QGroupBox("Notes");

    notesList = new QListWidget();
    notesList->setSelectionMode(QAbstractItemView::SingleSelection);
    notesList->setSelectionBehavior(QAbstractItemView::SelectItems);

    addNoteButton = new QPushButton();
    addNoteButton->setText("Add");
    addNoteButton->setMaximumWidth(30);
    addNoteButton->setEnabled(false);
    addNoteButton->setToolTip("Add a note to the current bookmark");

    editNoteButton = new QPushButton();
    editNoteButton->setText("Edit");
    editNoteButton->setMaximumWidth(30);
    editNoteButton->setEnabled(false);
    editNoteButton->setToolTip("Edit the selected note");

    deleteNoteButton = new QPushButton();
    deleteNoteButton->setText("Del");
    deleteNoteButton->setMaximumWidth(30);
    deleteNoteButton->setEnabled(false);
    deleteNoteButton->setToolTip("Delete the selected note");

    QVBoxLayout * buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(addNoteButton);
    buttonLayout->addWidget(editNoteButton);
    buttonLayout->addWidget(deleteNoteButton);

    QHBoxLayout * notesLayout = new QHBoxLayout();
    notesLayout->addWidget(notesList, 100);
    notesLayout->addLayout(buttonLayout);

    notesGroup->setLayout(notesLayout);
}

void BookmarkControl::initHighlightBox()
{
    highlightBoxGroup = new QGroupBox("Highlight Box");

    addBoxButton = new QPushButton();
    addBoxButton->setText("Add");
    addBoxButton->setMaximumWidth(30);
    addBoxButton->setEnabled(false);
    addBoxButton->setToolTip("Draw a highlight box on the image");

    editBoxButton = new QPushButton();
    editBoxButton->setText("Edit");
    editBoxButton->setMaximumWidth(30);
    editBoxButton->setEnabled(false);
    editBoxButton->setToolTip("Resize the highlight box on the image");

    deleteBoxButton = new QPushButton();
    deleteBoxButton->setText("Del");
    deleteBoxButton->setMaximumWidth(30);
    deleteBoxButton->setEnabled(false);
    deleteBoxButton->setToolTip("Remove the highlight box from the image");

    QHBoxLayout * highlightBoxLayout = new QHBoxLayout();
    highlightBoxLayout->addWidget(addBoxButton);
    highlightBoxLayout->addWidget(editBoxButton);
    highlightBoxLayout->addWidget(deleteBoxButton);

    highlightBoxGroup->setLayout(highlightBoxLayout);
}

void BookmarkControl::initLighting()
{
    lightingGroup = new QGroupBox("Light, Zoom, Pan, Rendering");

    updateLightingButton = new QPushButton();
    updateLightingButton->setText("Update Bookmark");
    updateLightingButton->setEnabled(false);
    updateLightingButton->setToolTip("<p>Change light, zoom, pan, and rendering in the bookmark to the current values.</p>");

    QHBoxLayout * lightingLayout = new QHBoxLayout();
    lightingLayout->addWidget(updateLightingButton);

    lightingGroup->setLayout(lightingLayout);
}

void BookmarkControl::initLayout()
{
    QHBoxLayout * bottomRowLayout = new QHBoxLayout();
    bottomRowLayout->addWidget(highlightBoxGroup);
    bottomRowLayout->addWidget(lightingGroup);

    QVBoxLayout * bookmarkLayout = new QVBoxLayout();
    bookmarkLayout->addWidget(bmSelectorGroup);
    bookmarkLayout->addWidget(notesGroup);
    bookmarkLayout->addLayout(bottomRowLayout);
    bookmarkLayout->setStretch(1, 100);

    setLayout(bookmarkLayout);
}

void BookmarkControl::setConnections()
{
    connect(bmComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBookmark(int)));
    connect(addBookmarkButton, SIGNAL(clicked()), this, SLOT(addBookmark()));
    connect(deleteBookmarkButton, SIGNAL(clicked()), this, SLOT(deleteBookmark()));
    connect(browser, SIGNAL(highlightBoxDrawn(QRectF)), this, SLOT(addHighlightBox(QRectF)));

    connect(this, SIGNAL(zoomChanged(int)), gui, SLOT(setZoomValue(int)));
    connect(this, SIGNAL(panChanged(QPointF)), browser, SLOT(setPan(QPointF)));
    connect(this, SIGNAL(lightChanged(const vcg::Point3f&, bool)), light, SLOT(setLight(const vcg::Point3f&, bool)));
    connect(this, SIGNAL(renderingModeChanged(int)), renderingDialog, SLOT(setRenderingMode(int)));
    connect(updateLightingButton, SIGNAL(clicked()), this, SLOT(updateLighting()));

    connect(addBoxButton, SIGNAL(clicked()), browser, SLOT(addHighlightBox()));
    connect(editBoxButton, SIGNAL(clicked()), browser, SLOT(editHighlightBox()));
    connect(deleteBoxButton, SIGNAL(clicked()), this, SLOT(deleteHighlightBox()));
    connect(deleteBoxButton, SIGNAL(clicked()), browser, SLOT(deleteHighlightBox()));
    connect(this, SIGNAL(highlightBoxChanged(bool, QRectF)), browser, SLOT(setHighlightBox(bool, QRectF)));

    connect(addNoteButton, SIGNAL(clicked()), this, SLOT(addNote()));
    connect(editNoteButton, SIGNAL(clicked()), this, SLOT(editNote()));
    connect(deleteNoteButton, SIGNAL(clicked()), this, SLOT(deleteNote()));
    connect(notesList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(editNote(QListWidgetItem *)));
    connect(notesList, SIGNAL(itemSelectionChanged()), this, SLOT(setButtonState()));
    connect(notesList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(showToolTip(QListWidgetItem*)));
}

/*====================================================================
 * Methods for working with the GUI
 *===================================================================*/

void BookmarkControl::populateBookmarkComboBox()
{
    // Clear out any current items in the combo box.

    bmComboBox->clear();

    // Add the "NONE" item and select it. The users uses this
    // to clear the current bookmark.

    bmComboBox->addItem("NONE", QVariant(-1));
    bmComboBox->setCurrentIndex(0);

    // Go through the list of bookmarks and add them to the combo box.

    QMapIterator<int, Bookmark> iterator(bookmarks);
    while (iterator.hasNext())
    {
        iterator.next();
        Bookmark bookmark = iterator.value();
        bmComboBox->addItem(bookmark.name, QVariant(bookmark.id));
    }

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::displayBookmark()
{
    if (currentID == -1)
    {
        // If the current bookmark is "NONE" (-1), set the lighting, zoom,
        // pan, and rendering to their original state. Note that because
        // we do not know the original zoom value -- it is less than 100%
        // if the image is too big to fit on the screen -- we use 0 and
        // let the browser clamp this to the minimum value.

        emit lightChanged(vcg::Point3f(0.0, 0.0, 1.0), false);
        emit zoomChanged(0);
        emit panChanged(QPointF(0.0, 0.0));
        emit highlightBoxChanged(false, QRectF(0.0, 0.0, 0.0, 0.0));
        emit renderingModeChanged(DEFAULT);
    }
    else
    {
        // Set the lighting, zoom, pan, and rendering for the bookmark.

        const Bookmark currentBookmark = bookmarks[currentID];
        emit lightChanged(getPoint3f(currentBookmark.incidence), false);
        emit zoomChanged(currentBookmark.zoom);
        emit panChanged(currentBookmark.pan);
        emit highlightBoxChanged(currentBookmark.hasHighlightBox, currentBookmark.highlightBox);
        emit renderingModeChanged(currentBookmark.renderingMode);
        displayRendering(currentBookmark);
    }

    // Display the notes, if any.

    displayNotes();
}

vcg::Point3f BookmarkControl::getPoint3f(QPointF point)
{
    // Given a point on the (x,y) plane, calculate the z coordinate
    // of a point on a unit sphere directly above the point on the plane.

    float z = sqrt(1 - point.x()*point.x() - point.y()*point.y());
    return vcg::Point3f(point.x(), point.y(), z);
}

void BookmarkControl::displayRendering(const Bookmark & bookmark)
{
    // Retrieve the rendering mode object used by the bookmark.

    RenderingMode* rm = browser->getRenderingModes()->value(bookmark.renderingMode);

    // Set the rendering parameters stored in the bookmark. Also set the sliders
    // and spin boxes for these controls.

    switch (bookmark.renderingMode)
    {
/*        case DEFAULT:

          // If we are using the default rendering, check if the image is
          // a multi-view image by trying to cast the RenderingMode to DefaultMRti.
          // If the cast succeeds, set the horizontal and vertical views.

          // OPEN ISSUES:
          //
          // a) The code has not been compiled and will not compile in its current state.
          //    In particular, it calls private methods to set the horizontal and vertical
          //    sliders and the check box.
          //
          // b) It is possible that the values stored in the bookmark file (which were directly
          //    retrieved from the sliders) use a different scale than setPosX() and setPosY().
          //    Please read the code in ViewpointControl and DefaultMRti (in multiviewrti.h and .cpp).
          //
          // c) There is an open design issue about whether this code should be here or
          //    somewhere else. Currently, the horizontal and vertical sliders are treated
          //    as rendering mode controls. The problem with this architecture is it does not
          //    allow multi-view RTI files to support any other rendering modes, such as diffuse
          //    gain, specular enhancement, or normals visualization. The reason is that it would
          //    not be possible to display the multi-view controls and the other rendering mode
          //    controls at the same time because this would require two rendering modes (multi-view
          //    and some other mode, such as specular enhancement, to be selected at the same
          //    time. An additional problem is that the controls for both rendering modes would
          //    occupy the same space on the GUI.
          //
          //    An alternate solution is to move the multi-view controls somewhere else on the
          //    GUI and somewhere else in the code. The obvious place is to make them parallel
          //    to controls such as the pan and zoom spinners, since they are controlling the
          //    user's viewpoint (like pan and zoom), not the way the image is rendered (like
          //    specular enhancement or normals visualization).
          //
          //    All of this is relevant to the bookmark and snapshot XMP files because it
          //    dictates where the multiview information is stored in the file, what tags are
          //    used, and where the information is processed in the code:
          //
          //    a) If multiview is considered another rendering mode, then the multiview
          //       information should be part of the rendering parameters in the bookmark file,
          //       use the rendering parameter tags, and be processed here.
          //
          //    b) If multiview is considered to be a way the user controls their viewpoint,
          //       then the multiview information should be stored parallel to Pan and Zoom,
          //       use tags like rti:HorizontalShift, rti:VerticalShift, and rti:NearestViewpoint,
          //       and be processed next to the code that processes those tags.
          //
          //    Personally, I feel (b) is the correct choice, even if multiview continues
          //    to use the rendering mode mechanism internally. However, this code is written
          //    to use choice (a). If (b) is used (and multiview continues to internally use
          //    the rendering mode mechanism), then the following changes need to be made:
          //
          //    a) Change the bookmark structure by adding fields for multiview (boolean),
          //       horizontal shift, vertical shift, and nearest viewpoint.
          //
          //    b) In displayBookmark(), call a method that contains the code in this
          //       case (the DEFAULT case). The method would first check if the multiview
          //       boolean was set.
          //
          //    c) In loadBookmarkFile(), add code to read the rti:HorizontalShift,
          //       rti:VerticalShift, and rti:NearestViewpoint tags and store these in
          //       the bookmark structure. (With the current code, this would happen
          //       in the existing code when rendering parameters are read.)
          //
          //    d) In saveBookmarkFile(), add code to store the values of the horizontal
          //       and vertical shifts and nearest viewpoint fields of the bookmark
          //       structure in the rti:HorizontalShift, rti:VerticalShift, and
          //       rti:NearestViewpoint tags. These tags should be siblings of the tags
          //       for zoom and pan.
          //
          //    e) In addBookmark(), pass the bookmark to a method (addShift()) that
          //       uses the code that is currently in the DEFAULT case of getRenderingMode().
          //       Modify the code to set the fields for horizontal and vertical shift
          //       and nearest viewpoint instead of setting rendering parameters.
          //
          //    f) In RtiBrowser.saveSnapshotMetadata(), move the code currently in the
          //       DEFAULT case of the switch statement that handles rendering modes to
          //       a place higher in the method. It should instead create rti:HorizontalShift,
          //       rti:VerticalShift, and rti:NearestViewpoint elements that are siblings
          //       of the elements for zoom and pan.

           {
               DefaultMRti* mrti = dynamic_cast<DefaultMRti*>(rm);
               if (mrti != 0)
               {
                   ViewpointControl * vc = (ViewpointControl*)renderingDialog->getRenderingControl();
                   for (int j=0; j < bookmark.params.count(); j++)
                   {
                       RenderingParam param = bookmark.params.at(j);
                       int value = roundParam(param.value);
                       if (param.name == QString("horizontalShift"))
                       {
                           mrti->setPosX(value);
                           vc->viewpointSlider->setValue(value); // WON'T COMPILE: viewpointSlider is private.
                       }
                       else if (param.name == QString("nearestViewpoint"))
                       {
                           // mrti->; // It does not appear that DefaultMRti cares about snapNearest. I believe this only affects viewpointSlider.
                           vc->snapNearest->setCheckState(value); // WON'T COMPILE: snapNearest is private.
                       }
                       else if (param.name == QString("verticalShift"))
                       {
                           mrti->setPosY(value);
                           vc->vertical->setValue(value); // WON'T COMPILE: vertical is private.
                       }
                   }
                }
            }
            break;
    }*/

        case DIFFUSE_GAIN:

            {
                DiffuseGain* dg = static_cast<DiffuseGain*> (rm);
                int gain = roundParam(bookmark.params.first().value);
                dg->setGain(gain);
                DiffuseGControl * dgc = (DiffuseGControl*)renderingDialog->getRenderingControl();
                dgc->groups.at(0)->spinBox->setValue(gain);
            }
            break;

        case SPECULAR_ENHANCEMENT:

            {
                SpecularEnhancement* se = static_cast<SpecularEnhancement*> (rm);
                SpecularEControl * sec = (SpecularEControl*)renderingDialog->getRenderingControl();
                for (int j=0; j < bookmark.params.count(); j++)
                {
                    RenderingParam param = bookmark.params.at(j);
                    int value = roundParam(param.value);
                    if (param.name == QString("diffuseColor"))
                    {
                        se->setKd(value);
                        sec->groups.at(0)->spinBox->setValue(value);
                    }
                    else if (param.name == QString("specularity"))
                    {
                        se->setKs(value);
                        sec->groups.at(1)->spinBox->setValue(value);
                    }
                    else if (param.name == QString("highlightSize"))
                    {
                        se->setExp(value);
                        sec->groups.at(2)->spinBox->setValue(value);
                    }
                }
            }
            break;

        case NORMAL_ENHANCEMENT:

            {
                NormalEnhancement* ne = static_cast<NormalEnhancement*> (rm);
                NormalEControl * nec = (NormalEControl*)renderingDialog->getRenderingControl();
                for (int j=0; j < bookmark.params.count(); j++)
                {
                    RenderingParam param = bookmark.params.at(j);
                    int value = roundParam(param.value);
                    if (param.name == QString("gain"))
                    {
                        ne->setGain(value);
                        nec->groups.at(0)->spinBox->setValue(value);
                    }
//                    else if (param.name == QString("kd"))
//                        ne->setKd((int)param.value);
                    else if (param.name == QString("environment"))
                    {
                        ne->setEnvIll(value);
                        nec->groups.at(1)->spinBox->setValue(value);
                    }
                }
            }
            break;

        case UNSHARP_MASKING_IMG:

            {
                UnsharpMasking* um_img = static_cast<UnsharpMasking*> (rm);
                int gain = roundParam(bookmark.params.first().value);
                um_img->setGain(gain);
                UnsharpMControl * um_img_c = (UnsharpMControl*)renderingDialog->getRenderingControl();
                um_img_c->groups.at(0)->spinBox->setValue(gain);
            }
            break;

        case UNSHARP_MASKING_LUM:

            {
                UnsharpMasking* um_lum = static_cast<UnsharpMasking*> (rm);
                int gain = roundParam(bookmark.params.first().value);
                um_lum->setGain(gain);
                UnsharpMControl * um_lum_c = (UnsharpMControl*)renderingDialog->getRenderingControl();
                um_lum_c->groups.at(0)->spinBox->setValue(gain);
            }
            break;

        case COEFF_ENHANCEMENT:

            {
                CoeffEnhancement* ce = static_cast<CoeffEnhancement*> (rm);
                int gain = roundParam(bookmark.params.first().value);
                ce->setGain(gain);
                CoeffEnhancControl * cec = (CoeffEnhancControl*)renderingDialog->getRenderingControl();
                cec->groups.at(0)->spinBox->setValue(gain);

            }
            break;

        case DETAIL_ENHANCEMENT:

            {
                DetailEnhancement* de = static_cast<DetailEnhancement*> (rm);

                OffsetNum o;
                TileSize size;
                int level;
                SharpnessMeasures m;
                SphereSampling ss;
                float k1;
                float k2;
                float t;
                SmoothingFilter f;
                int nIter;

                for (int j=0; j < bookmark.params.count(); j++)
                {
                    RenderingParam param = bookmark.params.at(j);
                    if (param.name == QString("localOffset"))
                        o = (OffsetNum)((int)param.value);
                    else if (param.name == QString("tileSize"))
                        size = (TileSize)((int)param.value);
                    else if (param.name == QString("numberInitialTiles"))
                        level = param.value;
                    else if (param.name == QString("sharpnessOperator"))
                        m = (SharpnessMeasures)((int)param.value);
                    else if (param.name == QString("lightSampling"))
                        ss = (SphereSampling)((int)param.value);
                    else if (param.name == QString("k1Sharpness"))
                        k1 = param.value;
                    else if (param.name == QString("k2Lightness"))
                        k2 = param.value;
                    else if (param.name == QString("threshold"))
                        t = param.value;
                    else if (param.name == QString("smoothingFilter"))
                        f = (SmoothingFilter)((int)param.value);
                    else if (param.name == QString("numberIterationSmoothing"))
                        nIter = param.value;
                }
                de->updateConfig(o, size, level, m, ss, k1, k2, t, f, nIter);
            }
            break;

        case DYN_DETAIL_ENHANCEMENT:

            {
                DynamicDetailEnh* dde = static_cast<DynamicDetailEnh*> (rm);

                int o;
                int size;
                SharpnessMeasuresDyn m;
                SphereSamplingDyn ss;
                float k1;
                float k2;
                float t;
                SmoothingFilterDyn f;
                int nIter;

                for (int j=0; j < bookmark.params.count(); j++)
                {
                    RenderingParam param = bookmark.params.at(j);
                    if (param.name == QString("degreeOffset"))
                        o = (int)param.value;
                    else if (param.name == QString("tileSize"))
                        size = (int)param.value;
                    else if (param.name == QString("sharpnessOperator"))
                        m = (SharpnessMeasuresDyn)((int)param.value);
                    else if (param.name == QString("lightSampling"))
                        ss = (SphereSamplingDyn)((int)param.value);
                    else if (param.name == QString("k1Sharpness"))
                        k1 = param.value;
                    else if (param.name == QString("k2Lightness"))
                        k2 = param.value;
                    else if (param.name == QString("threshold"))
                        t = param.value;
                    else if (param.name == QString("smoothingFilter"))
                        f = (SmoothingFilterDyn)((int)param.value);
                    else if (param.name == QString("numberIterationSmoothing"))
                        nIter = param.value;
                }
                dde->setOffset(o);
                dde->setTileSize(size);
                dde->updateConfig(m, ss, k1, k2, t,f, nIter);
            }
            break;

        default:
            break;
    }
}

void BookmarkControl::displayNotes()
{
    // Clear the current list of notes.

    notesList->clear();

    // If the bookmark is "NONE", just return.

    if (currentID == -1) return;

    // Go through the notes and add them to the list
    // of notes. We display the entire note as
    // a tool tip and store the note ID as user data.

    QMapIterator<int, Note> iterator(bookmarks[currentID].notes);
    while (iterator.hasNext())
    {
        iterator.next();
        Note note = iterator.value();

        QListWidgetItem * item = new QListWidgetItem(note.subject);
        QString toolTip = QString("<p>")
                            .append(note.subject)
                            .append("<br />")
                            .append(note.author)
                            .append("<br />")
                            .append(note.timestamp.toString())
                            .append("<br />")
                            .append("<br />")
                            .append(note.comment)
                            .append("</p>");
        item->setData(Qt::ToolTipRole, toolTip);
        item->setData(Qt::UserRole, note.id);
        notesList->addItem(item);
    }
}

/*====================================================================
 * Methods for managing bookmarks
 *===================================================================*/

void BookmarkControl::loadBookmarks(const QFileInfo & info)
{
    // Set the current bookmark to the empty bookmark.

    currentID = -1;

    // Load the bookmark file and populate the bookmark combo box.

    setPath(info);
    loadBookmarkFile();
    populateBookmarkComboBox();

    // Enable the combo box. The combo box is disabled only
    // when the program starts and no image is loaded.

    bmComboBox->setEnabled(true);

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::setPath(const QFileInfo & info)
{
    // Set the path to the bookmark file. The bookmark file name is:
    //
    //    Path to image file + image file name + "_" + image file extension + ".xmp"
    //
    // We use the image file extension in the bookmark file name because there
    // might be two or more image files with the same name and different
    // extensions, such as an .hsh, a .ptm, and an .rti.

    path = info.canonicalPath() + "/" + info.completeBaseName() + "_" + info.suffix() + ".xmp";
}

bool BookmarkControl::loadBookmarkFile()
{
    // Load the bookmarks from the bookmark file, if it exists.

    // This code assumes that the bookmark file is a valid XML
    // document. Results are undefined if the bookmark file is not valid.

    // Clear the existing bookmarks.

    bookmarks.clear();

    // Create a QFile and check if the file exists. If the file does
    // not exist, we just return -- there is nothing to load.

    QFile bookmarkFile(path);
    if (!bookmarkFile.exists()) return false;

    // Open the bookmark file and parse it. If an error occurs, display
    // a message and return false.

    QDomDocument xmp;
    QString errorStr;
    int errorLine, errorColumn;
    if (!xmp.setContent(&bookmarkFile, true, &errorStr, &errorLine, &errorColumn))
    {
        QMessageBox::information(window(),
                                 tr("Error opening bookmark file"),
                                 tr("Parse error in %1 at\n line %2, column %3:\n%4")
                                    .arg(path)
                                    .arg(errorLine)
                                    .arg(errorColumn)
                                    .arg(errorStr));
        return false;
    }

    // Get the Bookmark elements and build Bookmark objects.

    // Note that firstChildElement is not namespace-aware. It
    // returns all elements in any namespace that match the given
    // local name. Because a valid bookmark document uses local
    // names in the RTI namespace that are not in the RDF
    // namespace, we can safely avoid checking the namespace.

    QDomNodeList bmElems = xmp.elementsByTagNameNS(rtiURI, "Bookmark");
    for (int i = 0; i < bmElems.count(); i++)
    {
        Bookmark bm;

        // Set the bookmark id and name.

        QDomElement bmElem = bmElems.item(i).toElement();
        bm.id = bmElem.firstChildElement("ID").text().toInt();
        bm.name = bmElem.firstChildElement("Name").text();
        bm.rtiViewerID = bmElem.firstChildElement("CreatorTool").text();

        // Get the RenderingInfo element

        QDomElement infoElem = bmElem.firstChildElement("RenderingInfo");

        // Set the zoom

        bm.zoom = infoElem.firstChildElement("Zoom").text().toInt();

        // Set the pan

        QDomElement panElem = infoElem.firstChildElement("Pan");
        float pan_x = panElem.firstChildElement("x").text().toFloat(),
              pan_y = panElem.firstChildElement("y").text().toFloat();
        bm.pan = QPointF(pan_x, pan_y);

        // Set the incidence of the light

        QDomElement incidenceElem = infoElem.firstChildElement("Incidence");
        bm.incidence = QPointF(incidenceElem.firstChildElement("x").text().toFloat(),
                               incidenceElem.firstChildElement("y").text().toFloat());

        // Set the rendering mode and any parameters

        QDomElement renderingModeElem = infoElem.firstChildElement("RenderingMode");
        bm.renderingMode = renderingModeElem.firstChildElement("RenderingModeID").text().toInt();

        QDomNodeList paramElems = renderingModeElem.elementsByTagNameNS(rtiURI, "Parameter");
        for (int j = 0; j < paramElems.count(); j++)
        {
            QDomElement paramElem = paramElems.at(j).toElement();
            RenderingParam param = RenderingParam(paramElem.attribute("name"),
                                                  paramElem.attribute("value").toFloat());
            bm.params.append(param);
        }

        // Set the highlight box (if any)

        QDomElement highlightBoxElem = bmElem.firstChildElement("HighlightBox");
        if (!highlightBoxElem.isNull())
        {
            QDomElement tlElem = highlightBoxElem.firstChildElement("TopLeft");
            QDomElement brElem = highlightBoxElem.firstChildElement("BottomRight");
            float tl_x = tlElem.firstChildElement("x").text().toFloat(),
                  tl_y = tlElem.firstChildElement("y").text().toFloat(),
                  br_x = brElem.firstChildElement("x").text().toFloat(),
                  br_y = brElem.firstChildElement("y").text().toFloat();
            bm.highlightBox = QRectF(QPointF(tl_x, tl_y), QPointF(br_x, br_y));
            bm.hasHighlightBox = true;
        }
        else
        {
            bm.hasHighlightBox = false;
        }

        // Set the notes (if any).

        bm.notes = QMap<int, Note>();
        QDomNodeList noteElems = bmElem.elementsByTagNameNS(rtiURI, "Note");
        for (int k = 0; k < noteElems.count(); k++)
        {
            QDomElement noteElem = noteElems.at(k).toElement();
            Note note;
            note.id = noteElem.firstChildElement("ID").text().toInt();
//            note.id = noteElem.attribute("id").toInt();
            note.subject = noteElem.firstChildElement("Subject").text();
            note.author = noteElem.firstChildElement("Author").text();
            note.timestamp = QDateTime::fromString(noteElem.firstChildElement("TimeStamp").text(), "yyyy-MM-ddThh:mm:ss");
            note.comment = noteElem.firstChildElement("Comment").text();
            bm.notes.insert(note.id, note);
        }

        // Insert the bookmark into the bookmarks map.

        bookmarks.insert(bm.id, bm);
    }

    return true;
}

void BookmarkControl::saveBookmarkFile()
{
    // This function saves the bookmarks to a file.

    // If there are no bookmarks, just return.

    if (bookmarks.count() == 0) return;

    // Create a new DOM document.

    QDomDocument xmp;

    // Add the XML declaration

    QDomProcessingInstruction header = xmp.createProcessingInstruction( "xml", "version=\"1.0\"" );
    xmp.appendChild(header);

    // Create the rdf:RDF element.
    //
    // Note that QDomDocument writes namespace declarations on every element
    // that uses a namespace prefix or has an attribute that uses a namespace
    // prefix. This leads to an unnecessarily verbose XML document. As a workaround,
    // we declare the namespaces we need on the root element and use
    // QDomDocument.createElement instead of QDomDocument.createElementNS. Yech.

    QDomElement rdf = createChild(xmp, xmp, "rdf:RDF");
    rdf.setAttribute("xmlns:rdf", rdfURI);
    rdf.setAttribute("xmlns:rti", rtiURI);
    rdf.setAttribute("xmlns:xmp", xmpURI);

    // Add the rdf:Description child

    QDomElement description = createChild(xmp, rdf, "rdf:Description");
    description.setAttribute("rdf:about", "");

    // Add the rti:Bookmarks and rdf:Bag children

    QDomElement bookmarksElem = createChild(xmp, description, "rti:Bookmarks");
    QDomElement bag = createChild(xmp, bookmarksElem, "rdf:Bag");

    // Loop through the bookmarks and create rti:Bookmark elements

    QMapIterator<int, Bookmark> i(bookmarks);
    while (i.hasNext())
    {
        // Get the next bookmark

        Bookmark bm = i.next().value();
        if (bm.id == -1) continue;

        // Create the rdf:li, rti:Bookmark, rti:ID, rti:Name, and xmp:CreatorTool children.

        QDomElement li = createChild(xmp, bag, "rdf:li");

        QDomElement bookmark = createChild(xmp, li, "rti:Bookmark");
        bookmark.setAttribute("rdf:parseType", "Resource");
        createChild(xmp, bookmark, "rti:ID", QString::number(bm.id));
        createChild(xmp, bookmark, "rti:Name", bm.name);
        createChild(xmp, bookmark, "xmp:CreatorTool", bm.rtiViewerID);


        // Create the rti:RenderingInfo child.

        QDomElement info = createChild(xmp, bookmark, "rti:RenderingInfo");
        info.setAttribute("rdf:parseType", "Resource");

        // Create the rti:Zoom, rti:Pan, and rti:Incidence children.

        createChild(xmp, info, "rti:Zoom", QString::number(bm.zoom));

        QDomElement pan = createChild(xmp, info, "rti:Pan");
        pan.setAttribute("rdf:parseType", "Resource");
        createChild(xmp, pan, "rti:x", QString::number(bm.pan.x()));
        createChild(xmp, pan, "rti:y", QString::number(bm.pan.y()));

        QDomElement incidence = createChild(xmp, info, "rti:Incidence");
        incidence.setAttribute("rdf:parseType", "Resource");
        createChild(xmp, incidence, "rti:x", QString::number(bm.incidence.x()));
        createChild(xmp, incidence, "rti:y", QString::number(bm.incidence.y()));

        // Create the rti:RenderingMode child and rti:Parameters child (if any)

        QDomElement renderingMode = createChild(xmp, info, "rti:RenderingMode");
        renderingMode.setAttribute("rdf:parseType", "Resource");
        QDomComment renderingModeComment = xmp.createComment(
            "0=DEFAULT,\n1=DIFFUSE_GAIN,\n2=SPECULAR_ENHANCEMENT,\n3=NORMAL_ENHANCEMENT,\n4=UNSHARP_MASKING_IMG,\n5=UNSHARP_MASKING_LUM,\n6=COEFF_ENHANCEMENT,\n7=DETAIL_ENHANCEMENT,\n8=DYN_DETAIL_ENHANCEMENT\n9=NORMALS_VISUALIZATION");
        renderingMode.appendChild(renderingModeComment);

        createChild(xmp, renderingMode, "rti:RenderingModeID", QString::number(bm.renderingMode));

        if (bm.params.count())
        {
            QDomElement parameters = createChild(xmp, renderingMode, "rti:Parameters");
            QDomElement parametersBag = createChild(xmp, parameters, "rdf:Bag");

            QListIterator<RenderingParam> j(bm.params);
            while (j.hasNext())
            {
                RenderingParam p = j.next();
                QDomElement parameterLi = createChild(xmp, parametersBag, "rdf:li");
                QDomElement param = createChild(xmp, parameterLi, "rti:Parameter");
                param.setAttribute("name", p.name);
                param.setAttribute("value", p.value);
            }
        }

        // If there is a highlight box, create the rti:HighlightBox child.

        if (bm.hasHighlightBox)
        {
            QDomElement highlightBox = createChild(xmp, bookmark, "rti:HighlightBox");
            highlightBox.setAttribute("rdf:parseType", "Resource");

            QDomElement topLeft = createChild(xmp, highlightBox, "rti:TopLeft");
            topLeft.setAttribute("rdf:parseType", "Resource");
            createChild(xmp, topLeft, "rti:x", QString::number(bm.highlightBox.topLeft().x()));
            createChild(xmp, topLeft, "rti:y", QString::number(bm.highlightBox.topLeft().y()));

            QDomElement bottomRight = createChild(xmp, highlightBox, "rti:BottomRight");
            bottomRight.setAttribute("rdf:parseType", "Resource");
            createChild(xmp, bottomRight, "rti:x", QString::number(bm.highlightBox.bottomRight().x()));
            createChild(xmp, bottomRight, "rti:y", QString::number(bm.highlightBox.bottomRight().y()));
        }

        // If there are any notes, create the rti:Notes child.

        if (bm.notes.count())
        {
            QDomElement notesElem = createChild(xmp, bookmark, "rti:Notes");
            QDomElement notesBag = createChild(xmp, notesElem, "rdf:Bag");

            QMapIterator<int, Note> k(bm.notes);
            while (k.hasNext())
            {
                Note n = k.next().value();

                QDomElement noteLi = createChild(xmp, notesBag, "rdf:li");

                QDomElement note = createChild(xmp, noteLi, "rti:Note");

                createChild(xmp, note, "rti:ID", QString::number(n.id));
                createChild(xmp, note, "rti:Subject", n.subject);
                createChild(xmp, note, "rti:Author", n.author);
                createChild(xmp, note, "rti:TimeStamp", n.timestamp.toString("yyyy-MM-ddThh:mm:ss"));
                createChild(xmp, note, "rti:Comment", n.comment);
            }
        }
    }

    // Open the bookmark file, save the DOM document, and close the file.

    QFile bookmarkFile(path);
    bookmarkFile.open(QIODevice::WriteOnly);
    QTextStream out(&bookmarkFile);
    xmp.save(out, 2);
    bookmarkFile.close();
}

int BookmarkControl::getNextID(QList<int> keys)
{
    // This method accepts a list of integer keys in
    // ascending order. If the list is non-empty, it
    // gets the last key and returns its value plus 1.
    // Otherwise, it returns 1.

    if (keys.count())
        return keys.last() + 1;
    else
        return 1;
}

void BookmarkControl::getRenderingMode(Bookmark & bookmark)
{
    // Get the current rendering mode from the browser and
    // store it in the bookmark.

    bookmark.renderingMode = browser->getCurrentRendering();

    // Clear the existing rendering parameters (if any), then
    // get the parameters for the current rendering mode
    // and store them in the bookmark.

    bookmark.params.clear();
    switch (bookmark.renderingMode)
    {
/*
        case DEFAULT:

        {
            // If we are using the default rendering, check if the image is a multi-
            // view image by trying to cast the RenderingMode to DefaultMRti. If the
            // cast succeeds, get the horizontal and vertical views.

            // For more information about compiling this code and other issues, see
            // the comments in displayRendering().

            DefaultMRti* mrti = static_cast<DefaultMRti*> (browser->getRenderingModes()->value(bookmark.renderingMode));
            if (mrti != 0)
            {
                bookmark.params.append(RenderingParam("horizontalShift", mrti->getCurrentPositionX())); // WON'T COMPILE: getCurrentPositionX() does not exist
                bookmark.params.append(RenderingParam("nearestViewpoint", mrti->getUseFlow())); // WON'T COMPILE: getUseFlow() does not exist
                bookmark.params.append(RenderingParam("verticalShift", mrti->getCurrentPositionY())); // WON'T COMPILE: getCurrentPositionX() does not exist
            }
        }
        break;
        */

        case DIFFUSE_GAIN:

            {
                DiffuseGain* dg = static_cast<DiffuseGain*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("gain", dg->getGain()));
            }
            break;

        case SPECULAR_ENHANCEMENT:

            {
                SpecularEnhancement* se = static_cast<SpecularEnhancement*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("diffuseColor", se->getKd()));
                bookmark.params.append(RenderingParam("specularity", se->getKs()));
                bookmark.params.append(RenderingParam("highlightSize", se->getExp()));
            }
            break;

        case NORMAL_ENHANCEMENT:

            {
                NormalEnhancement* ne = static_cast<NormalEnhancement*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("gain", ne->getGain()));
//                bookmark.params.append(RenderingParam("kd", ne->getKd()));
                bookmark.params.append(RenderingParam("environment", ne->getEnvIll()));
            }
            break;

        case UNSHARP_MASKING_IMG:

            {
                UnsharpMasking* um_img = static_cast<UnsharpMasking*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("gain", um_img->getGain()));
            }
            break;

        case UNSHARP_MASKING_LUM:

            {
                UnsharpMasking* um_lum = static_cast<UnsharpMasking*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("gain", um_lum->getGain()));
            }
            break;

        case COEFF_ENHANCEMENT:

            {
                CoeffEnhancement* ce = static_cast<CoeffEnhancement*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("gain", ce->getGain()));
            }
            break;

        case DETAIL_ENHANCEMENT:

            {
                DetailEnhancement* de = static_cast<DetailEnhancement*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("localOffset", (float)de->getNOffset()));
                bookmark.params.append(RenderingParam("tileSize", (float)de->getMinTileSize()));
                bookmark.params.append(RenderingParam("numberInitialTiles", (float)de->getMinLevel()));
                bookmark.params.append(RenderingParam("sharpnessOperator", (float)de->getSharpnessOperator()));
                bookmark.params.append(RenderingParam("lightSampling", (float)de->getSphereSampling()));
                bookmark.params.append(RenderingParam("k1Sharpness", de->getK1()));
                bookmark.params.append(RenderingParam("k2Lightness", de->getK2()));
                bookmark.params.append(RenderingParam("threshold", de->getThreshold()));
                bookmark.params.append(RenderingParam("smoothingFilter", (float)de->getFilter()));
                bookmark.params.append(RenderingParam("numberIterationSmoothing", (float)de->getNIterSmoothing()));
            }
            break;

        case DYN_DETAIL_ENHANCEMENT:

            {
                DynamicDetailEnh* dde = static_cast<DynamicDetailEnh*> (browser->getRenderingModes()->value(bookmark.renderingMode));
                bookmark.params.append(RenderingParam("degreeOffset", (float)dde->getDegreeOffset()));
                bookmark.params.append(RenderingParam("tileSize", (float)dde->getTileSize()));
                bookmark.params.append(RenderingParam("sharpnessOperator", (float)dde->getSharpnessOperator()));
                bookmark.params.append(RenderingParam("lightSampling", (float)dde->getSphereSampling()));
                bookmark.params.append(RenderingParam("k1Sharpness", dde->getK1()));
                bookmark.params.append(RenderingParam("k2Lightness", dde->getK2()));
                bookmark.params.append(RenderingParam("threshold", dde->getThreshold()));
                bookmark.params.append(RenderingParam("smoothingFilter", (float)dde->getFilter()));
                bookmark.params.append(RenderingParam("numberIterationSmoothing", (float)dde->getNIterSmoothing()));
            }
            break;

        default:

            break;
    }
}

/*====================================================================
 * Public slots
 *===================================================================*/

void BookmarkControl::addHighlightBox(QRectF box)
{
    // Add the highlight box (sent by the browser) to the current bookmark.

    Bookmark & currentBookmark = bookmarks[currentID];
    currentBookmark.hasHighlightBox = true;
    currentBookmark.highlightBox = box;

    // Save the bookmark file.

    saveBookmarkFile();

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::addNote(QString subject, QString author, QDateTime timestamp, QString comment)
{
    // This method creates a new Note object and
    // adds it to the current bookmark object.

    // Get the current bookmark and get the next note ID.

    Bookmark & currentBookmark = bookmarks[currentID];
    int id = getNextID(currentBookmark.notes.keys());

    // Create a new note and add it to the current bookmark.

    Note note(id, subject, author, timestamp, comment);
    currentBookmark.notes.insert(id, note);
}

/*====================================================================
 * Private slots
 *===================================================================*/

void BookmarkControl::setBookmark(int index)
{
    // This slot is called when a bookmark is selected in
    // the bookmark combo box.

    // Set the ID of the current bookmark.

    // If the combo box index is -1, the combo box is empty
    // or no item is selected. If it is 0, the "NONE" item
    // is selected. In both cases, set the current bookmark
    // ID to -1, which means no bookmark is selected.
    //
    // Otherwise, get the bookmark ID from the data associated
    // with the selected item.

    currentID = (index > 0) ? bmComboBox->itemData(index).toInt() : -1;

    // Display the bookmark.

    displayBookmark();

    // Set the state of the bookmark buttons.

    setButtonState();
}


void BookmarkControl::addBookmark()
{
    // This methods adds a new bookmark.

    // Get the bookmark name.

    QString name;
    bool haveName = false;

    while (!haveName)
    {
        // Prompt the user for the bookmark name.

        bool ok;
        name = QInputDialog::getText(this,
                                     tr("Add bookmark"),
                                     tr("Bookmark name:"),
                                     QLineEdit::Normal,
                                     "",
                                     &ok);

        // Return if the user clicks Cancel or doesn't enter a name.

        if (!ok || name.isEmpty()) return;

        // Check that the name is not a duplicate.

        haveName = true;
        for (int i = 0; i < bookmarks.count(); i++)
        {
            if (name.compare(bookmarks[i].name) == 0)
            {
                QMessageBox msg(QMessageBox::Warning,
                                "Duplicate bookmark name",
                                "A bookmark with the name " + name + " already exists.",
                                QMessageBox::Ok);
                msg.exec();
                haveName = false;
                break;
            }
        }
    }

    // Create the bookmark object and set the fields.

    Bookmark bookmark(getNextID(bookmarks.keys()));
    bookmark.name = name;
    bookmark.rtiViewerID = rtiViewerURL;
    bookmark.hasHighlightBox = false;
    bookmark.zoom = gui->getZoom();
    bookmark.pan = browser->getSubimage().topLeft();
    vcg::Point3f l = light->getLight();
    bookmark.incidence = QPointF(l.X(), l.Y());
    getRenderingMode(bookmark);

    // Add the bookmark to the list of bookmarks.

    bookmarks.insert(bookmark.id, bookmark);

    // Add the bookmark to the bookmark control and set it to the current item.

    bmComboBox->addItem(bookmark.name, QVariant(bookmark.id));
    int index = bmComboBox->findData(QVariant(bookmark.id));
    bmComboBox->setCurrentIndex(index);

    // Save the bookmark file.

    saveBookmarkFile();

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::deleteBookmark()
{
    // This method deletes the currently selected bookmark.

    // Get the index of the currently selected bookmark. If the
    // index is 0, just return -- you can't delete the "NONE"
    // bookmark. Otherwise, get the bookmark's ID.

    int index = bmComboBox->currentIndex();
    if (index == 0) return;
    int id = bmComboBox->itemData(index).toInt();

    // Set the index to 0 (NONE) and remove the item from the combo box.

    bmComboBox->setCurrentIndex(0);
    bmComboBox->removeItem(index);

    // Remove the bookmark from the list of bookmarks.

    bookmarks.remove(id);

    // If there are any bookmarks left, save the bookmarkfile.
    // Otherwise, delete it.

    if (bookmarks.count())
    {
        saveBookmarkFile();
    }
    else
    {
        QFile bookmarkFile(path);
        bookmarkFile.remove();
    }

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::addNote()
{
    // This method creates a new note.

    // Get the user name from the environment.

    QString username = getenv("USERNAME"); // For Windows
    if (username.isNull()) username = getenv("USER"); // For Mac or Linux

    // Create a new Note dialog, connect it to the correct
    // slot, and display the dialog.

    NoteDialog * noteDialog = new NoteDialog(this, username);
    connect(noteDialog, SIGNAL(noteCreated(QString, QString, QDateTime, QString)), this, SLOT(addNote(QString, QString, QDateTime, QString)));
    noteDialog->exec();
    delete noteDialog;

    // Save the bookmark file.

    saveBookmarkFile();

    // Display the notes.

    displayNotes();

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::editNote()
{
    // This slot is called when the Edit button is clicked. Get
    // the currently selected note and pass the call to
    // another slot.

    editNote(notesList->currentItem());
}

void BookmarkControl::editNote(QListWidgetItem * item)
{
    // This slot changes an note.

    // Get the note ID from the data stored with the
    // item, then get the note.

    int id = item->data(Qt::UserRole).toInt();
    Note note = bookmarks[currentID].notes[id];

    // Create a new annoation dialog with values from the selected
    // note, connect it to the correct slot, and display the dialog.

    NoteDialog * noteDialog = new NoteDialog(this, note.author, note.subject, note.comment, note.id);
    connect(noteDialog, SIGNAL(noteEdited(int, QString, QString, QDateTime, QString)), this, SLOT(editNote(int, QString, QString, QDateTime, QString)));
    noteDialog->exec();
    delete noteDialog;

    // Save the bookmark file.

    saveBookmarkFile();

    // Display the notes.

    displayNotes();

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::editNote(int id, QString subject, QString author, QDateTime dateTime, QString comment)
{
    // This method changes an note object.

    // Get the specified note object.

    Note & note = bookmarks[currentID].notes[id];

    // Change the fields in the note object.

    note.subject = subject;
    note.author = author;
    note.timestamp = dateTime;
    note.comment = comment;
}


void BookmarkControl::deleteNote()
{
    // This method deletes an note.

    // Get the selected note item, get the note ID
    // from the item, and delete the note.

    QListWidgetItem * item = notesList->currentItem();
    int id = item->data(Qt::UserRole).toInt();
    bookmarks[currentID].notes.remove(id);

    // Save the bookmark file.

    saveBookmarkFile();

    // Display the notes.

    displayNotes();

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::showToolTip(QListWidgetItem * item)
{
    // When the user clicks on a note, display the tooltip (which
    // contains the entire note -- subject, author, date, comment).

    QToolTip::showText(QCursor::pos(), item->toolTip());
}

void BookmarkControl::deleteHighlightBox()
{
    // Remove the highlight box from the current bookmark.

    bookmarks[currentID].hasHighlightBox = false;

    // Save the bookmark file.

    saveBookmarkFile();

    // Set the state of the bookmark buttons.

    setButtonState();
}

void BookmarkControl::updateLighting()
{
    // This method changes the lighting, zoom, pan, and
    // rendering in the bookmark to the current values.

    // Get the current bookmark.

    Bookmark & currentBookmark = bookmarks[currentID];

    // Update the lighting, zoom, pan, and rendering values.

    currentBookmark.zoom = gui->getZoom();
    vcg::Point3f l = light->getLight();
    currentBookmark.incidence = QPointF(l.X(), l.Y());
    currentBookmark.pan = browser->getSubimage().topLeft();
    getRenderingMode(currentBookmark);

    // Save the bookmark file.

    saveBookmarkFile();
}

void BookmarkControl::setButtonState()
{
    // Always enable the Add Bookmark button. It is disabled
    // only when the program starts and no image is loaded.

    addBookmarkButton->setEnabled(true);

    // If a bookmark is selected, enable the buttons to delete
    // a bookmark, update the lighting, and add a note. Note
    // that when the bookmark ID is -1, no bookmark is selected
    // or the NONE bookmark is selected.

    bool bookmarkSelected = (currentID != -1);

    deleteBookmarkButton->setEnabled(bookmarkSelected);
    updateLightingButton->setEnabled(bookmarkSelected);
    addNoteButton->setEnabled(bookmarkSelected);

    // If a bookmark is selected, enable/disable the highlight box buttons:
    //
    //                    Button
    //    Box? |  Add      Edit     Delete
    //    -------------------------------
    //    Yes  |  Disable  Enable   Enable
    //    No   |  Enable   Disable  Disable
    //
    // If no bookmark is selected, disable all three buttons.

    bool hasHighlightBox = (bookmarkSelected) ? bookmarks[currentID].hasHighlightBox : false;
    addBoxButton->setEnabled(bookmarkSelected && !hasHighlightBox);
    editBoxButton->setEnabled(hasHighlightBox);
    deleteBoxButton->setEnabled(hasHighlightBox);

    // If an note is selected, enable the Edit and Delete
    // Note buttons. Otherwise, disable these buttons.

    bool noteSelected = (notesList->selectedItems().count() > 0);
    editNoteButton->setEnabled(noteSelected);
    deleteNoteButton->setEnabled(noteSelected);
}
