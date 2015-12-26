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
#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

class separation_c;
class Assembly;

/**
 * Base class for a disassembler.
 *
 * The interface is simple:
 * -# construct the class with whatever parameters the concrete subclass requires
 * -# call diassemble for each assembly found and evaluate the result
 *
 * some subclasses may be able to handle several assemblies, others may only
 * disassemble one, that depends on the concrete disassembler you use
 */
class DisassemblerInterface {

public:

  DisassemblerInterface() {}

  virtual ~DisassemblerInterface() {}

  /**
   * Try to disassemble an assembly.
   *
   * Because we can only have or don't have a disassembly sequence
   * we don't need the same complicated callback interface. The function
   * returns either the disassembly sequence or a null pointer.
   * you need to take care of freeing the disassembly sequence after
   * doing with it whatever you want
   */
  virtual separation_c * disassemble(const Assembly * /*assembly*/) { return 0; }

private:

  // no copying and assigning
  DisassemblerInterface(const DisassemblerInterface &);
  void operator=(const DisassemblerInterface &);

};

#endif
