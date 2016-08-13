# Directory containing class headers.
SET(VTK_VIEWS_HEADER_DIR "${VTK_INSTALL_PREFIX}/include/vtk-5.10")

# Classes in vtkViews.
SET(VTK_VIEWS_CLASSES
  "vtkConvertSelectionDomain"
  "vtkDataRepresentation"
  "vtkEmptyRepresentation"
  "vtkGraphLayoutView"
  "vtkHierarchicalGraphPipeline"
  "vtkHierarchicalGraphView"
  "vtkIcicleView"
  "vtkInteractorStyleAreaSelectHover"
  "vtkInteractorStyleTreeMapHover"
  "vtkRenderedSurfaceRepresentation"
  "vtkRenderedGraphRepresentation"
  "vtkRenderedRepresentation"
  "vtkRenderedTreeAreaRepresentation"
  "vtkRenderedHierarchyRepresentation"
  "vtkRenderView"
  "vtkRenderViewBase"
  "vtkTreeAreaView"
  "vtkTreeMapView"
  "vtkTreeRingView"
  "vtkView"
  "vtkViewUpdater"
  "vtkParallelCoordinatesHistogramRepresentation"
  "vtkParallelCoordinatesRepresentation"
  "vtkParallelCoordinatesView")

# Abstract classes in vtkViews.
SET(VTK_VIEWS_CLASSES_ABSTRACT)

# Wrap-exclude classes in vtkViews.
SET(VTK_VIEWS_CLASSES_WRAP_EXCLUDE)

# Wrap-special classes in vtkViews.
SET(VTK_VIEWS_CLASSES_WRAP_SPECIAL)

# Wrappable non-class headers for vtkViews.
SET(VTK_VIEWS_WRAP_HEADERS)

# Set convenient variables to test each class.
FOREACH(class ${VTK_VIEWS_CLASSES})
  SET(VTK_CLASS_EXISTS_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_VIEWS_CLASSES_ABSTRACT})
  SET(VTK_CLASS_ABSTRACT_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_VIEWS_CLASSES_WRAP_EXCLUDE})
  SET(VTK_CLASS_WRAP_EXCLUDE_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_VIEWS_CLASSES_WRAP_SPECIAL})
  SET(VTK_CLASS_WRAP_SPECIAL_${class} 1)
ENDFOREACH(class)
