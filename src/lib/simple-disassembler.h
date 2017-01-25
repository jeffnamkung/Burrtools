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
#ifndef __DISASSEMBLER_0_H__
#define __DISASSEMBLER_0_H__

#include "base-disassembler.h"

class Separation;

class disassemblerNode_c;

/**
 * This class is a disassembler especially tuned for simple analysis
 *
 * All involved steps are of size one. This enables us to use a simple
 * tree search instead of A* which would normally be required for
 * a graph search
 */
class SimpleDisassembler : public BaseDisassembler {

private:

  /**
   * The real disassembly routine.
   *
   * It separates the puzzle into 2 parts
   * and gets called recursively with each subpart to disassemble
   *
   * the return is the disassembly tree for that part
   *
   * pieces contains the names of all the pieces that are still inside the
   * subpuzzle puzzle, start defines the starting position of these pieces
   */
  Separation * disassemble_rec(const std::vector<unsigned int> & pieces, disassemblerNode_c * start);

public:

  SimpleDisassembler(const Problem * puz) : BaseDisassembler(puz) { }
  ~SimpleDisassembler() { }

private:

  // no copying and assigning
  SimpleDisassembler(const SimpleDisassembler &);
  void operator=(const SimpleDisassembler &);
};

#endif
