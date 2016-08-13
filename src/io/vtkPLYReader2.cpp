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

 - Acknowledgments: This PLY file reader is based on the original code of
   vtkPLYReader of VTK library from Kitware. The difference is that this reader
   can read texture coordinates as well as vertexes and face indices.
   This function is only valid when the number of texture coordinates is
   same as that of vertexes.

*****************************************************************************/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPLYReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#if defined(_WIN32) || defined(_WIN64)
  #define snprintf _snprintf
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
#endif

#include "vtkPLYReader2.h"

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPLY.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSortDataArray.h>

#include <ctype.h>
#include <stddef.h>

vtkStandardNewMacro(vtkPLYReader2)


// Construct object with merging set to true.
vtkPLYReader2::vtkPLYReader2()
{
  this->FileName = NULL;

  this->SetNumberOfInputPorts(0);
}

vtkPLYReader2::~vtkPLYReader2()
{
  if (this->FileName)
    {
    delete [] this->FileName;
    }
}

typedef struct _plyVertex {
  float x[3];             // the usual 3-space position of a vertex
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} plyVertex;

typedef struct _plyFace {
  unsigned char intensity; // optional face attributes
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char nverts;   // number of vertex indices in list (3 for triangle)
  int *verts;             // vertex index list
  unsigned char ntcoords;
  float *tcoords; //MK: texture coordinates (number of tcoords (two column vectors) is same as number of vertexes)
} plyFace;

int vtkPLYReader2::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkSmartPointer<vtkInformation> outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkSmartPointer<vtkPolyData>output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

//----------------------------------------------------------------------------
// structure
//  const char *name;                     /* property name */
//  int external_type;                    /* file's data type */
//  int internal_type;                    /* program's data type */
//  int offset;                           /* offset bytes of prop in a struct */

