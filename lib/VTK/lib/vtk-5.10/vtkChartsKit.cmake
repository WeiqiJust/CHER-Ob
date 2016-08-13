# Directory containing class headers.
SET(VTK_CHARTS_HEADER_DIR "${VTK_INSTALL_PREFIX}/include/vtk-5.10")

# Classes in vtkCharts.
SET(VTK_CHARTS_CLASSES
  "vtkAbstractContextBufferId"
  "vtkAbstractContextItem"
  "vtkAxis"
  "vtkAxisExtended"
  "vtkBlockItem"
  "vtkBrush"
  "vtkChart"
  "vtkChartLegend"
  "vtkChartHistogram2D"
  "vtkChartMatrix"
  "vtkChartParallelCoordinates"
  "vtkChartXY"
  "vtkChartPie"
  "vtkColorLegend"
  "vtkPlotPie"
  "vtkColorSeries"
  "vtkColorTransferFunctionItem"
  "vtkColorTransferControlPointsItem"
  "vtkCompositeControlPointsItem"
  "vtkCompositeTransferFunctionItem"
  "vtkContext2D"
  "vtkContextActor"
  "vtkContextBufferId"
  "vtkContextClip"
  "vtkContextDevice2D"
  "vtkContextInteractorStyle"
  "vtkContextItem"
  "vtkContextKeyEvent"
  "vtkContextMapper2D"
  "vtkContextMouseEvent"
  "vtkContextScene"
  "vtkContextTransform"
  "vtkContextView"
  "vtkControlPointsItem"
  "vtkImageItem"
  "vtkLookupTableItem"
  "vtkOpenGLContextBufferId"
  "vtkOpenGL2ContextDevice2D"
  "vtkOpenGLContextDevice2D"
  "vtkPen"
  "vtkPiecewiseControlPointsItem"
  "vtkPiecewiseFunctionItem"
  "vtkPiecewisePointHandleItem"
  "vtkPlot"
  "vtkPlotBar"
  "vtkPlotGrid"
  "vtkPlotHistogram2D"
  "vtkPlotLine"
  "vtkPlotStacked"
  "vtkPlotParallelCoordinates"
  "vtkPlotPoints"
  "vtkScalarsToColorsItem"
  "vtkScatterPlotMatrix"
  "vtkTooltipItem")

# Abstract classes in vtkCharts.
SET(VTK_CHARTS_CLASSES_ABSTRACT
  "vtkAbstractContextBufferId"
  "vtkAbstractContextItem"
  "vtkChart"
  "vtkContextDevice2D"
  "vtkContextItem"
  "vtkContextMapper2D"
  "vtkControlPointsItem"
  "vtkPlot"
  "vtkScalarsToColorsItem")

# Wrap-exclude classes in vtkCharts.
SET(VTK_CHARTS_CLASSES_WRAP_EXCLUDE
  "vtkAbstractContextBufferId"
  "vtkContextBufferId"
  "vtkContextKeyEvent"
  "vtkContextMouseEvent"
  "vtkOpenGLContextBufferId"
  "vtkOpenGL2ContextDevice2D"
  "vtkOpenGLContextDevice2D")

# Wrap-special classes in vtkCharts.
SET(VTK_CHARTS_CLASSES_WRAP_SPECIAL)

# Wrappable non-class headers for vtkCharts.
SET(VTK_CHARTS_WRAP_HEADERS)

# Set convenient variables to test each class.
FOREACH(class ${VTK_CHARTS_CLASSES})
  SET(VTK_CLASS_EXISTS_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_CHARTS_CLASSES_ABSTRACT})
  SET(VTK_CLASS_ABSTRACT_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_CHARTS_CLASSES_WRAP_EXCLUDE})
  SET(VTK_CLASS_WRAP_EXCLUDE_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_CHARTS_CLASSES_WRAP_SPECIAL})
  SET(VTK_CLASS_WRAP_SPECIAL_${class} 1)
ENDFOREACH(class)
