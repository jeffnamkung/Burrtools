/* BurrTools
 *
 * BurrTools is the legal property of its developers, whose
 * names are listed in the COPYRIGHT file, which is included
 * within the source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef __VOXEL_0_H__
#define __VOXEL_0_H__

#include "voxel.h"

/** \file voxel_0.h
 * Contains the declaration of \ref voxel_0_c
 */

/** voxel space class for cube based grids.
 * that are cubes themselfes but also grid like the rhombic grid, which
 * use functionality of this class (mainly the transform methods)
 *
 * The additional function comments contain mainly implementation details
 *
 * Compared to the other 2 basic voxel grids (triangles and spheres) this grid
 * is special as it implements many functions in a more efficient way because
 * the cubic grid is so simple
 */
class voxel_0_c : public Voxel {

  public:

    voxel_0_c(unsigned int x, unsigned int y, unsigned int z, const GridType * gt, voxel_type init = 0) : Voxel(x, y, z, gt, init) {}
    voxel_0_c(XmlParser & pars, const GridType * gt) : Voxel(pars, gt) {}
    voxel_0_c(const Voxel & orig) : Voxel(orig) { }
    voxel_0_c(const Voxel * orig) : Voxel(orig) { }

    void transformPoint(int * x, int * y, int * z, unsigned int trans) const;
    bool transform(unsigned int nr);

    bool getNeighbor(unsigned int idx, unsigned int typ, int x, int y, int z, int * xn, int *yn, int *zn) const;

    virtual void scale(unsigned int amount, bool grid);
    bool scaleDown(unsigned char by, bool action);
    void resizeInclude(int & px, int & py, int & pz);

    bool validCoordinate(int x, int y, int z) const;
    bool onGrid(int x, int y, int z) const;

    virtual void getConnectionFace(int x, int y, int z, int n, double bevel, double offset, std::vector<float> & faceCorners) const;
    virtual void calculateSize(float * x, float * y, float * z) const;
    virtual bool meshParamsValid(double bevel, double offset) const;

  private:

    // no copying and assigning
    void operator=(const voxel_0_c&);
};

#endif
