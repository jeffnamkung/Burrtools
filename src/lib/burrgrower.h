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
#ifndef __BURRGROWER_H__
#define __BURRGROWER_H__

#if 0

#include "assembler.h"

#include <vector>

class voxel_c;

class puzzleSol_c : public AssemblerCallbackInterface {

private:

  puzzle_c * puzzle;
  unsigned int prob;

  unsigned long solutions;
  unsigned long maxMoves;
  unsigned long minMoves;
  unsigned long maxLevel;
  unsigned long minLevel;

public:

  puzzleSol_c(puzzle_c * p, unsigned int prob);
  puzzleSol_c(const puzzleSol_c * p);
  virtual ~puzzleSol_c(void);

  double fitness(void);

  bool assembly(assembly_c * a);

  bool nosol() { return solutions == 0; }

  const puzzle_c * getPuzzle(void) const { return puzzle; }

  unsigned long numSolutions() { return solutions; }
  unsigned long numMoves() { return maxMoves; }
  unsigned long numLevel() { return maxLevel; }
};

class burrGrower_c {

  const puzzle_c * base;
  const unsigned int problem;

  unsigned int maxSetSize;

  // puzzles with unique solutions and highes first level
  std::vector<puzzleSol_c*> unique;

  // highest first level
  std::vector<puzzleSol_c*> highLevel;

  // highest overal moves
  std::vector<puzzleSol_c*> highMoves;

  // this function checks every found puzzle for some conditions
  // and saves the most interesting designs found
  void addToLists(puzzleSol_c * pz);

public:

  burrGrower_c(const puzzle_c *pz, unsigned int mss, unsigned int prob) : base(pz), problem(prob), maxSetSize(mss) {}

  void grow(std::vector<puzzleSol_c*> currentSet);
};

#endif


#endif
