#ifndef __vtkParallelInstantiator_h
#define __vtkParallelInstantiator_h

#include "vtkInstantiator.h"



class VTK_PARALLEL_EXPORT vtkParallelInstantiator
{
  public:
  vtkParallelInstantiator();
  ~vtkParallelInstantiator();
  private:
  static void ClassInitialize();
  static void ClassFinalize();
  static unsigned int Count;
}; 

static vtkParallelInstantiator vtkParallelInstantiatorInitializer;

#endif
