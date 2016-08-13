# Directory containing class headers.
SET(VTK_VOLUMERENDERING_HEADER_DIR "${VTK_INSTALL_PREFIX}/include/vtk-5.10")

# Classes in vtkVolumeRendering.
SET(VTK_VOLUMERENDERING_CLASSES
  "vtkDirectionEncoder"
  "vtkEncodedGradientEstimator"
  "vtkEncodedGradientShader"
  "vtkFiniteDifferenceGradientEstimator"
  "vtkFixedPointRayCastImage"
  "vtkFixedPointVolumeRayCastCompositeGOHelper"
  "vtkFixedPointVolumeRayCastCompositeGOShadeHelper"
  "vtkFixedPointVolumeRayCastCompositeHelper"
  "vtkFixedPointVolumeRayCastCompositeShadeHelper"
  "vtkFixedPointVolumeRayCastHelper"
  "vtkFixedPointVolumeRayCastMIPHelper"
  "vtkFixedPointVolumeRayCastMapper"
  "vtkVolumeRayCastSpaceLeapingImageFilter"
  "vtkGPUVolumeRayCastMapper"
  "vtkHAVSVolumeMapper"
  "vtkProjectedAAHexahedraMapper"
  "vtkProjectedTetrahedraMapper"
  "vtkRayCastImageDisplayHelper"
  "vtkRecursiveSphereDirectionEncoder"
  "vtkSmartVolumeMapper"
  "vtkSphericalDirectionEncoder"
  "vtkVolumeMapper"
  "vtkVolumeOutlineSource"
  "vtkVolumePicker"
  "vtkVolumeProMapper"
  "vtkVolumeRayCastCompositeFunction"
  "vtkVolumeRayCastFunction"
  "vtkVolumeRayCastIsosurfaceFunction"
  "vtkVolumeRayCastMIPFunction"
  "vtkVolumeRayCastMapper"
  "vtkVolumeRenderingFactory"
  "vtkVolumeTextureMapper"
  "vtkVolumeTextureMapper2D"
  "vtkVolumeTextureMapper3D"
  "vtkUnstructuredGridBunykRayCastFunction"
  "vtkUnstructuredGridHomogeneousRayIntegrator"
  "vtkUnstructuredGridLinearRayIntegrator"
  "vtkUnstructuredGridPartialPreIntegration"
  "vtkUnstructuredGridPreIntegration"
  "vtkUnstructuredGridVolumeMapper"
  "vtkUnstructuredGridVolumeRayCastFunction"
  "vtkUnstructuredGridVolumeRayCastIterator"
  "vtkUnstructuredGridVolumeRayIntegrator"
  "vtkUnstructuredGridVolumeRayCastMapper"
  "vtkUnstructuredGridVolumeZSweepMapper"
  "vtkOpenGLGPUVolumeRayCastMapper"
  "vtkOpenGLHAVSVolumeMapper"
  "vtkOpenGLProjectedAAHexahedraMapper"
  "vtkOpenGLProjectedTetrahedraMapper"
  "vtkOpenGLRayCastImageDisplayHelper"
  "vtkOpenGLVolumeTextureMapper2D"
  "vtkOpenGLVolumeTextureMapper3D")

# Abstract classes in vtkVolumeRendering.
SET(VTK_VOLUMERENDERING_CLASSES_ABSTRACT
  "vtkDirectionEncoder"
  "vtkEncodedGradientEstimator"
  "vtkFixedPointVolumeRayCastHelper"
  "vtkRayCastImageDisplayHelper"
  "vtkVolumeMapper"
  "vtkVolumeRayCastFunction"
  "vtkVolumeTextureMapper"
  "vtkUnstructuredGridVolumeMapper"
  "vtkUnstructuredGridVolumeRayCastFunction"
  "vtkUnstructuredGridVolumeRayCastIterator"
  "vtkUnstructuredGridVolumeRayIntegrator")

# Wrap-exclude classes in vtkVolumeRendering.
SET(VTK_VOLUMERENDERING_CLASSES_WRAP_EXCLUDE)

# Wrap-special classes in vtkVolumeRendering.
SET(VTK_VOLUMERENDERING_CLASSES_WRAP_SPECIAL)

# Wrappable non-class headers for vtkVolumeRendering.
SET(VTK_VOLUMERENDERING_WRAP_HEADERS)

# Set convenient variables to test each class.
FOREACH(class ${VTK_VOLUMERENDERING_CLASSES})
  SET(VTK_CLASS_EXISTS_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_VOLUMERENDERING_CLASSES_ABSTRACT})
  SET(VTK_CLASS_ABSTRACT_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_VOLUMERENDERING_CLASSES_WRAP_EXCLUDE})
  SET(VTK_CLASS_WRAP_EXCLUDE_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_VOLUMERENDERING_CLASSES_WRAP_SPECIAL})
  SET(VTK_CLASS_WRAP_SPECIAL_${class} 1)
ENDFOREACH(class)
