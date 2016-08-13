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


#ifndef ZORDER_H
#define ZORDER_H

#include <math.h>

namespace ZOrder
{

static int ZIndex(int r, int c, int level)
{
	int p = 1;
	int e = 1;
	int index = 0;
	for (int k = 0; k < level; k++)
	{
		if (c&p)
			index += e;

		e <<= 1;

		if (r&p)
			index += e;

		e <<= 1;
		p <<= 1;
	}

	return index;
}

/**
 * Create a matrix such that each element contains 
 * the index of a z-filling curve.
 * This matrix can be used to indexing a quadtree efficiently.
 * 
 * Example (level=2):
 *
 *    0  1  4  5
 *    2  3  6  7
 *    8  9  12 13
 *    10 11 14 15
 */
static int* createZMatrix(int level)
{
	int size = 1;
	for (int k = 0; k < level; k++)
		size <<= 1;

	int *zmatrix = new int[size*size];

	for (int row = 0; row < size; row++)
		for (int col = 0; col < size; col++)
		{
			zmatrix[col + row*size] = ZIndex(row, col, level);
		}

	return zmatrix;
}

} /* end namespace */

#endif  /* ZORDER_H */


