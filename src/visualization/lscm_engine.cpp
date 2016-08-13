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
#include "lscm_engine.h"
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkIdList.h>
//#include <time.h>
#include <qdatetime.h>
#include <qdebug.h>


#define MAX_ITERATIONS 2000

// I/O

std::ostream& operator<<(std::ostream& out, const Vector2& v) {
    return out << v.x << " " << v.y ;
}

std::ostream& operator<<(std::ostream& out, const Vector3& v) {
    return out << v.x << " " << v.y << " " << v.z ;
}

std::istream& operator>>(std::istream& in, Vector2& v) {
    return in >> v.x >> v.y ;
}

std::istream& operator>>(std::istream& in, Vector3& v) {
    return in >> v.x >> v.y >> v.z ;
}


Vertex* IndexedMesh::add_vertex() {
    vertex.push_back(Vertex()) ;
    vertex.rbegin()->id = vertex.size() - 1 ;
    return &*(vertex.rbegin()) ;
}

Vertex* IndexedMesh::add_vertex(const Vector3& p, const Vector2& t) {
    vertex.push_back(Vertex(p,t)) ;
    vertex.rbegin()->id = vertex.size() - 1 ;
    return &*(vertex.rbegin()) ;
}

void IndexedMesh::begin_facet() {
    assert(!in_facet) ;
    facet.push_back(Facet()) ;
    in_facet = true ;
}

void IndexedMesh::end_facet() {
    assert(in_facet) ;
    in_facet = false ;
}

void IndexedMesh::add_vertex_to_facet(unsigned int i) {
    assert(in_facet) ;
    assert(i < vertex.size()) ;
    facet.rbegin()->push_back(i) ;
}

void IndexedMesh::clear() {
    vertex.clear() ;
    facet.clear() ;
}

LSCM::LSCM()
{
    //type_solver = "SUPERLU";
    type_solver = "BICGSTAB";
    mesh = 0;
    vtkMesh = 0;
}


void LSCM::apply()
{
    int nb_vertices = mesh->vertex.size() ;
    project() ;
    nlNewContext() ;

    if (!strcmp(type_solver,"CG")) {
            nlSolverParameteri(NL_SOLVER, NL_CG) ;
            nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
        }
    else if (!strcmp(type_solver,"BICGSTAB")) {
            nlSolverParameteri(NL_SOLVER, NL_BICGSTAB) ;
            nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
        }
    else if (!strcmp(type_solver,"GMRES")) {
            nlSolverParameteri(NL_SOLVER, NL_GMRES) ;
        }
    else if (!strcmp(type_solver,"SUPERLU")) {
        if(nlInitExtension("SUPERLU")) {
            nlSolverParameteri(NL_SOLVER, NL_PERM_SUPERLU_EXT) ;
        } else {
            std::cerr << "OpenNL has not been compiled with SuperLU support." << std::endl;
            exit(-1);
        }
    }else if(
        !strcmp(type_solver,"FLOAT_CRS")
        || !strcmp(type_solver,"DOUBLE_CRS")
        || !strcmp(type_solver,"FLOAT_BCRS2")
        || !strcmp(type_solver,"DOUBLE_BCRS2")
        || !strcmp(type_solver,"FLOAT_ELL")
        || !strcmp(type_solver,"DOUBLE_ELL")
        || !strcmp(type_solver,"FLOAT_HYB")
        || !strcmp(type_solver,"DOUBLE_HYB")
        ) {
        if(nlInitExtension("CNC")) {
            if (!strcmp(type_solver,"FLOAT_CRS")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_FLOAT_CRS) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }
            else if (!strcmp(type_solver,"DOUBLE_CRS")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_DOUBLE_CRS) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }
            else if (!strcmp(type_solver,"FLOAT_BCRS2")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_FLOAT_BCRS2) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }
            else if (!strcmp(type_solver,"DOUBLE_BCRS2")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_DOUBLE_BCRS2) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }
            else if (!strcmp(type_solver,"FLOAT_ELL")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_FLOAT_ELL) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }
            else if (!strcmp(type_solver,"DOUBLE_ELL")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_DOUBLE_ELL) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }
            else if (!strcmp(type_solver,"FLOAT_HYB")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_FLOAT_HYB) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }
            else if (!strcmp(type_solver,"DOUBLE_HYB")) {
                nlSolverParameteri(NL_SOLVER, NL_CNC_DOUBLE_HYB) ;
                nlSolverParameteri(NL_PRECONDITIONER, NL_PRECOND_JACOBI) ;
            }

        } else {
            std::cerr << "OpenNL has not been compiled with CNC support." << std::endl;
            exit(-1);
        }
    } else {
        std::cerr << "type_solver must belong to { CG | BICGSTAB | GMRES | "
                    << "SUPERLU | FLOAT_CRS | FLOAT_BCRS2 | DOUBLE_CRS | "
                    << "DOUBLE_BCRS2 | FLOAT_ELL | DOUBLE_ELL | FLOAT_HYB |"
                    << "DOUBLE_HYB } "
                    << std::endl;
        exit(-1);
    }

    nlSolverParameteri(NL_NB_VARIABLES, 2*nb_vertices) ;
    nlSolverParameteri(NL_LEAST_SQUARES, NL_TRUE) ;
    //nlSolverParameteri(NL_MAX_ITERATIONS, 5*nb_vertices) ;
    nlSolverParameteri(NL_MAX_ITERATIONS, MAX_ITERATIONS);
    nlSolverParameterd(NL_THRESHOLD, 1e-10) ;
    nlBegin(NL_SYSTEM) ;
    mesh_to_solver() ;
    nlBegin(NL_MATRIX) ;
    setup_lscm() ;
    nlEnd(NL_MATRIX) ;
    nlEnd(NL_SYSTEM) ;
    //std::cout << "Solving ..." << std::endl ;
    bool nl_result = nlSolve() ;
    //std::cout << "Open_NL result: " << nl_result << std::endl ;
    if(nl_result)
    {
        solver_to_mesh() ;
        double time ;
        NLint iterations;
        nlGetDoublev(NL_ELAPSED_TIME, &time) ;
        nlGetIntergerv(NL_USED_ITERATIONS, &iterations);
        std::cout << "flattening succeeded" << std::endl ;
    }
    else
    {
        std::cout << "flattening failed" << std::endl ;
    }
    nlDeleteContext(nlGetCurrent()) ;
}

