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
#ifndef __GROUPING_H__
#define __GROUPING_H__

#include <vector>

/**
 * Solves the grouping problem
 *
 * Problem: when trying to disassemble a puzzle with groups there
 * comes up the problem when identical pieces belong to different groups.
 * Because you don't know, to which group the current piece belongs to
 * you don't know what to do.
 *
 * But the groups assigned to a piece might change the disassembability of the
 * whole puzzle
 *
 * Example a puzzle made out of 8 identical pieces, 4 belong to group A and
 * the other 4 to group B. The pieces are assembled into such a shape
 * that always 4 do interlock and can not be disassembled. Depending
 * on how the pieces from the 2 groups are distributed between the
 * 2 subassemblies the puzzle is or isn't disassembable.
 *
 * The functionality in here is supposed to help solve this problem
 * when the disassembler goes along it's way and tries to disassemble
 * a (sub)problem it only stops premature when there is a clear group
 * assignment available, meaning all pieces have a unique group assigned.
 *
 * Otherwise it tries to disassemble. If this is not possible it
 * checks with the functions in here, if it is possible to assign
 * groups to the pieces in such a way that all the involved pieces
 * are in the same group. The class will allocate the groups to the
 * involved pieces and use this knowledge later on when more groups
 * get added.
 */

class grouping_c {

  struct set {
    unsigned int currentGroup;

    std::vector<unsigned int> pieces;
  };

  std::vector<set> sets;

  struct piece {
    unsigned int piece;
    unsigned int group;
    int count;
  };

  std::vector<piece> pieces;

  unsigned int numGroups;

  unsigned int findPiece(unsigned int pc, unsigned int gp) {
    unsigned int i;

    for (i = 0; i < pieces.size(); i++)
      if ((pieces[i].piece == pc) && (pieces[i].group == gp))
        break;

    return i;
  }

  bool failed;

public:


  grouping_c(void) : numGroups(0), failed(false) {}

  /**
   * Initialisation.
   * the following functions tell the class what pieces can be in what groups
   * and how many of them are available
   */
  void addPieces(unsigned int pc, unsigned int group, unsigned int count);

  /** remove all sets and start fresh */
  void reSet(void);

  /**
   * start a new set of pieces.
   */
  void newSet(void);

  /**
   * Add the pieces you need to the current set. Each time you add a new piece
   * you get a return value saying if it can still be done, as soon
   * as you get false from that function you must stop, it can not
   * be true later any way and the function will return unexpected
   * results after the first false
   */
  bool addPieceToSet(unsigned int pc);

private:

  // no copying and assigning
  grouping_c(const grouping_c&);
  void operator=(const grouping_c&);
};

#endif
