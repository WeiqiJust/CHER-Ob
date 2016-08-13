# Directory containing class headers.
SET(VTK_PARALLEL_HEADER_DIR "${VTK_INSTALL_PREFIX}/include/vtk-5.10")

# Classes in vtkParallel.
SET(VTK_PARALLEL_CLASSES
  "vtkBranchExtentTranslator"
  "vtkCachingInterpolatedVelocityField"
  "vtkClientServerSynchronizedRenderers"
  "vtkCollectGraph"
  "vtkCollectPolyData"
  "vtkCollectTable"
  "vtkCommunicator"
  "vtkCompositedSynchronizedRenderers"
  "vtkCompositer"
  "vtkCompressCompositer"
  "vtkCutMaterial"
  "vtkDistributedDataFilter"
  "vtkDistributedStreamTracer"
  "vtkDummyCommunicator"
  "vtkDummyController"
  "vtkDuplicatePolyData"
  "vtkEnSightWriter"
  "vtkExtractCTHPart"
  "vtkExtractPiece"
  "vtkExtractPolyDataPiece"
  "vtkExtractUnstructuredGridPiece"
  "vtkExtractUserDefinedPiece"
  "vtkMPIImageReader"
  "vtkMultiProcessController"
  "vtkMultiProcessStream"
  "vtkParallelFactory"
  "vtkPassThroughFilter"
  "vtkPCellDataToPointData"
  "vtkPChacoReader"
  "vtkPCosmoHaloFinder"
  "vtkPCosmoReader"
  "vtkPDataSetReader"
  "vtkPDataSetWriter"
  "vtkPExtractArraysOverTime"
  "vtkPHardwareSelector"
  "vtkPieceRequestFilter"
  "vtkPieceScalars"
  "vtkPKdTree"
  "vtkPLinearExtrusionFilter"
  "vtkPMaskPoints"
  "vtkPNrrdReader"
  "vtkPOPReader"
  "vtkPOpenFOAMReader"
  "vtkPOutlineCornerFilter"
  "vtkPOutlineFilter"
  "vtkPPolyDataNormals"
  "vtkPProbeFilter"
  "vtkProcess"
  "vtkProcessGroup"
  "vtkProcessIdScalars"
  "vtkPLSDynaReader"
  "vtkPReflectionFilter"
  "vtkPSLACReader"
  "vtkPSphereSource"
  "vtkPStreamTracer"
  "vtkPTableToStructuredGrid"
  "vtkPYoungsMaterialInterface"
  "vtkRectilinearGridOutlineFilter"
  "vtkSocketCommunicator"
  "vtkSocketController"
  "vtkSubCommunicator"
  "vtkSubGroup"
  "vtkTemporalFractal"
  "vtkTemporalInterpolatedVelocityField"
  "vtkTemporalStreamTracer"
  "vtkTransmitImageDataPiece"
  "vtkTransmitPolyDataPiece"
  "vtkTransmitRectilinearGridPiece"
  "vtkTransmitStructuredGridPiece"
  "vtkTransmitUnstructuredGridPiece"
  "vtkVPICReader"
  "vtkWindBladeReader"
  "vtkXMLPHierarchicalBoxDataWriter"
  "vtkXMLPMultiBlockDataWriter"
  "vtkClientServerCompositePass"
  "vtkCompositeRenderManager"
  "vtkCompositeRGBAPass"
  "vtkCompositeZPass"
  "vtkImageRenderManager"
  "vtkMemoryLimitImageDataStreamer"
  "vtkParallelRenderManager"
  "vtkPImageWriter"
  "vtkPipelineSize"
  "vtkSynchronizedRenderers"
  "vtkSynchronizedRenderWindows"
  "vtkTreeCompositer"
  "vtkExodusIIWriter")

# Abstract classes in vtkParallel.
SET(VTK_PARALLEL_CLASSES_ABSTRACT
  "vtkCommunicator"
  "vtkMultiProcessController"
  "vtkMultiProcessStream"
  "vtkProcess"
  "vtkPStreamTracer"
  "vtkParallelRenderManager")

# Wrap-exclude classes in vtkParallel.
SET(VTK_PARALLEL_CLASSES_WRAP_EXCLUDE
  "vtkMultiProcessStream"
  "vtkCompositeRGBAPass"
  "vtkCompositeZPass")

# Wrap-special classes in vtkParallel.
SET(VTK_PARALLEL_CLASSES_WRAP_SPECIAL)

# Wrappable non-class headers for vtkParallel.
SET(VTK_PARALLEL_WRAP_HEADERS)

# Set convenient variables to test each class.
FOREACH(class ${VTK_PARALLEL_CLASSES})
  SET(VTK_CLASS_EXISTS_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_PARALLEL_CLASSES_ABSTRACT})
  SET(VTK_CLASS_ABSTRACT_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_PARALLEL_CLASSES_WRAP_EXCLUDE})
  SET(VTK_CLASS_WRAP_EXCLUDE_${class} 1)
ENDFOREACH(class)
FOREACH(class ${VTK_PARALLEL_CLASSES_WRAP_SPECIAL})
  SET(VTK_CLASS_WRAP_SPECIAL_${class} 1)
ENDFOREACH(class)