void LSCM::SwapUVtoGeometry()
{
    for(unsigned int i=0; i < mesh->vertex.size(); i++)
    {
        double u = mesh->vertex[i].tex_coord.x;
        double v = mesh->vertex[i].tex_coord.y;
        mesh->vertex[i].point.x = u;
        mesh->vertex[i].point.y = v;
        mesh->vertex[i].point.z = 0;
    }
}

void LSCM::setup_lscm()
{
    for(unsigned int i=0; i<mesh->facet.size(); i++) {
        const Facet& F = mesh->facet[i] ;
        setup_lscm(F) ;
    }
}

void LSCM::setup_lscm(const Facet& F)
{
    for(unsigned int i=1; i < F.size() - 1; i++) {
        setup_conformal_map_relations(
            mesh->vertex[F[0]], mesh->vertex[F[i]], mesh->vertex[F[i+1]]
        ) ;
    }
}

void LSCM::setup_conformal_map_relations(Vertex& v0, Vertex& v1, Vertex& v2)
{
    int id0 = v0.id ;
    int id1 = v1.id ;
    int id2 = v2.id ;

    Vector3& p0 = v0.point ;
    Vector3& p1 = v1.point ;
    Vector3& p2 = v2.point ;

    Vector2 z0,z1,z2 ;
    project_triangle(p0,p1,p2,z0,z1,z2) ;
    Vector2 z01 = z1 - z0 ;
    Vector2 z02 = z2 - z0 ;
    double a = z01.x ;
    double b = z01.y ;
    double c = z02.x ;
    double d = z02.y ;
    assert(b == 0.0) ;

    // Note  : 2*id + 0 --> u
    //         2*id + 1 --> v
    int u0_id = 2*id0     ;
    int v0_id = 2*id0 + 1 ;
    int u1_id = 2*id1     ;
    int v1_id = 2*id1 + 1 ;
    int u2_id = 2*id2     ;
    int v2_id = 2*id2 + 1 ;

    // Note : b = 0

    // Real part
    nlBegin(NL_ROW) ;
    nlCoefficient(u0_id, -a+c)  ;
    nlCoefficient(v0_id,  b-d)  ;
    nlCoefficient(u1_id,   -c)  ;
    nlCoefficient(v1_id,    d)  ;
    nlCoefficient(u2_id,    a) ;
    nlEnd(NL_ROW) ;

    // Imaginary part
    nlBegin(NL_ROW) ;
    nlCoefficient(u0_id, -b+d) ;
    nlCoefficient(v0_id, -a+c) ;
    nlCoefficient(u1_id,   -d) ;
    nlCoefficient(v1_id,   -c) ;
    nlCoefficient(v2_id,    a) ;
    nlEnd(NL_ROW) ;
}

void LSCM::solver_to_mesh()
{
    for(unsigned int i=0; i<mesh->vertex.size(); i++) {
        Vertex& it = mesh->vertex[i] ;
        double u = nlGetVariable(2 * it.id    ) ;
        double v = nlGetVariable(2 * it.id + 1) ;
        it.tex_coord = Vector2(u,v) ;
    }
}

void LSCM::mesh_to_solver()
{
    for(unsigned int i=0; i<mesh->vertex.size(); i++) {
        Vertex& it = mesh->vertex[i] ;
        double u = it.tex_coord.x ;
        double v = it.tex_coord.y ;
        nlSetVariable(2 * it.id    , u) ;
        nlSetVariable(2 * it.id + 1, v) ;
        if(it.locked) {
            nlLockVariable(2 * it.id    ) ;
            nlLockVariable(2 * it.id + 1) ;
        }
    }
}