//  int is_list;                          /* 1 = list, 0 = scalar */
//  int count_external;                   /* file's count type */
//  int count_internal;                   /* program's count type */
//  int count_offset;                     /* offset byte for list count */
//----------------------------------------------------------------------------
  PlyProperty vertProps[] = {
    {"x", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex,x)),
     0, 0, 0, 0},
    {"y", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex,x)+sizeof(float)),
     0, 0, 0, 0},
    {"z", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex,x)+sizeof(float)+sizeof(float)),
     0, 0, 0, 0},
    {"red", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyVertex,red)), 0, 0, 0, 0},
    {"green", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyVertex,green)), 0, 0, 0, 0},
    {"blue", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyVertex,blue)), 0, 0, 0, 0},
  };

  PlyProperty faceProps[] = {
    {"vertex_indices", PLY_INT, PLY_INT, static_cast<int>(offsetof(plyFace,verts)), 1, PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace,nverts))},
    {"intensity", PLY_UCHAR, PLY_UCHAR, 
     static_cast<int>(offsetof(plyFace,intensity)), 0, 0, 0, 0},
    {"red", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace,red)), 0, 0, 0, 0},
    {"green", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace,green)), 0, 0, 0, 0},
    {"blue", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace,blue)), 0, 0, 0, 0},
    {"texcoord", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyFace,tcoords)), 1, PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace,ntcoords))},// MK: texture coordinates
  };

  int i, j, k;
  int numPts=0, numPolys=0;

  if (!this->FileName)
    {
    return 0;
    }

  // open a PLY file for reading
  PlyFile *ply;
  int nelems, fileType, numElems, nprops;
  char **elist, *elemName;
  float version;
  
  if ( !(ply = vtkPLY::ply_open_for_reading(this->FileName, &nelems, &elist, 
                                            &fileType, &version)) )
    {
    return 0;
    }

  // Check to make sure that we can read geometry
  PlyElement *elem;
  int index;
  if ( (elem = vtkPLY::find_element (ply, "vertex")) == NULL || // OK
       vtkPLY::find_property (elem, "x", &index) == NULL || // OK
       vtkPLY::find_property (elem, "y", &index) == NULL || // OK
       vtkPLY::find_property (elem, "z", &index) == NULL || // OK
//       ( (elem = vtkPLY::find_element (ply, "face")) == NULL && // either one
//         (elem = vtkPLY::find_element (ply, "range_grid")) == NULL ) ||
       ( (elem = vtkPLY::find_element (ply, "face")) == NULL ) ||
       vtkPLY::find_property (elem, "vertex_indices", &index) == NULL ) // Problem
    {
    vtkPLY::ply_close (ply); // stanford bunny crash
    return 0;
    }

  // Check for optional attribute data. We can handle intensity; and the
  // triplet red, green, blue.
  unsigned char intensityAvailable=false, RGBCellsAvailable=false, RGBPointsAvailable=false;
  unsigned char texcoordAvailable=false;

  vtkSmartPointer<vtkPoints>points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray>polys = vtkSmartPointer<vtkCellArray>::New();

  // optionals
  vtkSmartPointer<vtkFloatArray> texcoord = NULL;
  vtkSmartPointer<vtkFloatArray> texcoord_new = NULL;
  vtkSmartPointer<vtkUnsignedCharArray> intensity = NULL;
  vtkSmartPointer<vtkUnsignedCharArray> RGBCells = NULL;
  vtkSmartPointer<vtkUnsignedCharArray> RGBPoints = NULL;

  vtkSmartPointer<vtkPoints> new_points = NULL;
  vtkSmartPointer<vtkCellArray> new_polys = NULL;
  vtkSmartPointer<vtkFloatArray> new_texcoord = NULL;


  if ( ( (elem = vtkPLY::find_element (ply, "face")) != NULL ||
         (elem = vtkPLY::find_element (ply, "range_grid")) != NULL ) &&
       vtkPLY::find_property (elem, "intensity", &index) != NULL )
    {
    intensity = vtkSmartPointer<vtkUnsignedCharArray>::New();
    intensity->SetName("intensity");
    intensityAvailable = true;
    output->GetCellData()->AddArray(intensity);
    output->GetCellData()->SetActiveScalars("intensity");
    }

  if ( ( (elem = vtkPLY::find_element (ply, "face")) != NULL ||
         (elem = vtkPLY::find_element (ply, "range_grid")) != NULL ) &&
       vtkPLY::find_property (elem, "red", &index) != NULL &&
       vtkPLY::find_property (elem, "green", &index) != NULL &&
       vtkPLY::find_property (elem, "blue", &index) != NULL )
    {
    RGBCells = vtkSmartPointer<vtkUnsignedCharArray>::New();
    RGBCells->SetName("RGB");
    RGBCellsAvailable = true;
    output->GetCellData()->AddArray(RGBCells);
    output->GetCellData()->SetActiveScalars("RGB");
    }

  if ( (elem = vtkPLY::find_element (ply, "vertex")) != NULL &&
       vtkPLY::find_property (elem, "red", &index) != NULL &&
       vtkPLY::find_property (elem, "green", &index) != NULL &&
       vtkPLY::find_property (elem, "blue", &index) != NULL )
    {
    RGBPoints = vtkSmartPointer<vtkUnsignedCharArray>::New();
    RGBPointsAvailable = true;
    RGBPoints->SetName("RGB");
    output->GetPointData()->SetScalars(RGBPoints);
    }

  //===========================================================
  // assign the texture coordinate point data to the reader structure
  if ( ( (elem = vtkPLY::find_element (ply, "face")) != NULL ||
         (elem = vtkPLY::find_element (ply, "range_grid")) != NULL ) &&
       vtkPLY::find_property(elem, "texcoord", &index) != NULL ) //MK: texture coordinates
    {
    texcoord = vtkSmartPointer<vtkFloatArray>::New(); // texture coordinates
    texcoord_new = vtkSmartPointer<vtkFloatArray>::New(); // texture coordinates
    texcoordAvailable = true;
    texcoord->SetName("texcoord");
    texcoord_new->SetName("texcoord");
//    output->GetPointData()->SetTCoords(texcoord);
    output->GetPointData()->SetTCoords(texcoord_new);
    output->Squeeze();
    }
  //===========================================================


  // Okay, now we can grab the data
  for (i = 0; i < nelems; i++)  // nelems = 2
    {
    //get the description of the first element */
    elemName = elist[i];
    vtkPLY::ply_get_element_description (ply, elemName, &numElems, &nprops);

    polys->Allocate(polys->EstimateSize(numPolys,3),numPolys/2);

    //------------------------------------------------------------------------------------------
    // if we're on vertex elements, read them in
    if ( elemName && !strcasecmp ("vertex", elemName) )
      {
        //MK: for stanford bunny
        if (numElems == 0)
            break;

      // Create a list of points
      numPts = numElems;
      points->SetDataTypeToFloat();
      points->SetNumberOfPoints(numPts);
      
      // Setup to read the PLY elements
      vtkPLY::ply_get_property (ply, elemName, &vertProps[0]);
      vtkPLY::ply_get_property (ply, elemName, &vertProps[1]);
      vtkPLY::ply_get_property (ply, elemName, &vertProps[2]);


      if ( RGBPointsAvailable )
        {
        vtkPLY::ply_get_property (ply, elemName, &vertProps[3]);
        vtkPLY::ply_get_property (ply, elemName, &vertProps[4]);
        vtkPLY::ply_get_property (ply, elemName, &vertProps[5]);
        RGBPoints->SetNumberOfComponents(3);
        RGBPoints->SetNumberOfTuples(numPts);
        }

      plyVertex vertex;
      for (j=0; j < numPts; j++) 
        {
        vtkPLY::ply_get_element (ply, (void *) &vertex);
        points->SetPoint (j, vertex.x);
        if ( RGBPointsAvailable )
          {
          RGBPoints->SetTuple3(j,vertex.red,vertex.green,vertex.blue);
          }
        }

      output->SetPoints(points);

      }//if vertex

    //------------------------------------------------------------------------------------------
    else if ( elemName && ( !strcasecmp ("face", elemName)  || !strcasecmp ("range_grid", elemName)) )
      {
      // Create a polygonal array
      numPolys = numElems;
//      cerr << elemName;

      plyFace face;
      int verts[256];
      float tCoords[256];
      vtkIdType vtkVerts[256];

      // Get the face properties
      vtkPLY::ply_get_property (ply, elemName, &faceProps[0]);

      if ( intensityAvailable )
        {
        vtkPLY::ply_get_property (ply, elemName, &faceProps[1]);
        RGBCells->SetNumberOfComponents(1);
        RGBCells->SetNumberOfTuples(numPolys);
        }
      if ( RGBCellsAvailable )
        {
        vtkPLY::ply_get_property (ply, elemName, &faceProps[2]);
        vtkPLY::ply_get_property (ply, elemName, &faceProps[3]);
        vtkPLY::ply_get_property (ply, elemName, &faceProps[4]);
        RGBCells->SetNumberOfComponents(3);
        RGBCells->SetNumberOfTuples(numPolys);
        }

      if ( texcoordAvailable )
      {
        vtkPLY::ply_get_property (ply, elemName, &faceProps[5]);
        texcoord->SetNumberOfComponents(6);
        texcoord->SetNumberOfTuples(numPolys);
      }

      polys->InitTraversal();
      // grab all the face elements
      for (j=0; j < numPolys; j++) 
        {
        //grab and element from the file
        face.verts = verts;
        face.tcoords = tCoords; //MK for texture
        vtkPLY::ply_get_element (ply, (void *) &face);

        for (k=0; k<face.nverts; k++)
          {
          vtkVerts[k] = face.verts[k];
          }
        polys->InsertNextCell(face.nverts,vtkVerts);

        if ( texcoordAvailable )
        {
          texcoord->SetTupleValue(j,face.tcoords); // 6 coordinates per polygon
        }

        if ( intensityAvailable )
          {
          intensity->SetValue(j,face.intensity);
          }
        if ( RGBCellsAvailable )
          {
          RGBCells->SetValue(3*j,face.red);
          RGBCells->SetValue(3*j+1,face.green);
          RGBCells->SetValue(3*j+2,face.blue);
          }
        } // end of "for-roof" of face

      //=====================================================================
      // resorting the texture coordinates with vertex coordinates in faces
      if ( texcoordAvailable )
      {
        vtkIdType dummy_warning_prevention_mechanism[1];
        vtkIdType n_pts = -1;
        vtkIdType *pts = dummy_warning_prevention_mechanism;
        // reshape the matrix
        texcoord_new->SetNumberOfComponents(2);
        texcoord_new->SetNumberOfTuples(texcoord->GetNumberOfTuples()); // number of vertex

        float uvall[256];
        float *puvall;

        for (int i=0; i<numPolys; ++i)
          {
            // read the vertex index
          polys->GetNextCell(n_pts,pts);

          // read the t coords of six per polygon

          puvall = uvall;
          texcoord->GetTupleValue(i, puvall);

          for (int j=0; j<n_pts; ++j) // pts[j] = 1/3/4
            {
              // enter the new reading two-by-two // uvall[] = 0.2, 0.9, 0.1, 0.2, 0.5, 0.7
              float tcoord_new[2];
              tcoord_new[0] = *(puvall++);
              tcoord_new[1] = *(puvall++);
              texcoord_new->InsertTuple(pts[j], tcoord_new);
            } // for
        }
      }

      }//if face
    output->SetPolys(polys);
    output->Squeeze();

    free(elist[i]); //allocated by ply_open_for_reading
    }//for all elements of the PLY file
  free(elist); //allocated by ply_open_for_reading

  // close the PLY file 
  if (ply->fp != NULL && ply->nelems != 0)
    vtkPLY::ply_close (ply);
  else
      return 0;

  return 1;
}

int vtkPLYReader2::CanReadFile(const char *filename)
{
  FILE *fd = fopen(filename, "rb");
  if (!fd) return 0;

  char line[16]; memset( line, 0, sizeof(line) );
  fgets(line, 16, fd);
  fclose(fd);
  return (strncmp(line, "ply", 3) == 0);
}

void vtkPLYReader2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: " 
     << (this->FileName ? this->FileName : "(none)") << "\n";

}
