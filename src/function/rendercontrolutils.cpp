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
/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/

#include "rendercontrolutils.h"

#include <QDebug>

/*====================================================================
 *
 * RenderControlGroup class
 *
 *===================================================================*/

/*====================================================================
 * Public methods
 *===================================================================*/

RenderControlGroup::RenderControlGroup(QObject * obj, const char * name, int initValue, int minimum, int maximum)
{
    initialize(obj, name, initValue, minimum, maximum);
}

/*====================================================================
 * Private methods
 *===================================================================*/

void RenderControlGroup::initialize(QObject * obj, const char *name, int initValue, int minimum, int maximum)
{
    initLabel(name);
    initSlider(obj, initValue, minimum, maximum);
    initSpinBox(initValue, minimum, maximum);
    connect();
}

void RenderControlGroup::initLabel(const char * name)
{
    label = new QLabel(name);
}


void RenderControlGroup::initSlider(QObject *obj, int initValue, int minimum, int maximum)
{
    slider = new QSlider();
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(minimum, maximum);
    slider->setValue(initValue);
    slider->installEventFilter(obj);
}

void RenderControlGroup::initSpinBox(int initValue, int minimum, int maximum)
{
    spinBox = new QSpinBox();
    spinBox->setRange(minimum, maximum);
    spinBox->setKeyboardTracking(false);
    spinBox->setValue(initValue);
}

void RenderControlGroup::connect()
{
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
    QObject::connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
}

/*====================================================================
 * Private slots
 *===================================================================*/

void RenderControlGroup::sliderChanged(int value)
{
    // Intercept changes to the slider and update the spin box. Note that
    // we suppress signals from the spin box so that it doesn't emit a
    // valueChanged() signal, which would cause the image to be updated.
    // For more information, see the comments for RenderControlGroup.

    spinBox->blockSignals(true);
    spinBox->setValue(value);
    spinBox->blockSignals(false);
}


/*====================================================================
 *
 * RenderControlUtils class
 *
 *===================================================================*/

/*====================================================================
 * Public methods
 *===================================================================*/

RenderControlUtils::RenderControlUtils() :
    groups()
{

}

QLayout* RenderControlUtils::createLayout()
{
    QGridLayout *layout = new QGridLayout;
    int size = groups.size();

    // Cycle through the groups of label/slider/spin box and lay
    // them out in a grid. Place stretchable blank rows between
    // each group.

    for (int i = 0; i < size; i++)
    {
        layout->setRowStretch(2*i, 100);
        RenderControlGroup* group = groups.at(i);
        layout->addWidget(group->label, 2*i + 1, 0);
        layout->addWidget(group->slider, 2*i + 1, 1);
        layout->addWidget(group->spinBox, 2*i + 1, 2);
    }
    layout->setRowStretch(2*size, 100);
    layout->setColumnStretch(1, 100);

    return layout;
}

int RenderControlUtils::getSliderIndex(QObject * watched, QEvent * event)
{
    // When a slider receives an event, find out which slider received
    // the event. If the event is releasing the left or middle mouse button,
    // return the index of the slider. Otherwise, return -1.

    for (int i = 0; i < groups.size(); i++)
    {
        if (watched == groups.at(i)->slider)
        {
            if (event->type() == QEvent::MouseButtonRelease)
            {
                Qt::MouseButton button = static_cast<QMouseEvent*>(event)->button();
                if ((button == Qt::LeftButton) || (button == Qt::MiddleButton))
                    return i;
            }
        }
    }

    return -1;
}

