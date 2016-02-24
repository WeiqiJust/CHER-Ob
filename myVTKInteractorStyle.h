
#ifndef MYVTKINTERACTORSTYLE_H
#define MYVTKINTERACTORSTYLE_H

#include <vtkInteractorStyleImage.h>

class myVTKInteractorStyle: public vtkInteractorStyleImage
{
public:
	static myVTKInteractorStyle* New();
    vtkTypeMacro(myVTKInteractorStyle, vtkInteractorStyleImage);

	virtual void OnMouseMove()
    {
		///*int x = this->Interactor->GetEventPosition()[0];
		//int y = this->Interactor->GetEventPosition()[1];*/

		//switch (this->State)
		//{
		//case VTKIS_WINDOW_LEVEL:
		//  /*this->FindPokedRenderer(x, y);
		//  this->WindowLevel();
		//  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);*/
		//  break;

		///*case VTKIS_PICK:
		//  this->FindPokedRenderer(x, y);
		//  this->Pick();
		//  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		//  break;

		//case VTKIS_SLICE:
		//  this->FindPokedRenderer(x, y);
		//  this->Slice();
		//  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		//  break;*/
		//}

		//// Call parent to handle all other states and perform additional work
		////this->Superclass::OnMouseMove();
    }
};
vtkStandardNewMacro(myVTKInteractorStyle);

#endif
