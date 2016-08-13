/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Min H. Kim (minhkim@cs.yale.edu)

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

 - Acknowledgments: This VRML file reader is based on the original code of
   vtkVRMLSource of ParaView from Kitware. The difference is that this reader
   can read texture coordinates as well as vertexes and face indices.
   This function is only functional when the number of texture coordinates is
   same as that of vertexes.

*****************************************************************************/
/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile: vtkVRMLSource.h,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVRMLSource - Converts importer to a source.
// .SECTION Description
// Since paraview can only use vtkSources, I am wrapping the VRML importer
// as a source.  I will loose lights, texture maps and colors,
// MK: now it will include texture coordinates in PolyData.

#ifndef __vtkVRMLSource2_h
#define __vtkVRMLSource2_h

#include "vtkSource.h"
class vtkVRMLImporter;
class vtkPolyData;

class vtkVRMLSource2 : public vtkSource
{
public:
  vtkTypeRevisionMacro(vtkVRMLSource2,vtkSource)
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkVRMLSource2 *New();

  // Description:
  // VRML file name.  Set
  vtkSetStringMacro(FileName)
  vtkGetStringMacro(FileName)

  int GetNumberOfOutputs();
  vtkPolyData* GetOutput(int idx);
  vtkPolyData* GetOutput() { return this->GetOutput(0);}

  // Description: 
  // Descided whether to generate color arrays or not.
  vtkSetMacro(Color,int) // usage example: this->SetColor(1);
  vtkGetMacro(Color,int)
  vtkBooleanMacro(Color,int)

  // Description:
  // This method allows all parts to be put into a single output.
  // By default this flag is on.
  vtkSetMacro(Append,int) // usage example: this->SetAppend(1);
  vtkGetMacro(Append,int)
  vtkBooleanMacro(Append,int)

protected:
  vtkVRMLSource2();
  ~vtkVRMLSource2();

  void Execute();
  void InitializeImporter();
  void CopyImporterToOutputs();

  char* FileName;
  vtkVRMLImporter *Importer;
  int Color;
  int Append;

private:
  vtkVRMLSource2(const vtkVRMLSource2&);  // Not implemented.
  void operator=(const vtkVRMLSource2&);  // Not implemented.
};

#endif

