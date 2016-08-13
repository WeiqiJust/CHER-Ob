#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vtksys" for configuration "Release"
set_property(TARGET vtksys APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtksys PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtksys.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "ws2_32"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtksys.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtksys )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtksys "${_IMPORT_PREFIX}/lib/vtk-5.10/vtksys.lib" "${_IMPORT_PREFIX}/bin/vtksys.dll" )

# Import target "vtkzlib" for configuration "Release"
set_property(TARGET vtkzlib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkzlib PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkzlib.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkzlib.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkzlib )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkzlib "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkzlib.lib" "${_IMPORT_PREFIX}/bin/vtkzlib.dll" )

# Import target "vtkhdf5" for configuration "Release"
set_property(TARGET vtkhdf5 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkhdf5 PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkhdf5.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "ws2_32;wsock32;vtkzlib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkhdf5.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkhdf5 )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkhdf5 "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkhdf5.lib" "${_IMPORT_PREFIX}/bin/vtkhdf5.dll" )

# Import target "vtkhdf5_hl" for configuration "Release"
set_property(TARGET vtkhdf5_hl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkhdf5_hl PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkhdf5_hl.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkhdf5"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkhdf5_hl.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkhdf5_hl )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkhdf5_hl "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkhdf5_hl.lib" "${_IMPORT_PREFIX}/bin/vtkhdf5_hl.dll" )

# Import target "vtkjpeg" for configuration "Release"
set_property(TARGET vtkjpeg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkjpeg PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkjpeg.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkjpeg.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkjpeg )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkjpeg "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkjpeg.lib" "${_IMPORT_PREFIX}/bin/vtkjpeg.dll" )

# Import target "vtkpng" for configuration "Release"
set_property(TARGET vtkpng APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkpng PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkpng.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkzlib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkpng.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkpng )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkpng "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkpng.lib" "${_IMPORT_PREFIX}/bin/vtkpng.dll" )

# Import target "vtktiff" for configuration "Release"
set_property(TARGET vtktiff APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtktiff PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtktiff.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkzlib;vtkjpeg"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtktiff.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtktiff )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtktiff "${_IMPORT_PREFIX}/lib/vtk-5.10/vtktiff.lib" "${_IMPORT_PREFIX}/bin/vtktiff.dll" )

# Import target "vtkexpat" for configuration "Release"
set_property(TARGET vtkexpat APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkexpat PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkexpat.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkexpat.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkexpat )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkexpat "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkexpat.lib" "${_IMPORT_PREFIX}/bin/vtkexpat.dll" )

# Import target "vtkfreetype" for configuration "Release"
set_property(TARGET vtkfreetype APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkfreetype PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkfreetype.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkfreetype.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkfreetype )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkfreetype "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkfreetype.lib" "${_IMPORT_PREFIX}/bin/vtkfreetype.dll" )

# Import target "vtklibxml2" for configuration "Release"
set_property(TARGET vtklibxml2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtklibxml2 PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtklibxml2.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkzlib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtklibxml2.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtklibxml2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtklibxml2 "${_IMPORT_PREFIX}/lib/vtk-5.10/vtklibxml2.lib" "${_IMPORT_PREFIX}/bin/vtklibxml2.dll" )

# Import target "vtkDICOMParser" for configuration "Release"
set_property(TARGET vtkDICOMParser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkDICOMParser PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkDICOMParser.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkDICOMParser.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkDICOMParser )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkDICOMParser "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkDICOMParser.lib" "${_IMPORT_PREFIX}/bin/vtkDICOMParser.dll" )

# Import target "vtkproj4" for configuration "Release"
set_property(TARGET vtkproj4 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkproj4 PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkproj4.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkproj4.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkproj4 )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkproj4 "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkproj4.lib" "${_IMPORT_PREFIX}/bin/vtkproj4.dll" )

