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
#include "solution.h"

#include "bt_assert.h"
#include "disassembly.h"
#include "assembly.h"

#include "../tools/xml.h"

Solution::Solution(XmlParser &pars, unsigned int pieces, const GridType *gt)
    :
    tree(0), treeInfo(0), assemblyNum(0), solutionNum(0) {
  pars.require(XmlParser::START_TAG, "solution");

  std::string str;

  str = pars.getAttributeValue("asmNum");
  if (str.length())
    assemblyNum = atoi(str.c_str());

  str = pars.getAttributeValue("solNum");
  if (str.length())
    solutionNum = atoi(str.c_str());

  do {
    int state = pars.nextTag();

    if (state == XmlParser::END_TAG) break;
    pars.require(XmlParser::START_TAG, "");

    if (pars.getName() == "assembly") {
      assembly = new Assembly(pars, pieces, gt);
      pars.require(XmlParser::END_TAG, "assembly");
    } else if (pars.getName() == "separation") {
      if (!assembly)
        pars.exception(
            "an assembly must always be before a separation in a solution");

      // find the number of really placed pieces
      unsigned int pl = 0;
      for (unsigned int i = 0; i < assembly->placementCount(); i++)
        if (assembly->isPlaced(i))
          pl++;
      tree = new Separation(pars, pl);

      pars.require(XmlParser::END_TAG, "separation");
    } else if (pars.getName() == "separationInfo") {
      treeInfo = new SeparationInfo(pars);
      pars.require(XmlParser::END_TAG, "separationInfo");
    } else
      pars.skipSubTree();

    pars.require(XmlParser::END_TAG, "");

  } while (true);

  pars.require(XmlParser::END_TAG, "solution");

  if (!assembly)
    pars.exception("no assembly in solution");

  if (tree && treeInfo) {
    delete treeInfo;
    treeInfo = 0;
  }
}

void Solution::save(XmlWriter &xml) const {
  xml.newTag("solution");

  if (assemblyNum) {
    xml.newAttrib("asmNum", assemblyNum);
  }

  if ((tree || treeInfo) && solutionNum)
    xml.newAttrib("solNum", solutionNum);

  assembly->save(xml);

  if (tree) {
    tree->save(xml);
  } else if (treeInfo) {
    treeInfo->save(xml);
  }

  xml.endTag("solution");
}

Solution::~Solution() {
  if (tree)
    delete tree;

  if (assembly)
    delete assembly;

  if (treeInfo)
    delete treeInfo;
}

void Solution::exchangeShape(unsigned int s1, unsigned int s2) {
  if (assembly)
    assembly->exchangeShape(s1, s2);
  if (tree)
    tree->exchangeShape(s1, s2);
}

const Disassembly *Solution::getDisassemblyInfo(void) const {
  if (tree) return tree;
  if (treeInfo) return treeInfo;
  return 0;
}

Disassembly *Solution::getDisassemblyInfo(void) {
  if (tree) return tree;
  if (treeInfo) return treeInfo;
  return 0;
}

void Solution::removeDisassembly(void) {
  if (tree) {
    if (!treeInfo)
      treeInfo = new SeparationInfo(tree);

    delete tree;
    tree = 0;
  }
}

void Solution::setDisassembly(Separation *sep) {
  if (tree) delete tree;
  tree = sep;

  if (treeInfo) delete treeInfo;
  treeInfo = 0;
}

void Solution::removePieces(unsigned int start, unsigned int count) {
  if (assembly)
    assembly->removePieces(start, count);
  if (tree)
    tree->removePieces(start, count);
}

void Solution::addNonPlacedPieces(unsigned int start, unsigned int count) {
  if (assembly)
    assembly->addNonPlacedPieces(start, count);
  if (tree)
    tree->addNonPlacedPieces(start, count);
}

