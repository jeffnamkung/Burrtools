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
#include "grid-type.h"

#include "don-knuth-assembler.h"
#include "wei_hwa_huang_assembler.h"
#include "movementcache_0.h"
#include "movementcache_1.h"
#include "cube_voxel.h"
#include "triangle_voxel.h"
#include "sphere_voxel.h"
#include "rhombic_voxel.h"
#include "voxel_4.h"
#include "symmetries_0.h"
#include "symmetries_1.h"
#include "symmetries_2.h"
#include "stl_0.h"
#include "stl_2.h"
#include "problem.h"

#include "../tools/xml.h"

GridType::GridType(XmlParser &pars) {
  pars.require(XmlParser::START_TAG, "gridType");

  std::string typeStr = pars.getAttributeValue("type");

  if (!typeStr.length())
    pars.exception("grid type needs a valid 'type' attribute");

  // set to the correct size
  type = (gridType) atoi(typeStr.c_str());

  switch (type) {

    case GT_BRICKS:
    case GT_TRIANGULAR_PRISM:
    case GT_SPHERES:
    case GT_RHOMBIC:
    case GT_TETRA_OCTA:break;

    default:pars.exception("puzzle with unknown grid type");
  }

  pars.skipSubTree();

  sym = 0;
}

GridType::GridType(const GridType &orig) : type(orig.type), sym(0) {
}

void GridType::save(XmlWriter &xml) const {
  xml.newTag("gridType");

  xml.newAttrib("type", (unsigned long) type);

  xml.endTag("gridType");
}

GridType::GridType() {
  type = GT_BRICKS;

  sym = 0;
}

GridType::GridType(gridType gt) {

  type = gt;

  switch (type) {
    case GT_BRICKS:break;

    case GT_TRIANGULAR_PRISM:break;

    case GT_SPHERES:break;

    case GT_RHOMBIC:break;

    case GT_TETRA_OCTA:break;

    default:bt_assert(0);
  }

  sym = 0;
}

GridType::~GridType() {
  if (sym)
    delete sym;
}

movementCache_c *GridType::getMovementCache(const Problem *puz) const {
  switch (type) {
    case GT_BRICKS: return new movementCache_0_c(puz);
    case GT_TRIANGULAR_PRISM: return new movementCache_1_c(puz);
    default: return 0;
  }
}

Voxel *GridType::getVoxel(unsigned int x,
                          unsigned int y,
                          unsigned int z,
                          voxel_type init) const {
  switch (type) {
    case GT_BRICKS: return new CubeVoxel(x, y, z, this, init);
    case GT_TRIANGULAR_PRISM: return new TriangleVoxel(x, y, z, this, init);
    case GT_SPHERES: return new SphereVoxel(x, y, z, this, init);
    case GT_RHOMBIC: return new RhombicVoxel(x, y, z, this, init);
    case GT_TETRA_OCTA: return new voxel_4_c(x, y, z, this, init);
    default: return 0;
  }
}

Voxel *GridType::getVoxel(XmlParser &pars) const {
  switch (type) {
    case GT_BRICKS: return new CubeVoxel(pars, this);
    case GT_TRIANGULAR_PRISM: return new TriangleVoxel(pars, this);
    case GT_SPHERES: return new SphereVoxel(pars, this);
    case GT_RHOMBIC: return new RhombicVoxel(pars, this);
    case GT_TETRA_OCTA: return new voxel_4_c(pars, this);
    default: return 0;
  }
}

Voxel *GridType::getVoxel(const Voxel &orig) const {
  switch (type) {
    case GT_BRICKS: return new CubeVoxel(orig);
    case GT_TRIANGULAR_PRISM: return new TriangleVoxel(orig);
    case GT_SPHERES: return new SphereVoxel(orig);
    case GT_RHOMBIC: return new RhombicVoxel(orig);
    case GT_TETRA_OCTA: return new voxel_4_c(orig);
    default: return 0;
  }
}

Voxel *GridType::getVoxel(const Voxel *orig) const {
  switch (type) {
    case GT_BRICKS: return new CubeVoxel(orig);
    case GT_TRIANGULAR_PRISM: return new TriangleVoxel(orig);
    case GT_SPHERES: return new SphereVoxel(orig);
    case GT_RHOMBIC: return new RhombicVoxel(orig);
    case GT_TETRA_OCTA: return new voxel_4_c(orig);
    default: return 0;
  }
}

const symmetries_c *GridType::getSymmetries(void) const {
  if (!sym) {
    switch (type) {
      case GT_BRICKS:
      case GT_RHOMBIC:
      case GT_TETRA_OCTA:sym = new symmetries_0_c();
        break;
      case GT_TRIANGULAR_PRISM:sym = new symmetries_1_c();
        break;
      case GT_SPHERES:sym = new symmetries_2_c();
        break;
      default:break;
    }
  }

  return sym;
}

unsigned int GridType::getCapabilities(void) const {
  switch (type) {
    case GT_BRICKS:
      return CAP_ASSEMBLE
          | CAP_DISASSEMBLE
          | CAP_STLEXPORT;

    case GT_TRIANGULAR_PRISM:
      return CAP_ASSEMBLE
          | CAP_DISASSEMBLE
          | CAP_STLEXPORT;

    case GT_SPHERES:
      return CAP_ASSEMBLE
          | CAP_STLEXPORT;

    case GT_RHOMBIC:
      return CAP_ASSEMBLE
          | CAP_STLEXPORT;

    case GT_TETRA_OCTA:
      return CAP_ASSEMBLE
          | CAP_STLEXPORT;
    default: return 0;
  }
}

AssemblerInterface *GridType::findAssembler(const Problem *p) {
  if (DonKnuthAssembler::canHandle(p)) {
    fprintf(stderr, "using assembler 0\n");
    return new DonKnuthAssembler();
  }
  if (WeiHwaHuangAssembler::canHandle(p)) {
    fprintf(stderr, "using assembler 1\n");
    return new WeiHwaHuangAssembler();
  }

  return 0;
}

stlExporter_c *GridType::getStlExporter(void) const {
  switch (type) {
    case GT_BRICKS: return new stlExporter_0_c();
    case GT_TRIANGULAR_PRISM: return new stlExporter_0_c();
    case GT_SPHERES: return new stlExporter_2_c();
    case GT_RHOMBIC: return new stlExporter_0_c();
    case GT_TETRA_OCTA: return new stlExporter_0_c();
    default: return 0;
  }
}

