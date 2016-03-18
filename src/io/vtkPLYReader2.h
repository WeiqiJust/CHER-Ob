/****************************************************************************

 - Codename: Hyper3D (Yale Computer Graphics Group)

 - Writers:   Min H. Kim (minhkim@cs.yale.edu)

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

 - Acknowledgments: This PLY file reader is based on the original code of
   vtkPLYReader of VTK library from Kitware. The difference is that this reader
   can read texture coordinates as well as vertexes and face indices.
   This function is only valid when the number of texture coordinates is
   same as that of vertexes.

*****************************************************************************/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPLYReader2.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPLYReader2 - read Stanford University PLY polygonal file format
// .SECTION Description
// vtkPLYReader2 is a source object that reads polygonal data in
// Stanford University PLY file format (see 
// http://graphics.stanford.edu/data/3Dscanrep). It requires that
// the elements "vertex" and "face" are defined. The "vertex" element
// must have the properties "x", "y", and "z". The "face" element must
// have the property "vertex_indices" defined. Optionally, if the "face"
// element has the properties "intensity" and/or the triplet "red",
// "green", and "blue"; these are read and added as scalars to the
// output data.

// .SECTION See Also
// vtkPLYWriter

#ifndef __vtkPLYReader2_h
#define __vtkPLYReader2_h

#include "vtkPolyDataAlgorithm.h"

class vtkPLYReader2 : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkPLYReader2,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with merging set to true.
  static vtkPLYReader2 *New();

  // Description:
  // Specify file name of stereo lithography file.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // A simple, non-exhaustive check to see if a file is a valid ply file.
  static int CanReadFile(const char *filename);

protected:
  vtkPLYReader2();
  ~vtkPLYReader2();

  char *FileName;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
private:
  vtkPLYReader2(const vtkPLYReader2&);  // Not implemented.
  void operator=(const vtkPLYReader2&);  // Not implemented.
};

#endif
