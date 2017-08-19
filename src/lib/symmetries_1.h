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
#ifndef __SYMMETRIES_1_H__
#define __SYMMETRIES_1_H__

#include "symmetries.h"

class GridType;

/** this is the symmetries class for triangles */
class symmetries_1_c : public symmetries_c {

 public:

  symmetries_1_c(void);

  unsigned int getNumTransformations(void) const;
  unsigned int getNumTransformationsMirror(void) const;
  bool symmetrieContainsTransformation(symmetries_t s, unsigned int t) const;
  unsigned char transAdd(unsigned char t1, unsigned char t2) const;
  unsigned char minimizeTransformation(symmetries_t s,
                                       unsigned char trans) const;
  unsigned int countSymmetryIntersection(symmetries_t resultSym,
                                         symmetries_t s2) const;
  bool symmetriesLeft(symmetries_t resultSym, symmetries_t s2) const;
  symmetries_t calculateSymmetry(const Voxel *pp) const;
  bool symmetryContainsMirror(symmetries_t sym) const;
  bool symmetryKnown(const Voxel *pp) const;
  bool isTransformationUnique(symmetries_t s, unsigned int trans) const;

 private:

  // no copying and assigning
  symmetries_1_c(const symmetries_1_c &);
  void operator=(const symmetries_1_c &);
};

#endif
