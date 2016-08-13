/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Ying Yang (ying.yang.yy368@yale.edu)

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

 - Acknowledgments: This EXR file reader is based on the original code of
   exrreadchannel for MATLAB by Edgar Velazquez-Armendariz.

*****************************************************************************/
#ifndef RENDERCONTROLBASE_H
#define RENDERCONTROLBASE_H

#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <QMouseEvent>

/*====================================================================
 *
 * RenderControlGroup class header
 *
 *===================================================================*/

//! Controls for single rendering parameter.
/*!
  The RenderControlGroup class contains a label, a slider, and
  a spin box. These are used to control a single rendering parameter.
*/

class RenderControlGroup: public QObject
{

    Q_OBJECT

    // This class groups a label, a slider, and a spin box together.

    // We connect the slider and the spin box together so they will
    // update each other. We also connect the spin box with the signal
    // that updates the image.

    // We do not connect the slider with the signal that updates the
    // image because it would send too many signals to process easily.
    // Instead, we look for a mouse button release event on the left
    // or middle button, which indicates the user is done sliding.

public:
    /*====================================================================
     * Public methods
     *===================================================================*/

    //! Constructor
    /*!
      Constructs a RenderControlGroup.
      \param obj The rendering object
      \param name The parameter name
      \param initValue The initial value of the slider and spin box
      \param minimum The minimum of the slider and spin box
      \param maximum The maximum of the slider and spin box
    */

    RenderControlGroup(QObject * obj, const char *name, int initValue, int minimum = 0, int maximum = 100);

    /*====================================================================
     * Public data members
     *===================================================================*/

    QSlider * slider; /*!< The slider. */
    QSpinBox * spinBox; /*!< The spin box. */
    QLabel * label; /*!< The label. */

    /*====================================================================
     * Private methods
     *===================================================================*/

private:

    void initialize(QObject * obj, const char * name, int initValue, int minimum, int maximum);

    void initLabel(const char *name);

    void initSlider(QObject * obj, int initValue, int minimum, int maximum);

    void initSpinBox(int initValue, int minimum, int maximum);

    void connect();

    /*====================================================================
     * Private slots
     *===================================================================*/

private slots:

    void sliderChanged(int value);
};

/*====================================================================
 *
 * RenderControlUtils class header
 *
 *===================================================================*/

//! The RenderingControlGroup class handles the controls for a particular rendering.
/*!
  It contains one RenderControlGroup for each rendering parameter.
*/

class RenderControlUtils
{
public:

    /*====================================================================
     * Public methods
     *===================================================================*/

    /*!
      Constructor.
    */

    RenderControlUtils();

    //! Lays out the parameter controls
    /*!
      Lays out the controls for all of the parameters.
    */

    QLayout* createLayout();

    //! Get the index of the slider that received an event.
    /*!
      If a left or middle mouse button release event occurred,
      returns the index of the slider that received the event.
      If a different event occurred, returns -1.
    */

    int getSliderIndex(QObject * watched, QEvent * event);

    /*====================================================================
     * Public data members
     *===================================================================*/

    QList<RenderControlGroup *> groups;

};

#endif // RENDERCONTROLBASE_H
