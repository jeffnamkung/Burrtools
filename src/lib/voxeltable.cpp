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
#include "voxeltable.h"

#include "voxel.h"
#include "puzzle.h"

voxelTable_c::voxelTable_c(void)
{
  tableSize = 101;
  hashTable = new hashNode*[tableSize];

  for (unsigned long i = 0; i < tableSize; i++)
    hashTable[i] = 0;

  tableEntries = 0;
}

voxelTable_c::~voxelTable_c() {

  for (unsigned int i = 0; i < tableSize; i++) {
    while (hashTable[i]) {

      hashNode * n = hashTable[i];
      hashTable[i] = n->next;

      delete n;
    }
  }

  delete [] hashTable;
}

static unsigned long calcHashValue(const Voxel * v)
{
  unsigned long res = 101;

  res *= v->boundX2()-v->boundX1();
  res += 11;
  res *= v->boundY2()-v->boundY1();
  res += 13;
  res *= v->boundZ2()-v->boundZ1();
  res += 17;

  for (unsigned int z = v->boundZ1(); z <= v->boundZ2(); z++)
    for (unsigned int y = v->boundY1(); y <= v->boundY2(); y++)
      for (unsigned int x = v->boundX1(); x <= v->boundX2(); x++)
      {
        res *= 101;
        res += v->getState(x, y, z);
      }

  return res;
}

static unsigned long calcColourHashValue(const Voxel * v)
{
  unsigned long res = 101;

  res *= v->boundX2()-v->boundX1();
  res += 11;
  res *= v->boundY2()-v->boundY1();
  res += 13;
  res *= v->boundZ2()-v->boundZ1();
  res += 17;

  for (unsigned int z = v->boundZ1(); z <= v->boundZ2(); z++)
    for (unsigned int y = v->boundY1(); y <= v->boundY2(); y++)
      for (unsigned int x = v->boundX1(); x <= v->boundX2(); x++)
      {
        res *= 101;
        res += v->getState(x, y, z);
        if (v->getState(x, y, z) != Voxel::VX_EMPTY)
        {
          res *= 3;
          res += v->getColor(x, y, z);
        }
      }

  return res;
}

bool voxelTable_c::getSpace(const Voxel *v, unsigned int *index, unsigned char * trans, unsigned int params) const
{
  unsigned long hash = (params & PAR_COLOUR) ? calcColourHashValue(v) : calcHashValue(v);

  hashNode * n = hashTable[hash % tableSize];

  while (n)
  {
    if (n->hash == hash && ((params & PAR_MIRROR) || (n->transformation < v->getGridType()->getSymmetries()->getNumTransformations())))
    {
      Voxel * v2 = v->getGridType()->getVoxel(findSpace(n->index));
      bool found = v2->transform(n->transformation) && v->identicalInBB(v2, false);
      delete v2;

      if (found)
      {
        if (index) *index = n->index;
        if (trans) *trans = n->transformation;

        return true;
      }
    }
    n = n->next;
  }

  return false;
}

void voxelTable_c::addSpace(unsigned int index, unsigned int params)
{
  const Voxel * v = findSpace(index);

  const symmetries_c * symm = v->getGridType()->getSymmetries();

  unsigned char endTrans = (params & PAR_MIRROR)
    ? symm->getNumTransformationsMirror()
    : symm->getNumTransformations();

  symmetries_t sym = v->selfSymmetries();

  if (tableEntries >= tableSize) {
    // rehash table

    unsigned long newSize = 3*tableSize + 1;
    hashNode ** t2 = new hashNode * [newSize];

    for (unsigned long i = 0; i < newSize; i++)
      t2[i] = 0;

    for (unsigned int i = 0; i < tableSize; i++) {
      while (hashTable[i]) {
        hashNode * n = hashTable[i];
        hashTable[i] = n->next;

        n->next = t2[n->hash % newSize];
        t2[n->hash % newSize] = n;
      }
    }

    delete [] hashTable;
    hashTable = t2;
    tableSize = newSize;
  }

  for (unsigned char trans = 0; trans < endTrans; trans++) {
    if (symm->isTransformationUnique(sym, trans)) {
      // add all transformations of the voxel space to the table, that are actually different

      Voxel * v2 = v->getGridType()->getVoxel(v);
      if (v2->transform(trans))
      {
        unsigned long hash = (params & PAR_COLOUR) ? calcColourHashValue(v2) : calcHashValue(v2);

        hashNode * n = new hashNode;
        n->index = index;
        n->transformation = trans;
        n->hash = hash;
        n->next = hashTable[hash % tableSize];
        hashTable[hash % tableSize] = n;
        tableEntries++;
      }

      delete v2;
    }
  }
}

const Voxel * voxelTablePuzzle_c::findSpace(unsigned int index) const { return puzzle->getShape(index); }

