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
/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/


#pragma once

//#include <emmintrinsics.h>
#include <QDebug>

#include <emmintrin.h>

#define ALIGN_SIZE 32

#if (_MSC_VER == 0)
        #define __forceinline inline
#endif


#if _MSC_VER
  #define ALGNW __declspec(align(ALIGN_SIZE))
  #define ALGNL
#else
  #include <stdlib.h>
  #define ALGNW
  #define ALGNL __attribute__((aligned(ALIGN_SIZE)))
#endif

struct ALGNW LightMemoized {

public:
	float _aligned[6];

	__forceinline operator float *() const { return (float*)&_aligned[0];}

	__forceinline LightMemoized() { for(int i=0;i <6; i++) _aligned[i]=0; };
	__forceinline LightMemoized(float lx, float ly) { _aligned[0]=lx*lx; _aligned[1]=ly*ly; _aligned[2]=lx*ly; _aligned[3]=lx; _aligned[4]=ly; _aligned[5]=1.0f;  };

        __forceinline void *operator new (size_t size) {
            #if _MSC_VER
            void* ptr = _aligned_malloc(size,ALIGN_SIZE);
            #else
            #ifdef __APPLE__
            void* ptr = malloc(size);
            #else
            void* ptr = _mm_malloc(size,ALIGN_SIZE);
            #endif
            #endif
            return ptr;
        };
        __forceinline void operator delete (void * ptr ) {
            #if _MSC_VER
                _aligned_free(ptr);
            #else
            free(ptr);
#ifdef __APPLE__
free(ptr);
#else
_mm_free(ptr);
#endif
            #endif
        };
        __forceinline void *operator new[] (size_t size) {
            #if _MSC_VER
            void* ptr = _aligned_malloc(size,ALIGN_SIZE);
            #else
            #ifdef __APPLE__
            void* ptr = malloc(size);
            #else
            void* ptr = _mm_malloc(size,ALIGN_SIZE);
            #endif
            #endif
            return ptr;
        };
        __forceinline void operator delete[] (void * ptr) {
            #if _MSC_VER
                _aligned_free(ptr);
            #else
#ifdef __APPLE__
free(ptr);
#else
_mm_free(ptr);
#endif
            #endif
        };

} ALGNL;

struct ALGNW PTMCoefficient {

public:
	int _aligned[6];

	__forceinline operator int *() const { return (int*)&_aligned[0];}

	__forceinline PTMCoefficient() { for(int i=0;i <6; i++) _aligned[i]=0; };


	__forceinline float evalPoly(const LightMemoized &Light) const {

        float r = _aligned[5];
        for(int i=0; i < 5 ; i++) r+= _aligned[i]*Light[i];
		return r;
	}

        __forceinline float evalPoly(const float &lx,const float &ly) const {

			LightMemoized Light(lx,ly);
            return evalPoly(Light);
       }

        __forceinline void *operator new (size_t size) {
            #if _MSC_VER
            void* ptr = _aligned_malloc(size,ALIGN_SIZE);
            #else
            void* ptr = malloc(size);
            #endif
            return ptr;
        };


        __forceinline void operator delete (void * ptr ) {
            #if _MSC_VER
                _aligned_free(ptr);
            #else
            free(ptr);
            #endif
        };
        __forceinline void *operator new[] (size_t size) {
            #if _MSC_VER
            void* ptr = _aligned_malloc(size,ALIGN_SIZE);
            #else
//            qDebug() << "doing malloc. size: " <<size;
            void* ptr = malloc(size);
//            qDebug() << "did malloc. ptr: " <<ptr;
            #endif
            return ptr;
        };
        __forceinline void operator delete[] (void * ptr) {
            #if _MSC_VER
                _aligned_free(ptr);
            #else
               free(ptr);
            #endif
        };


} ALGNL;