void LSCM::project()
{
    // Get bbox
    unsigned int i ;

    double xmin =  1e30 ;
    double ymin =  1e30 ;
    double zmin =  1e30 ;
    double xmax = -1e30 ;
    double ymax = -1e30 ;
    double zmax = -1e30 ;

    for(i=0; i<mesh->vertex.size(); i++) {
        const Vertex& v = mesh->vertex[i] ;
        xmin = nl_min(v.point.x, xmin) ;
        ymin = nl_min(v.point.y, xmin) ;
        zmin = nl_min(v.point.z, xmin) ;

        xmax = nl_max(v.point.x, xmin) ;
        ymax = nl_max(v.point.y, xmin) ;
        zmax = nl_max(v.point.z, xmin) ;
    }

    double dx = xmax - xmin ;
    double dy = ymax - ymin ;
    double dz = zmax - zmin ;

    Vector3 V1,V2 ;

    // Find shortest bbox axis
    if(dx < dy && dx < dz) {
        if(dy > dz) {
            V1 = Vector3(0,1,0) ;
            V2 = Vector3(0,0,1) ;
        } else {
            V2 = Vector3(0,1,0) ;
            V1 = Vector3(0,0,1) ;
        }
    } else if(dy < dx && dy < dz) {
        if(dx > dz) {
            V1 = Vector3(1,0,0) ;
            V2 = Vector3(0,0,1) ;
        } else {
            V2 = Vector3(1,0,0) ;
            V1 = Vector3(0,0,1) ;
        }
    } else if(dz < dx && dz < dy) {
        if(dx > dy) {
            V1 = Vector3(1,0,0) ;
            V2 = Vector3(0,1,0) ;
        } else {
            V2 = Vector3(1,0,0) ;
            V1 = Vector3(0,1,0) ;
        }
    }

    // Project onto shortest bbox axis,
    // and lock extrema vertices

    Vertex* vxmin = NULL ;
    double  umin = 1e30 ;
    Vertex* vxmax = NULL ;
    double  umax = -1e30 ;

    for(i=0; i<mesh->vertex.size(); i++) {
        Vertex& V = mesh->vertex[i] ;
        double u = V.point * V1 ;
        double v = V.point * V2 ;
        V.tex_coord = Vector2(u,v) ;
        if(u < umin) {
            vxmin = &V ;
            umin = u ;
        }
        if(u > umax) {
            vxmax = &V ;
            umax = u ;
        }
    }

    vxmin->locked = true ;
    vxmax->locked = true ;
}

IndexedMesh* LSCM::QMesh2IndMesh(vtkPolyData *qm)
{
    if(mesh != NULL)
    {
        delete mesh;
        mesh = 0;
    }

    mesh = new IndexedMesh();

    int numberOfCells = qm->GetNumberOfCells();
    vtkPoints *qPoints = qm->GetPoints();
    int numberOfPoints = qm->GetPoints()->GetNumberOfPoints();
    for(int i = 0; i < numberOfPoints; ++i)
    {
        double pi[3];
        qm->GetPoint(i,pi);
        mesh->add_vertex(Vector3(pi[0],pi[1],pi[2]),Vector2(0,0));
    }

    for(int k = 0; k < numberOfCells; ++k)
    {
        vtkSmartPointer<vtkIdList> vertexList = vtkSmartPointer<vtkIdList>::New();
        qm->GetCellPoints(k,vertexList);
        int v0 = vertexList->GetId(0);
        int v1 = vertexList->GetId(1);
        int v2 = vertexList->GetId(2);
        mesh->begin_facet();
        mesh->add_vertex_to_facet(v0);
        mesh->add_vertex_to_facet(v1);
        mesh->add_vertex_to_facet(v2);
        mesh->end_facet();
    }

    return mesh;
}

/*
MyQMesh* LSCM::IndMesh2QMesh(IndexedMesh *im)
{
    return new MyQMesh();
}
*/

void LSCM::TransferFlatGeometryToQMesh(vtkPolyData *qm)
{
    for(int i = 0; i < mesh->vertex.size(); ++i)
    {
        double u = mesh->vertex[i].tex_coord.x;
        double v = mesh->vertex[i].tex_coord.y;
        qm->GetPoints()->SetPoint(i,u,v,0.0);
    }
}

void LSCM::FlattenMesh(vtkPolyData *qm)
{
    QTime time1;
    time1.start();
    QMesh2IndMesh(qm);
    apply();
    TransferFlatGeometryToQMesh(qm);
    delete mesh;
    mesh = 0;
    double nMs = (double)time1.elapsed();
    double nSeconds = nMs / 1000;
    qDebug() << "Flattening took " << nSeconds << " seconds. Flattened mesh has : " << qm->GetNumberOfPoints() << " vertices \n";
}
