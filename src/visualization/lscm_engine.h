/****************************************************************************

 - Codename: CHER-Ob (Yale Computer Graphics Group)

 - Writers:  Ying Yang (ying.yang.yy368@yale.edu)

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

 - Acknowledgments: This EXR file reader is based on the original code of
   exrreadchannel for MATLAB by Edgar Velazquez-Armendariz.

*****************************************************************************/
#ifndef LSCM_ENGINE_H
#define LSCM_ENGINE_H

#include "../NL/nl.h"
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>


/*************************************************************************************/
/* Basic functions */

template <class T> inline T nl_min(T x, T y) { return x < y ? x : y ; }
template <class T> inline T nl_max(T x, T y) { return x > y ? x : y ; }

/*************************************************************************************/
/* Basic geometric types */

class Vector2 {
public:
    Vector2(double x_in, double y_in) : x(x_in), y(y_in) { }
    Vector2() : x(0), y(0) { }

    Vector2 operator+(const Vector2& v2) {
        return Vector2(x + v2.x,		y + v2.y ) ;
    }

    Vector2 operator-(const Vector2& v2) {
        return Vector2( x - v2.x, y - v2.y ) ;
    }

    double x ;
    double y ;
} ;

class Vector3 {
public:
    Vector3(double x_in, double y_in, double z_in) : x(x_in), y(y_in), z(z_in) { }
    Vector3() : x(0), y(0), z(0) { }
    double length() const {
        return sqrt(x*x + y*y + z*z) ;
    }
    void normalize() {
        double l = length() ;
        x /= l ; y /= l ; z /= l ;
    }

    /* Dot Product */
    double operator*(const Vector3& v2){
        return x * v2.x + y * v2.y + z * v2.z ;
    }

    /* cross product */
    Vector3 operator^(const Vector3& v2){
        return Vector3(y*v2.z - v2.y*z,	z*v2.x - v2.z*x,	x*v2.y - v2.x*y ) ;
    }

    Vector3 operator+(const Vector3& v2){
        return Vector3(x + v2.x,		y + v2.y,	z + v2.z ) ;
    }

    Vector3 operator-(const Vector3& v2){
        return Vector3(x - v2.x,		y - v2.y,	z - v2.z ) ;
    }

    double x ;
    double y ;
    double z ;
} ;


/***********************************************************************************/
/* Mesh class */

// Note1: this is a minimum mesh class, it does not have facet adjacency information
// (we do not need it for LSCM). This is just like an Inventor indexed face set.
//
// Note2: load() and save() use Alias|Wavefront .obj file format

class vtkPolyData;

class Vertex {
public:
    Vertex() : locked(false), id(-1) { }
    Vertex( const Vector3& p, const Vector2& t ) : point(p), tex_coord(t), locked(false), id(-1) { }

    Vector3 point ;
    Vector2 tex_coord ;
    bool    locked ;
    int     id ;
} ;

class Facet : public std::vector<int> {
public:
} ;

class IndexedMesh {
public:
    IndexedMesh() : in_facet(false) { }

    Vertex* add_vertex();

    Vertex* add_vertex(const Vector3& p, const Vector2& t);

    void begin_facet();

    void end_facet();

    void add_vertex_to_facet(unsigned int i);

    void clear();

    std::vector<Vertex> vertex ;
    std::vector<Facet>  facet ;
    bool in_facet ;
} ;

class LSCM {
public:

    LSCM();

    // Outline of the algorithm:
    // 1) Find an initial solution by projecting on a plane
    // 2) Lock two vertices of the mesh
    // 3) Copy the initial u,v coordinates to OpenNL
    // 3) Construct the LSCM equation with OpenNL
    // 4) Solve the equation with OpenNL
    // 5) Copy OpenNL solution to the u,v coordinates

    void FlattenMesh(vtkPolyData *qm);
    void apply();
    void SwapUVtoGeometry();

protected:

    void setup_lscm();

    // Note: no-need to triangulate the facet,
    // we can do that "virtually", by creating triangles
    // radiating around vertex 0 of the facet.
    // (however, this may be invalid for concave facets)
    void setup_lscm(const Facet& F);

    // Computes the coordinates of the vertices of a triangle
    // in a local 2D orthonormal basis of the triangle's plane.
    static void project_triangle(Vector3& p0, Vector3& p1, Vector3& p2, Vector2& z0, Vector2& z1, Vector2& z2)
    {
        Vector3 X = p1 - p0 ;
        X.normalize() ;
        Vector3 Z = X ^ (p2 - p0) ;
        Z.normalize() ;
        Vector3 Y = Z ^ X ;
        const Vector3& O = p0 ;

        double x0 = 0 ;
        double y0 = 0 ;
        double x1 = (p1 - O).length() ;
        double y1 = 0 ;
        double x2 = (p2 - O) * X ;
        double y2 = (p2 - O) * Y ;

        z0 = Vector2(x0,y0) ;
        z1 = Vector2(x1,y1) ;
        z2 = Vector2(x2,y2) ;
    }

    // LSCM equation, geometric form :
    // (Z1 - Z0)(U2 - U0) = (Z2 - Z0)(U1 - U0)
    // Where Uk = uk + i.vk is the complex number
    //                       corresponding to (u,v) coords
    //       Zk = xk + i.yk is the complex number
    //                       corresponding to local (x,y) coords
    // cool: no divide with this expression,
    //  makes it more numerically stable in
    //  the presence of degenerate triangles.
    void setup_conformal_map_relations(Vertex& v0, Vertex& v1, Vertex& v2);

    // copies u,v coordinates from OpenNL solver to the mesh.
    void solver_to_mesh();

    // copies u,v coordinates from the mesh to OpenNL solver.
    void mesh_to_solver();

    // Chooses an initial solution, and locks two vertices
    void project();

    // Convert between MyQMesh and IndexedMesh
    IndexedMesh* QMesh2IndMesh(vtkPolyData *qm);
    //MyQMesh* IndMesh2QMesh(IndexedMesh *im);
    void TransferFlatGeometryToQMesh(vtkPolyData *qm);

    const char *type_solver;
    IndexedMesh *mesh;
    vtkPolyData *vtkMesh;
} ;

#endif // LSCM_ENGINE_H
