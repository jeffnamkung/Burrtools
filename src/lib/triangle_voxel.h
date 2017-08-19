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
#ifndef __VOXEL_1_H__
#define __VOXEL_1_H__

#include "voxel.h"

/**
 * Voxel class for the triangle grid.
 *
 * This grid is a bit more irregular then the others. It has in a way (depending
 * on how you look at it) either 2 different axes, or 4 axes, of which one is different.
 *
 * The grid is a stack of planes of tightly packed triangles. The grid in each plane is
 * identical. But on the plane you have 2 different triangles: one halve points up the
 * other halve points down.
 *
 * This means that you can move along the z-axis (the stacking direction) in one voxel
 * steps, but in teh other directions only in 2 voxel steps.
 */
class TriangleVoxel : public Voxel {

 public:

  TriangleVoxel(unsigned int x,
                unsigned int y,
                unsigned int z,
                const GridType *gt,
                voxel_type init = 0) : Voxel(x, y, z, gt, init) {}
  TriangleVoxel(XmlParser &pars, const GridType *gt) : Voxel(pars, gt) {}
  TriangleVoxel(const Voxel &orig) : Voxel(orig) {}
  TriangleVoxel(const Voxel *orig) : Voxel(orig) {}

  void transformPoint(int *x, int *y, int *z, unsigned int trans) const;
  bool transform(unsigned int nr);

  void minimizePiece(void);
  bool identicalInBB(const Voxel *op, bool includeColors = true) const;

  bool getNeighbor(unsigned int idx,
                   unsigned int typ,
                   int x,
                   int y,
                   int z,
                   int *xn,
                   int *yn,
                   int *zn) const;

  virtual void scale(unsigned int amount, bool grid);
  void resizeInclude(int &px, int &py, int &pz);

  bool validCoordinate(int x, int y, int z) const;
  bool onGrid(int x, int y, int z) const;

  virtual bool meshParamsValid(double bevel, double offset) const;
  virtual void getConnectionFace(int x,
                                 int y,
                                 int z,
                                 int n,
                                 double bevel,
                                 double offset,
                                 std::vector<float> &faceCorners) const;
  virtual void calculateSize(float *x, float *y, float *z) const;
  virtual void recalcSpaceCoordinates(float *x, float *y, float * /*z*/) const;

 private:

  // no copying and assigning
  void operator=(const TriangleVoxel &);
};

#endif
