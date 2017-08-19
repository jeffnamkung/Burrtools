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
#include "assembler-interface.h"

#include "../tools/xml.h"

AssemblerInterface::errState AssemblerInterface::createMatrix(const Problem * /*puz*/,
                                                              bool /*keepMirror*/,
                                                              bool /*keepRotations*/,
                                                              bool /*complete*/) {
  return ERR_NONE;
}

AssemblerInterface::errState AssemblerInterface::setPosition(const char * /*string*/,
                                                             const char * /*version*/) {
  return ERR_CAN_NOT_RESTORE_VERSION;
}

void AssemblerInterface::save(XmlWriter &xml) const {
  xml.newTag("assembler");
  xml.endTag("assembler");
}

