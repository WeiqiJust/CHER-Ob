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

 - Acknowledgments: Some portions of this file are based on the example codes
   of ITK/VTK library from Kitware, QT API from Nokia. I would like to thank
   annonymous help by various software engineering communities.

*****************************************************************************/
#ifndef MYVTKINTERACTORSTYLE_H
#define MYVTKINTERACTORSTYLE_H


#include <vtkInteractorStyleTrackballCamera.h>

class myVTKInteractorStyle: public vtkInteractorStyleTrackballCamera
{
public:
	static myVTKInteractorStyle* New();
    vtkTypeMacro(myVTKInteractorStyle, vtkInteractorStyleTrackballCamera);

	virtual void OnMouseMove()
    {
		int x = this->Interactor->GetEventPosition()[0];
		int y = this->Interactor->GetEventPosition()[1];

		switch (this->State)
		{
		case VTKIS_PAN:
			this->FindPokedRenderer(x, y);
			this->Pan();
			this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
			break;
		}
    }
};
vtkStandardNewMacro(myVTKInteractorStyle);

#endif
