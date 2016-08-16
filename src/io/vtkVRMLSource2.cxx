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
  Module:    $RCSfile: vtkVRMLSource.cxx,v $

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkVRML.h"
#include "vtkVRMLSource2.h"
#include "vtkVRMLImporter.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkActorCollection.h"
#include "vtkActor.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"

#include <stdio.h>
#include <iostream>

vtkCxxRevisionMacro(vtkVRMLSource2, "$Revision: 2.0 $")
vtkStandardNewMacro(vtkVRMLSource2)

vtkVRMLSource2::vtkVRMLSource2()
{
  this->FileName = NULL;
  this->Importer = NULL;
  this->Color = 1;
  this->Append = 1;
}

vtkVRMLSource2::~vtkVRMLSource2()
{
  this->SetFileName(NULL);
  if (this->Importer)
    {
    this->Importer->Delete();
    this->Importer = NULL;
    }
}

vtkPolyData* vtkVRMLSource2::GetOutput(int idx)
{
  if (this->Importer == NULL)
    {
    this->InitializeImporter();
    }

  return (vtkPolyData *) this->vtkSource::GetOutput(idx); 
}

int vtkVRMLSource2::GetNumberOfOutputs()
{
  if (this->Append)
    {
    return 1;
    }

  if (this->Importer == NULL)
    {
    this->InitializeImporter();
    }

  return this->NumberOfOutputs;
}

void vtkVRMLSource2::Execute()
{
  if (this->Importer == NULL)
    {
    this->InitializeImporter();
    }
  this->CopyImporterToOutputs();
}

// actual file reading function
void vtkVRMLSource2::InitializeImporter()
{
  vtkRenderer* ren;
  vtkActorCollection* actors;
  vtkActor* actor;
  vtkPolyDataMapper* mapper;
  int idx;  

  if (this->Importer)
    {
    this->Importer->Delete();
    this->Importer = NULL;
    }
  this->Importer = vtkVRMLImporter::New();
  this->Importer->SetFileName(this->FileName);
  this->Importer->Read();
  ren = this->Importer->GetRenderer();
  actors = ren->GetActors();
  actors->InitTraversal();
  idx = 0;
  while ( (actor = actors->GetNextActor()) )
    {
    mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
    if (mapper)
      {
      //mapper->GetInput()->Update();
      vtkPolyData *newOutput = vtkPolyData::New();
      newOutput->CopyInformation(mapper->GetInput());
      this->SetNthOutput(idx, newOutput);
      ++idx;
      newOutput->Delete();
      newOutput = NULL;
      }
    }
}

