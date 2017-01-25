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
#ifndef __SOLUTION_H__
#define __SOLUTION_H__

class Assembly;
class Separation;
class separationInfo_c;
class XmlParser;
class XmlWriter;
class GridType;
class Disassembly;

/**
 * This class stores the information for one solution for a
 * problem.
 */
class solution_c
{
  /* the assembly contains the pieces so that they
   * do assemble into the result shape */
  Assembly * assembly;

  /* the disassembly tree, only not NULL, if we
   * have disassembled the puzzle
   */
  Separation * tree;

  /* if no separation is given, maybe we have a separationInfo
   * that contains only some of the information of a full separation
   * but requires a lot less memory
   */
  separationInfo_c * treeInfo;

  /* as it is now possible to not save all solutions
   * it might be useful to know the exact number and sequence
   * how solutions were found
   *
   * solutionNum is 0, when no disassembly is known
   */
  unsigned int assemblyNum;
  unsigned int solutionNum;

public:

  /** create a solution with a proper separation */
  solution_c(Assembly * assm, unsigned int assmNum, Separation * t, unsigned int solNum) :
    assembly(assm), tree(t), treeInfo(0), assemblyNum(assmNum), solutionNum(solNum) {}

  /** create a solution with only separation information */
  solution_c(Assembly * assm, unsigned int assmNum, separationInfo_c * ti, unsigned int solNum) :
    assembly(assm), tree(0), treeInfo(ti), assemblyNum(assmNum), solutionNum(solNum) {}

  /** creat a solution with assembly only, no disassembly */
  solution_c(Assembly * assm, unsigned int assmNum) :
    assembly(assm), tree(0), treeInfo(0), assemblyNum(assmNum), solutionNum(0) {}

  /** load a solution from file */
  solution_c(XmlParser & pars, unsigned int pieces, const GridType * gt);

  ~solution_c(void);

  /** save the solution to the XML file */
  void save(XmlWriter & xml) const;

  /** get the assembly from this solution, it will always be not NULL */
  Assembly * getAssembly() { return assembly; }
  const Assembly * getAssembly(void) const { return assembly; }

  /** get the full disassembly or 0 if there is none */
  Separation * getDisassembly() { return tree; }
  const Separation * getDisassembly(void) const { return tree; }

  /** get either the disassembly or the disassembly information or nothing */
  Disassembly * getDisassemblyInfo(void);
  const Disassembly * getDisassemblyInfo(void) const;

  /** get the assembly number */
  unsigned int getAssemblyNumber(void) const { return assemblyNum; }
  /** get the solution number (0 if this solution has no disassembly */
  unsigned int getSolutionNumber(void) const { return solutionNum; }

  /** remove an existing disassembly, and replace it by disassembly information */
  void removeDisassembly(void);
  /** add a new disassembly to this solution, deleting an old one, in
   * case such an old exists
   */
  void setDisassembly(Separation * sep);

  /** change the solution so that shape s1 and s2 are swapped */
  void exchangeShape(unsigned int s1, unsigned int s2);

  /** remove count pieces starting with start from all information within this solution */
  void removePieces(unsigned int start, unsigned int count);

  /** add count not placed pieces starting at position start */
  void addNonPlacedPieces(unsigned int start, unsigned int count);
};

#endif
