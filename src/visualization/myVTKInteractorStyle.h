
#ifndef MYVTKINTERACTORSTYLE_H
#define MYVTKINTERACTORSTYLE_H

//#include <vtkInteractorStyleImage.h>
//
//class myVTKInteractorStyle: public vtkInteractorStyleImage
//{
//public:
//	static myVTKInteractorStyle* New();
//    vtkTypeMacro(myVTKInteractorStyle, vtkInteractorStyleImage);
//
//	virtual void OnMouseMove()
//    {
//    }
//};
//vtkStandardNewMacro(myVTKInteractorStyle);


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