//MK: this function is mainly changed
void vtkVRMLSource2::CopyImporterToOutputs()
{
  vtkRenderer* ren;
  vtkActorCollection* actors;
  vtkActor* actor;
  vtkPolyDataMapper* mapper;
  vtkPolyData* input;
  vtkPolyData* output;
  int idx;
  int numArrays, arrayIdx, numPoints, numCells;
  vtkDataArray* array;
  int arrayCount = 0;
  char name[256]; memset( name, 0, sizeof(name) );
  int ptIdx;
  vtkAppendPolyData* append = NULL;

  if (this->Importer == NULL)
    {
    return;
    }

  if (this->Append)
    {
    append = vtkAppendPolyData::New();
    }

  ren = this->Importer->GetRenderer();
  actors = ren->GetActors();
  actors->InitTraversal();
  idx = 0;
  while ( (actor = actors->GetNextActor()) )
    {
    mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
    if (mapper)
      {
      input = mapper->GetInput();
      input->Update();

      if (append)
        {
        output = vtkPolyData::New();
        }
      else
        {
        output = this->GetOutput(idx);
        }

      // (1) copy vertices and faces of geometric structure!!!
      // copy transform and copy structure
      vtkTransformPolyDataFilter *tf = vtkTransformPolyDataFilter::New();
      vtkTransform *trans = vtkTransform::New();
      tf->SetInput(input);
      tf->SetTransform(trans);
      trans->SetMatrix(actor->GetMatrix());
      input = tf->GetOutput();
      input->Update();
      output->CopyStructure(input);

      // (2) this copies texture coordinates.
      // (vertex) Only copy well formed arrays.
      numPoints = input->GetNumberOfPoints();
      numArrays = input->GetPointData()->GetNumberOfArrays();
#ifdef _DEBUG
      cerr << "numPoints: " << numPoints << endl;
      cerr << "numArrays: " << numArrays << endl;
#endif
      for ( arrayIdx = 0; arrayIdx < numArrays; ++arrayIdx)
        {
          // MK: here the first array is vtkFloatArray that includes texture coordinates.
        array = input->GetPointData()->GetTCoords(); // MK this one is correct
        //array = input->GetPointData()->GetArray(arrayIdx); // Dont' use this! our IBM vrml didn't work
        if (array->GetNumberOfTuples() == numPoints)
          {
          if (array->GetName() == NULL)
            {
            sprintf(name, "VRMLArray%d", ++arrayCount);
            array->SetName(name);
            }

          if (arrayIdx==0)
            output->GetPointData()->SetTCoords( array ); // IMPORTANT: extra copy for texture coordinates to the VTK structure!

          output->GetPointData()->AddArray(array); // OLD
          }
        }


      // (3) copying cells (but generally, zero)
      // (face) Only copy well formed arrays.
      numCells = input->GetNumberOfCells();
      numArrays = input->GetCellData()->GetNumberOfArrays();
#ifdef _DEBUG
      cerr << "numPoints: " << numPoints << endl;
      cerr << "numArrays: " << numArrays << endl;
#endif
      for ( arrayIdx = 0; arrayIdx < numArrays; ++arrayIdx)
        {
        array = input->GetCellData()->GetArray(arrayIdx);
        if (array->GetNumberOfTuples() == numCells)
          {
          if (array->GetName() == NULL)
            {
            sprintf(name, "VRMLArray%d", ++arrayCount);
            array->SetName(name);
            }
          output->GetCellData()->AddArray(array);
          } 
        }

      //---------------------------------------------------------------
      // MK: copying (texture coordinates)
/*
 * example of VRML Structure
 *
#VRML V2.0 utf8

# Generated by someone

NavigationInfo {
  type [ "EXAMINE", "ANY" ]
}
Transform {
  scale 1 1 1
  translation 0 0 0
  children
  [
    Shape
    {
      geometry IndexedFaceSet
      {
        creaseAngle .5
        solid FALSE
        coord Coordinate
        {
          point
          [
          ...
          ]
          }
      texCoord TextureCoordinate
      {
        point
        [

        0.911133 0.62793 0.911133 0.612305 0.926758 0.62793 0.829102 0.319336 0.829102 0.334961 0.813477
        ...
          ]
        }
        texCoordIndex
        [
        ...
        ]
        coordIndex
        [
        ...
        ]
      }
      appearance Appearance
      {
        material Material
        {
         ambientIntensity 0.2
         diffuseColor 0.9 0.9 0.9
         specularColor .1 .1 .1
         shininess .5
        }
        texture ImageTexture { url "totem5_T5k1.jpg" }
      }
    }
  ]
}
*/
      //---------------------------------------------------------------

      if (this->Color)
        {
        vtkUnsignedCharArray *colorArray = vtkUnsignedCharArray::New();
        unsigned char r, g, b;
        double* actorColor;
     
        actorColor = actor->GetProperty()->GetColor();
        r = static_cast<unsigned char>(actorColor[0]*255.0);
        g = static_cast<unsigned char>(actorColor[1]*255.0);
        b = static_cast<unsigned char>(actorColor[2]*255.0);
        colorArray->SetName("VRMLColor");
        colorArray->SetNumberOfComponents(3);
        for (ptIdx = 0; ptIdx < numPoints; ++ptIdx)
          {
          colorArray->InsertNextValue(r);
          colorArray->InsertNextValue(g);
          colorArray->InsertNextValue(b);
          }
        output->GetPointData()->SetScalars(colorArray);
        colorArray->Delete();
        colorArray = NULL;
        }
      if (append)
        {
        append->AddInput(output);
        output->Delete();
        output = NULL;
        }

      ++idx;
      tf->Delete();
      tf = NULL;
      trans->Delete();
      trans = NULL;
      }
    }

  if (append)
    {
    output = this->GetOutput();
    append->Update();
    output->ShallowCopy(append->GetOutput());
    append->Delete();
    }
}

void vtkVRMLSource2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if (this->FileName)
    {
    os << indent << "FileName: " << this->FileName << endl;
    }
  os << indent << "Color: " << this->Color << endl;
  os << indent << "Append: " << this->Append << endl;
}

