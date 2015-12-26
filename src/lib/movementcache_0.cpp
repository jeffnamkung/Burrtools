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
#include "movementcache_0.h"

#include "voxel.h"

#define NUM_DIRECTIONS 3

movementCache_0_c::movementCache_0_c(const Problem * puz) : movementCache_c(puz) {
}

static int min(int a, int b) { if (a < b) return a; else return b; }
static int max(int a, int b) { if (a > b) return a; else return b; }

/* calculate the required movement possibilities */
unsigned int* movementCache_0_c::moCalcValues(const Voxel * sh1, const Voxel * sh2, int dx, int dy, int dz) {

  /* because the dx, dy and dz values are calculated using the hotspot we need to reverse
   * that process
   */
  dx += (sh1->getHx() - sh2->getHx());
  dy += (sh1->getHy() - sh2->getHy());
  dz += (sh1->getHz() - sh2->getHz());

  unsigned int * move = new unsigned int[NUM_DIRECTIONS];

  /* calculate some bounding boxes for the intersecting and union boxes of the 2 pieces */
  int x1i, x2i, y1i, y2i, z1i, z2i;

  x1i = max(sh1->boundX1(), sh2->boundX1() + dx);
  x2i = min(sh1->boundX2(), sh2->boundX2() + dx);
  y1i = max(sh1->boundY1(), sh2->boundY1() + dy);
  y2i = min(sh1->boundY2(), sh2->boundY2() + dy);
  z1i = max(sh1->boundZ1(), sh2->boundZ1() + dz);
  z2i = min(sh1->boundZ2(), sh2->boundZ2() + dz);

  int x1u, x2u, y1u, y2u, z1u, z2u;

  x1u = sh1->boundX1();
  x2u = max(sh1->boundX2(), sh2->boundX2() + dx);
  y1u = sh1->boundY1();
  y2u = max(sh1->boundY2(), sh2->boundY2() + dy);
  z1u = sh1->boundZ1();
  z2u = max(sh1->boundZ2(), sh2->boundZ2() + dz);

  /* these will contain the result assume free movement for the beginning */
  unsigned int mx, my, mz;
  mx = my = mz = 32000;

  /* now we want to calculate the movement possibilities for the x-axis
   * we need to check the intersecting area of the y and z axis and the union
   * area of the x axis.
   *
   * scan in the positive x-direction and search for the smallest gap between
   * a cube of piece 1 and a cube in piece 2
   * so if we find a cube in piece 1 on our way we reset the start gap marker (last)
   * when we find a cube in the 2nd piece we look how long ago the last piece
   * one hit we had, if that value is smaller than the saved one, we save that
   *
   * to avoid the need for a check for the case that we need to hit a
   * cube 1 first before calculating a gap size I initialize the gap
   * start marker so that the resulting gap would be so big that it is bigger
   * than the initial value
   */
  for (int y = y1i; y <= y2i; y++)
    for (int z = z1i; z <= z2i; z++) {

      unsigned int gap = 32000;

      for (int x = x1u; x <= x2u; x++)
      {
        if (sh1->isFilled2(x, y, z))
        {
          gap = 0;
        }
        else if (sh2->isFilled2(x-dx, y-dy, z-dz))
        {
          if (gap < mx) mx = gap;
        }
        else
        {
          gap++;
        }
      }
    }

  /* same for y direction */
  for (int x = x1i; x <= x2i; x++)
    for (int z = z1i; z <= z2i; z++) {

      unsigned int gap = 32000;

      for (int y = y1u; y <= y2u; y++)
      {
        if (sh1->isFilled2(x, y, z))
        {
          gap = 0;
        }
        else if (sh2->isFilled2(x-dx, y-dy, z-dz))
        {
          if (gap < my) my = gap;
        }
        else
        {
          gap++;
        }
      }
    }

  /* finally the z direction */
  for (int x = x1i; x <= x2i; x++)
    for (int y = y1i; y <= y2i; y++) {

      unsigned int gap = 32000;

      for (int z = z1u; z <= z2u; z++)
      {
        if (sh1->isFilled2(x, y, z))
        {
          gap = 0;
        }
        else if (sh2->isFilled2(x-dx, y-dy, z-dz))
        {
          if (gap < mz) mz = gap;
        }
        else
        {
          gap++;
        }
      }
    }

  move[0] = mx;
  move[1] = my;
  move[2] = mz;

  return move;
}


unsigned int movementCache_0_c::numDirections() { return NUM_DIRECTIONS; }
void movementCache_0_c::getDirection(unsigned int dir, int * x, int * y, int * z) {

  switch (dir) {
    case 0: *x = 1; *y = *z = 0; break;
    case 1: *y = 1; *x = *z = 0; break;
    case 2: *z = 1; *x = *y = 0; break;
    default: bt_assert(0);
  }
}