# Import target "mpistubs" for configuration "Release"
set_property(TARGET mpistubs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mpistubs PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/mpistubs.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtksys"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/mpistubs.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS mpistubs )
list(APPEND _IMPORT_CHECK_FILES_FOR_mpistubs "${_IMPORT_PREFIX}/lib/vtk-5.10/mpistubs.lib" "${_IMPORT_PREFIX}/bin/mpistubs.dll" )

# Import target "MapReduceMPI" for configuration "Release"
set_property(TARGET MapReduceMPI APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(MapReduceMPI PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/MapReduceMPI.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "mpistubs"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/MapReduceMPI.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS MapReduceMPI )
list(APPEND _IMPORT_CHECK_FILES_FOR_MapReduceMPI "${_IMPORT_PREFIX}/lib/vtk-5.10/MapReduceMPI.lib" "${_IMPORT_PREFIX}/bin/MapReduceMPI.dll" )

# Import target "vtkverdict" for configuration "Release"
set_property(TARGET vtkverdict APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkverdict PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkverdict.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkverdict.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkverdict )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkverdict "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkverdict.lib" "${_IMPORT_PREFIX}/bin/vtkverdict.dll" )

# Import target "vtkNetCDF" for configuration "Release"
set_property(TARGET vtkNetCDF APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkNetCDF PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkNetCDF.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkhdf5;vtkhdf5_hl"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkNetCDF.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkNetCDF )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkNetCDF "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkNetCDF.lib" "${_IMPORT_PREFIX}/bin/vtkNetCDF.dll" )

# Import target "vtkNetCDF_cxx" for configuration "Release"
set_property(TARGET vtkNetCDF_cxx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkNetCDF_cxx PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkNetCDF_cxx.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkNetCDF"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkNetCDF_cxx.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkNetCDF_cxx )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkNetCDF_cxx "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkNetCDF_cxx.lib" "${_IMPORT_PREFIX}/bin/vtkNetCDF_cxx.dll" )

# Import target "vtkmetaio" for configuration "Release"
set_property(TARGET vtkmetaio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkmetaio PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkmetaio.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkzlib;comctl32;wsock32"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkmetaio.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkmetaio )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkmetaio "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkmetaio.lib" "${_IMPORT_PREFIX}/bin/vtkmetaio.dll" )

# Import target "vtksqlite" for configuration "Release"
set_property(TARGET vtksqlite APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtksqlite PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtksqlite.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtksqlite )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtksqlite "${_IMPORT_PREFIX}/lib/vtk-5.10/vtksqlite.lib" )

# Import target "vtkexoIIc" for configuration "Release"
set_property(TARGET vtkexoIIc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkexoIIc PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkexoIIc.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkNetCDF"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkexoIIc.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkexoIIc )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkexoIIc "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkexoIIc.lib" "${_IMPORT_PREFIX}/bin/vtkexoIIc.dll" )

# Import target "LSDyna" for configuration "Release"
set_property(TARGET LSDyna APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LSDyna PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/LSDyna.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtksys"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/LSDyna.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS LSDyna )
list(APPEND _IMPORT_CHECK_FILES_FOR_LSDyna "${_IMPORT_PREFIX}/lib/vtk-5.10/LSDyna.lib" "${_IMPORT_PREFIX}/bin/LSDyna.dll" )

# Import target "vtkalglib" for configuration "Release"
set_property(TARGET vtkalglib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkalglib PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkalglib.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkalglib.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkalglib )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkalglib "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkalglib.lib" "${_IMPORT_PREFIX}/bin/vtkalglib.dll" )

# Import target "vtkEncodeString" for configuration "Release"
set_property(TARGET vtkEncodeString APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkEncodeString PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkEncodeString.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkEncodeString )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkEncodeString "${_IMPORT_PREFIX}/bin/vtkEncodeString.exe" )

# Import target "VPIC" for configuration "Release"
set_property(TARGET VPIC APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(VPIC PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/VPIC.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtksys"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/VPIC.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS VPIC )
list(APPEND _IMPORT_CHECK_FILES_FOR_VPIC "${_IMPORT_PREFIX}/lib/vtk-5.10/VPIC.lib" "${_IMPORT_PREFIX}/bin/VPIC.dll" )

# Import target "Cosmo" for configuration "Release"
set_property(TARGET Cosmo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Cosmo PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/Cosmo.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtksys;vtkCommon"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/Cosmo.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS Cosmo )
list(APPEND _IMPORT_CHECK_FILES_FOR_Cosmo "${_IMPORT_PREFIX}/lib/vtk-5.10/Cosmo.lib" "${_IMPORT_PREFIX}/bin/Cosmo.dll" )

# Import target "vtkftgl" for configuration "Release"
set_property(TARGET vtkftgl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkftgl PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkftgl.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "opengl32;vtkfreetype"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkftgl.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkftgl )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkftgl "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkftgl.lib" "${_IMPORT_PREFIX}/bin/vtkftgl.dll" )

# Import target "vtkCommon" for configuration "Release"
set_property(TARGET vtkCommon APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkCommon PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkCommon.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtksys"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkCommon.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkCommon )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkCommon "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkCommon.lib" "${_IMPORT_PREFIX}/bin/vtkCommon.dll" )

# Import target "vtkFiltering" for configuration "Release"
set_property(TARGET vtkFiltering APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkFiltering PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkFiltering.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkCommon"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkFiltering.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkFiltering )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkFiltering "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkFiltering.lib" "${_IMPORT_PREFIX}/bin/vtkFiltering.dll" )

# Import target "vtkImaging" for configuration "Release"
set_property(TARGET vtkImaging APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkImaging PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkImaging.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkFiltering"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkImaging.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkImaging )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkImaging "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkImaging.lib" "${_IMPORT_PREFIX}/bin/vtkImaging.dll" )

# Import target "vtkGraphics" for configuration "Release"
set_property(TARGET vtkGraphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkGraphics PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkGraphics.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vtkverdict"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkFiltering"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkGraphics.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkGraphics )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkGraphics "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkGraphics.lib" "${_IMPORT_PREFIX}/bin/vtkGraphics.dll" )

# Import target "vtkGenericFiltering" for configuration "Release"
set_property(TARGET vtkGenericFiltering APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkGenericFiltering PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkGenericFiltering.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkFiltering;vtkGraphics"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkGenericFiltering.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkGenericFiltering )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkGenericFiltering "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkGenericFiltering.lib" "${_IMPORT_PREFIX}/bin/vtkGenericFiltering.dll" )

# Import target "vtkIO" for configuration "Release"
set_property(TARGET vtkIO APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkIO PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkIO.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vtkDICOMParser;vtkNetCDF;vtkNetCDF_cxx;LSDyna;vtkmetaio;vtkpng;vtkzlib;vtkjpeg;vtktiff;vtkexpat;vtksys"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkFiltering"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkIO.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkIO )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkIO "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkIO.lib" "${_IMPORT_PREFIX}/bin/vtkIO.dll" )

# Import target "vtkRendering" for configuration "Release"
set_property(TARGET vtkRendering APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkRendering PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkRendering.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vtkIO;vtkftgl;vtkfreetype"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkGraphics;vtkImaging"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkRendering.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkRendering )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkRendering "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkRendering.lib" "${_IMPORT_PREFIX}/bin/vtkRendering.dll" )

# Import target "vtkVolumeRendering" for configuration "Release"
set_property(TARGET vtkVolumeRendering APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkVolumeRendering PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkVolumeRendering.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkRendering;vtkIO"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkVolumeRendering.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkVolumeRendering )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkVolumeRendering "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkVolumeRendering.lib" "${_IMPORT_PREFIX}/bin/vtkVolumeRendering.dll" )

# Import target "vtkHybrid" for configuration "Release"
set_property(TARGET vtkHybrid APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkHybrid PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkHybrid.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vtkexoIIc;vtkftgl"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkRendering;vtkIO;vtkParallel"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkHybrid.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkHybrid )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkHybrid "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkHybrid.lib" "${_IMPORT_PREFIX}/bin/vtkHybrid.dll" )

# Import target "vtkWidgets" for configuration "Release"
set_property(TARGET vtkWidgets APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkWidgets PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkWidgets.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkRendering;vtkHybrid;vtkVolumeRendering"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkWidgets.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkWidgets )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkWidgets "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkWidgets.lib" "${_IMPORT_PREFIX}/bin/vtkWidgets.dll" )

# Import target "vtkParallel" for configuration "Release"
set_property(TARGET vtkParallel APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkParallel PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkParallel.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "VPIC;Cosmo;LSDyna;vtkexoIIc"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkRendering;vtkIO"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkParallel.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkParallel )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkParallel "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkParallel.lib" "${_IMPORT_PREFIX}/bin/vtkParallel.dll" )

# Import target "vtkInfovis" for configuration "Release"
set_property(TARGET vtkInfovis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkInfovis PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkInfovis.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vtklibxml2;vtkalglib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkWidgets;vtkParallel"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkInfovis.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkInfovis )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkInfovis "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkInfovis.lib" "${_IMPORT_PREFIX}/bin/vtkInfovis.dll" )

# Import target "vtkGeovis" for configuration "Release"
set_property(TARGET vtkGeovis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkGeovis PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkGeovis.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vtkproj4"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkWidgets;vtkViews"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkGeovis.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkGeovis )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkGeovis "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkGeovis.lib" "${_IMPORT_PREFIX}/bin/vtkGeovis.dll" )

# Import target "vtkViews" for configuration "Release"
set_property(TARGET vtkViews APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkViews PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkViews.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkInfovis"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkViews.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkViews )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkViews "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkViews.lib" "${_IMPORT_PREFIX}/bin/vtkViews.dll" )

# Import target "vtkQtChart" for configuration "Release"
set_property(TARGET vtkQtChart APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkQtChart PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkQtChart.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "C:/Qt/4.8.5/lib/QtOpenGL4.lib;C:/Qt/4.8.5/lib/QtWebKit4.lib;C:/Qt/4.8.5/lib/QtGui4.lib;C:/Qt/4.8.5/lib/QtSql4.lib;C:/Qt/4.8.5/lib/QtNetwork4.lib;C:/Qt/4.8.5/lib/QtCore4.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkQtChart.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkQtChart )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkQtChart "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkQtChart.lib" "${_IMPORT_PREFIX}/bin/vtkQtChart.dll" )

# Import target "QVTK" for configuration "Release"
set_property(TARGET QVTK APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(QVTK PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/QVTK.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "C:/Qt/4.8.5/lib/QtOpenGL4.lib;C:/Qt/4.8.5/lib/QtWebKit4.lib;C:/Qt/4.8.5/lib/QtGui4.lib;C:/Qt/4.8.5/lib/QtSql4.lib;C:/Qt/4.8.5/lib/QtNetwork4.lib;C:/Qt/4.8.5/lib/QtCore4.lib;vtkRendering;vtkIO;vtkGraphics;vtkImaging;vtkCommon;vtkViews;opengl32;vtkQtChart"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/QVTK.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS QVTK )
list(APPEND _IMPORT_CHECK_FILES_FOR_QVTK "${_IMPORT_PREFIX}/lib/vtk-5.10/QVTK.lib" "${_IMPORT_PREFIX}/bin/QVTK.dll" )

# Import target "vtkCharts" for configuration "Release"
set_property(TARGET vtkCharts APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(vtkCharts PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkCharts.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "vtkftgl"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "vtkViews"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vtkCharts.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS vtkCharts )
list(APPEND _IMPORT_CHECK_FILES_FOR_vtkCharts "${_IMPORT_PREFIX}/lib/vtk-5.10/vtkCharts.lib" "${_IMPORT_PREFIX}/bin/vtkCharts.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
