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
#include "problem.h"

#include "voxel.h"
#include "assembly.h"
#include "disassembly.h"
#include "puzzle.h"
#include "solution.h"

#include "../tools/xml.h"

#include <algorithm>

/** internal class of problem storing the grouping information of a shape */
class Group {
 public:

  Group(unsigned short gr, unsigned short cnt) : group(gr), count(cnt) {}

  unsigned short group;
  unsigned short count;
};

/** internal class of problem storing the information attached to a piece in
 * a problem */
class Part {
 public:
  Part(unsigned short id,
       unsigned short mn,
       unsigned short mx,
       unsigned short grp) : shapeId(id), min(mn), max(mx) {
    if (grp)
      groups.push_back(Group(grp, max));
  }

  Part(unsigned short id, unsigned short mn, unsigned short mx)
      : shapeId(id), min(mn), max(mx) {}

  Part(Part *orig)
      : shapeId(orig->shapeId),
        min(orig->min),
        max(orig->max),
        groups(orig->groups) {}

  void addGroup(unsigned short grp, unsigned short cnt) {
    groups.push_back(Group(grp, cnt));
  }

  unsigned short shapeId;
  unsigned short min;
  unsigned short max;

  std::vector<Group> groups;
};

Problem::Problem(Puzzle &puz) :
    puzzle(puz), result(0xFFFFFFFF),
    assm(0), solveState(SS_UNSOLVED), numAssemblies(0),
    numSolutions(0), usedTime(0), maxHoles(0xFFFFFFFF) {}

Problem::~Problem() {
  if (assm)
    delete assm;
}

Problem::Problem(const Problem *orig, Puzzle &puz) :
    puzzle(puz), result(orig->result),
    solveState(SS_UNSOLVED), numAssemblies(0), numSolutions(0), usedTime(0) {
  assm = 0;

  for (std::set<uint32_t>::iterator i = orig->colorConstraints.begin();
       i != orig->colorConstraints.end(); i++)
    colorConstraints.insert(*i);

  for (const auto &part : orig->parts_) {
    parts_.push_back(std::make_unique<Part>(*part));
  }

  maxHoles = orig->maxHoles;

  // solutions are NOT copied including the fields assm, solve state is set to unsolved
  // number of solutions and time to find them are unknown...

  // the name is also left intentionally empty because the user will
  // likely give a new name any way
}

void Problem::save(XmlWriter &xml) const {
  xml.newTag("problem");

  if (name.length() > 0)
    xml.newAttrib("name", name);

  xml.newAttrib("state", solveState);

  if (solveState != SS_UNSOLVED) {
    xml.newAttrib("assemblies", numAssemblies);
    xml.newAttrib("solutions", numSolutions);
    xml.newAttrib("time", usedTime);
  }

  if (maxHoles != 0xFFFFFFFF)
    xml.newAttrib("maxHoles", maxHoles);

  xml.newTag("shapes"); // for historical reasons, parts are saved in an XML-tag called shapes

  for (const auto &part : parts_) {
    xml.newTag("shape");

    xml.newAttrib("id", part->shapeId);

    if (part->min == part->max) {
      xml.newAttrib("count", part->min);
    } else {
      xml.newAttrib("min", part->min);
      xml.newAttrib("max", part->max);
    }

    if (part->groups.size() == 0) {
      // do nothing, we don't need to save anything in this case
    } else if ((part->groups.size() == 1) &&
        (part->groups[0].count == part->max)) {
      // this is the case when all pieces are in the same group
      // we only need to save the group, if it is not 0,
      // the loader takes 0 as default anyway
      if (part->groups[0].group != 0)
        xml.newAttrib("group", part->groups[0].group);
    } else {
      for (const auto &group : part->groups) {
        if (group.group != 0) {
          xml.newTag("group");
          xml.newAttrib("group", group.group);
          xml.newAttrib("count", group.count);
          xml.endTag("group");
        }
      }
    }
    xml.endTag("shape");
  }

  xml.endTag("shapes");

  xml.newTag("result");
  xml.newAttrib("id", result);
  xml.endTag("result");

  xml.newTag("bitmap");
  for (std::set<uint32_t>::iterator i = colorConstraints.begin();
       i != colorConstraints.end(); i++) {
    xml.newTag("pair");
    xml.newAttrib("piece", *i >> 16);
    xml.newAttrib("result", *i & 0xFFFF);
    xml.endTag("pair");
  }
  xml.endTag("bitmap");

  if (solveState == SS_SOLVING) {
    if (assm) {
      assm->save(xml);
    } else if (assemblerState != "" && assemblerVersion != "") {
      xml.newTag("assembler");
      xml.newAttrib("version", assemblerVersion);
      xml.addContent(assemblerState);
      xml.endTag("assembler");
    }
  }

  if (solutions_.size()) {
    xml.newTag("solutions");
    for (unsigned int i = 0; i < solutions_.size(); i++)
      solutions_[i]->save(xml);
    xml.endTag("solutions");
  }

  xml.endTag("problem");
}

Problem::Problem(Puzzle &puz, XmlParser &pars)
    : puzzle(puz), result(0xFFFFFFFF), assm(0) {
  pars.require(XmlParser::START_TAG, "problem");

  name = pars.getAttributeValue("name");
  solveState = SS_UNSOLVED;
  numAssemblies = numSolutions = usedTime = 0;
  maxHoles = 0xFFFFFFFF;

  std::string str = pars.getAttributeValue("maxHoles");
  if (str.length())
    maxHoles = atoi(str.c_str());

  str = pars.getAttributeValue("state");
  if (str.length())
    solveState = (SolverState) atoi(str.c_str());

  if (solveState != SS_UNSOLVED) {
    str = pars.getAttributeValue("assemblies");
    if (str.length())
      numAssemblies = atoi(str.c_str());

    str = pars.getAttributeValue("solutions");
    if (str.length())
      numSolutions = atoi(str.c_str());

    str = pars.getAttributeValue("time");
    if (str.length())
      usedTime = atoi(str.c_str());
  }

  unsigned int pieces = 0;

  do {
    int state = pars.nextTag();

    if (state == XmlParser::END_TAG) break;
    pars.require(XmlParser::START_TAG, "");

    if (pars.getName()
        == "shapes")  // parts are saved in this tag for historical reasons
    {
      do {
        int state = pars.nextTag();

        if (state == XmlParser::END_TAG) break;
        pars.require(XmlParser::START_TAG, "");

        if (pars.getName() == "shape") {
          unsigned short id, min, max, grp;

          str = pars.getAttributeValue("id");
          if (str.length() == 0)
            pars.exception("a shape node must have an 'idt' attribute");

          id = atoi(str.c_str());

          str = pars.getAttributeValue("count");

          if (str.length())
            min = max = atoi(str.c_str());
          else if (pars.getAttributeValue("min").length() &&
              pars.getAttributeValue("max").length()) {
            min = atoi(pars.getAttributeValue("min").c_str());
            max = atoi(pars.getAttributeValue("max").c_str());
            if (min > max)
              pars.exception("min of shape count must by <= max");
          } else
            min = max = 1;

          str = pars.getAttributeValue("group");
          grp = atoi(str.c_str());

          pieces += max;

          if (id >= puzzle.shapeNumber())
            pars.exception("the shape ids must be for valid shapes");

          if (grp) {
            parts_.push_back(std::make_unique<Part>(id, min, max, grp));
            pars.skipSubTree();
          } else {
            /* OK we have 2 ways to specify groups for pieces, either
             * a group attribute in the tag. Then all pieces are
             * in the given group, or you specify a list of group
             * tags inside the tag. Each of the group tag gives a
             * group and a count
             */
            parts_.push_back(std::make_unique<Part>(id, min, max));

            do {
              int state = pars.nextTag();

              if (state == XmlParser::END_TAG) break;
              pars.require(XmlParser::START_TAG, "");

              if (pars.getName() == "group") {
                str = pars.getAttributeValue("group");
                if (!str.length())
                  pars.exception(
                      "a group node must have a valid group attribute");
                grp = atoi(str.c_str());

                str = pars.getAttributeValue("count");
                if (!str.length())
                  pars.exception(
                      "a group node must have a valid count attribute");
                unsigned int cnt = atoi(str.c_str());

                (*parts_.rbegin())->addGroup(grp, cnt);
              }

              pars.skipSubTree();

            } while (true);
          }
        } else
          pars.skipSubTree();

        pars.require(XmlParser::END_TAG, "shape");

      } while (true);

      pars.require(XmlParser::END_TAG, "shapes");
    } else if (pars.getName() == "result") {
      str = pars.getAttributeValue("id");
      if (!str.length())
        pars.exception(
            "the result node must have an 'id' attribute with content");
      result = atoi(str.c_str());
      pars.skipSubTree();
    } else if (pars.getName() == "solutions") {
      do {
        int state = pars.nextTag();

        if (state == XmlParser::END_TAG) break;
        pars.require(XmlParser::START_TAG, "");

        if (pars.getName() == "solution")
          solutions_.push_back(std::make_unique<Solution>(pars,
                                                          pieces,
                                                          puzzle.getGridType()));
        else
          pars.skipSubTree();

        pars.require(XmlParser::END_TAG, "solution");

      } while (true);

      pars.require(XmlParser::END_TAG, "solutions");
    } else if (pars.getName() == "bitmap") {
      do {
        int state = pars.nextTag();

        if (state == XmlParser::END_TAG) break;
        pars.require(XmlParser::START_TAG, "");

        if (pars.getName() == "pair") {
          str = pars.getAttributeValue("piece");
          if (str.length() == 0)
            pars.exception("a pair node must have a piece attribute");
          unsigned int piece = atoi(str.c_str());

          str = pars.getAttributeValue("result");
          if (str.length() == 0)
            pars.exception("a pair node must have a result attribute");
          unsigned int result = atoi(str.c_str());

          colorConstraints.insert(piece << 16 | result);
        }

        pars.skipSubTree();

      } while (true);

      pars.require(XmlParser::END_TAG, "bitmap");
    } else if (pars.getName() == "assembler") {
      str = pars.getAttributeValue("version");
      if (str.length()) {
        assemblerVersion = str;
        pars.next();
        assemblerState = pars.getText();
        pars.next();
        pars.require(XmlParser::END_TAG, "assembler");
      } else
        pars.skipSubTree();

      pars.require(XmlParser::END_TAG, "assembler");
    } else {
      pars.skipSubTree();
    }

  } while (true);

  // some post processing after the loading of values

  // if, for whatever reasons the shape is was not right, we reset it to an empty shape
  if (result >= puzzle.shapeNumber())
    result = 0xFFFFFFFF;

  pars.require(XmlParser::END_TAG, "problem");
}

/************** PROBLEM ****************/

void Problem::removeShape(unsigned short idx) {

  if (result == idx) {
    editProblem();
    result = 0xFFFFFFFF;
  }

  // remove the shape from the part list
  setShapeMaximum(idx, 0);

  /* now check all shapes, and the result, if their id is larger
   * than the deleted shape, if so decrement to update the number
   */
  for (auto &part : parts_) {
    if (part->shapeId > idx) {
      part->shapeId--;
    }
  }

  if (result > idx)
    result--;
}

void Problem::exchangeShapeId(unsigned int s1, unsigned int s2) {

  if (result == s1) result = s2;
  else if (result == s2) result = s1;

  for (auto &part : parts_) {
    if (part->shapeId == s1) {
      part->shapeId = s2;
    } else if (part->shapeId == s2) {
      part->shapeId = s1;
    }
  }
}

void Problem::allowPlacement(unsigned int pc, unsigned int res) {
  bt_assert(pc <= puzzle.colorNumber());
  bt_assert(res <= puzzle.colorNumber());

  if ((pc == 0) || (res == 0))
    return;

  colorConstraints.insert((pc - 1) << 16 | (res - 1));
}

void Problem::disallowPlacement(unsigned int pc, unsigned int res) {
  bt_assert(pc <= puzzle.colorNumber());
  bt_assert(res <= puzzle.colorNumber());

  if ((pc == 0) || (res == 0))
    return;

  std::set<uint32_t>::iterator
      i = colorConstraints.find((pc - 1) << 16 | (res - 1));
  if (i != colorConstraints.end())
    colorConstraints.erase(i);
}

bool Problem::placementAllowed(unsigned int pc, unsigned int res) const {
  bt_assert(pc <= puzzle.colorNumber());
  bt_assert(res <= puzzle.colorNumber());

  if (puzzle.colorNumber() == 0)
    return true;

  return (pc == 0) || (res == 0)
      || (colorConstraints.find((pc - 1) << 16 | (res - 1))
          != colorConstraints.end());
}

void Problem::exchangeShape(unsigned int s1, unsigned int s2) {
  bt_assert(s1 < parts_.size());
  bt_assert(s2 < parts_.size());

  if (s1 == s2) return;

  if (s1 > s2) {
    unsigned int s = s1;
    s1 = s2;
    s2 = s;
  }

  unsigned int p1Start;
  unsigned int p2Start;
  unsigned int p1Count;
  unsigned int p2Count;

  p1Start = p2Start = 0;

  for (unsigned int i = 0; i < s1; i++)
    p1Start += parts_[i]->max;

  for (unsigned int i = 0; i < s2; i++)
    p2Start += parts_[i]->max;

  p1Count = parts_[s1]->max;
  p2Count = parts_[s2]->max;

  bt_assert(p1Start + p1Count == p2Start);

  parts_[s1].swap(parts_[s2]);

  /* this vector holds the target position of all the involved piece
   * as long as its not in the order 0, 1, 2, ... some pieces must be exchanged
   */
  std::vector<unsigned int> pos;

  pos.resize(p1Count + p2Count);

  for (unsigned int i = 0; i < p1Count; i++)
    pos[i] = p2Count + i;
  for (unsigned int i = 0; i < p2Count; i++)
    pos[p1Count + i] = i;

  for (unsigned int i = 0; i < p1Count + p2Count - 1; i++)
    if (pos[i] != i) {

      // search for i
      unsigned int j = i + 1;
      while ((j < p1Count + p2Count) && (pos[j] != i)) j++;

      bt_assert(j < p1Count + p2Count);

      for (unsigned int s = 0; s < solutions_.size(); s++)
        solutions_[s]->exchangeShape(p1Start + i, p1Start + j);

      pos[j] = pos[i];
      // normally we would also need pos[i] = i; but as we don't touch that field any more let's save that operation
    }
}

void Problem::setResultId(unsigned int shape) {
  bt_assert(shape < puzzle.shapeNumber());

  if (shape != result) {
    removeAllSolutions();
    result = shape;
  }
}

unsigned int Problem::getResultId(void) const {
  bt_assert(result < puzzle.shapeNumber());
  return result;
}
bool Problem::resultValid(void) const {
  return result < puzzle.shapeNumber();
}

/* get the result shape voxel space */
const Voxel *Problem::getResultShape(void) const {
  bt_assert(result < puzzle.shapeNumber());
  return puzzle.getShape(result);
}

Voxel *Problem::getResultShape() {
  bt_assert(result < puzzle.shapeNumber());
  return puzzle.getShape(result);
}

void Problem::setShapeMinimum(unsigned int shape, unsigned int count) {
  bt_assert(shape < puzzle.shapeNumber());

  editProblem();

  unsigned int pieceIdx = 0;

  for (const auto &part : parts_) {
    if (part->shapeId == shape) {
      part->min = count;
      if (part->min > part->max)
        setShapeMaximum(shape, count);

      if (count > 0) {
        // delete all solutions that have not the required minimum of the pieces
        unsigned int s = 0;
        while (s < solutions_.size()) {
          if (!solutions_[s]->getAssembly()->isPlaced(pieceIdx + count - 1)) {
            solutions_.erase(solutions_.begin() + s);
          } else {
            s++;
          }
        }
      }

      return;
    }
    pieceIdx += part->max;
  }

  // when we get here there is no piece with the required puzzle shape, so add it
  if (count) {
    parts_.push_back(std::make_unique<Part>(shape, count, count, 0));

    // add new placements, pieces are not placed
    for (const auto &solution : solutions_) {
      solution->addNonPlacedPieces(pieceNumber(), count);
    }
  }
}

void Problem::setShapeMaximum(unsigned int shape, unsigned int count) {
  bt_assert(shape < puzzle.shapeNumber());

  unsigned int pieceIdx = 0;

  for (unsigned int id = 0; id < parts_.size(); id++) {
    if (parts_[id]->shapeId == shape) {
      // remove the shape
      if (count == 0) {
        editProblem();

        unsigned int s = 0;

        while (s < solutions_.size()) {
          if (solutions_[s]->getAssembly()->isPlaced(pieceIdx)) {
            solutions_.erase(solutions_.begin() + s);
          } else {
            solutions_[s]->removePieces(pieceIdx, parts_[id]->max);
            s++;
          }
        }

        parts_.erase(parts_.begin() + id);

        return;
      }

      // increase the maximum
      if (count > parts_[id]->max) {
        editProblem();

        for (unsigned int s = 0; s < solutions_.size(); s++)
          solutions_[s]->addNonPlacedPieces(pieceIdx + parts_[id]->max,
                                            count - parts_[id]->max);
      } else if (count < parts_[id]->max)  // cedrease the maximum
      {
        editProblem();
        // go through all solutions, those solutions that use more
        // than the new max pieces of this shape must be deleted
        //
        // the other solutions may stay, but require some
        // placements removed
        unsigned int s = 0;
        while (s < solutions_.size()) {
          if (solutions_[s]->getAssembly()->isPlaced(
              pieceIdx + parts_[id]->max - 1)) {
            // too many pieces placed -> delete solution
            solutions_.erase(solutions_.begin() + s);
          } else {
            solutions_[s]->removePieces(pieceIdx + count,
                                        parts_[id]->max - count);
            s++;
          }
        }
      }

      parts_[id]->max = count;
      if (parts_[id]->max < parts_[id]->min)
        setShapeMinimum(shape, count);

      return;
    }
    pieceIdx += parts_[id]->max;
  }

  if (count) {
    parts_.push_back(std::make_unique<Part>(shape, 0, count, 0));

    // add new placements, pieces are not placed
    for (unsigned int s = 0; s < solutions_.size(); s++)
      solutions_[s]->addNonPlacedPieces(pieceIdx, count);
  }
}

unsigned int Problem::getShapeMinimum(unsigned int shape) const {
  bt_assert(shape < puzzle.shapeNumber());

  for (const auto &part : parts_) {
    if (part->shapeId == shape) {
      return part->min;
    }
  }

  return 0;
}

unsigned int Problem::getShapeMaximum(unsigned int shape) const {
  bt_assert(shape < puzzle.shapeNumber());

  for (const auto &part : parts_) {
    if (part->shapeId == shape) {
      return part->max;
    }
  }

  return 0;
}

/* return the number of pieces in the problem (sum of all counts of all shapes */
unsigned int Problem::pieceNumber(void) const {
  unsigned int result = 0;

  for (const auto &part : parts_) {
    result += part->max;
  }

  return result;
}

/* return the shape id of the given shape (index into the shape array of the puzzle */
unsigned int Problem::getShape(unsigned int shapeID) const {
  bt_assert(shapeID < parts_.size());

  return parts_[shapeID]->shapeId;
}

unsigned int Problem::getShapeId(unsigned int shape) const {
  bt_assert(shape < puzzle.shapeNumber());

  for (unsigned int i = 0; i < parts_.size(); i++)
    if (parts_[i]->shapeId == shape)
      return i;

  bt_assert(0);
  return 0;
}

bool Problem::usesShape(unsigned int shape) const {
  if (result == shape)
    return true;

  for (const auto &part : parts_) {
    if (part->shapeId == shape) {
      return true;
    }
  }

  return false;
}

const Voxel *Problem::getShapeShape(unsigned int shapeID) const {
  bt_assert(shapeID < parts_.size());
  return puzzle.getShape(parts_[shapeID]->shapeId);
}

Voxel *Problem::getShapeShape(unsigned int shapeID) {
  bt_assert(shapeID < parts_.size());
  return puzzle.getShape(parts_[shapeID]->shapeId);
}

/* return the instance count for one shape of the problem */
unsigned int Problem::getShapeMin(unsigned int shapeID) const {
  bt_assert(shapeID < parts_.size());

  return parts_[shapeID]->min;
}
unsigned int Problem::getShapeMax(unsigned int shapeID) const {
  bt_assert(shapeID < parts_.size());

  return parts_[shapeID]->max;
}

void Problem::addSolution(Assembly *assm) {
  bt_assert(assm);
  bt_assert(solveState == SS_SOLVING);

  solutions_.push_back(std::make_unique<Solution>(assm, numAssemblies));
}

void Problem::addSolution(Assembly *assm,
                          Separation *disasm,
                          unsigned int pos) {
  bt_assert(assm);
  bt_assert(solveState == SS_SOLVING);

  // if the given index is behind the number of solutions add at the end
  if (pos < solutions_.size())
    solutions_.insert(solutions_.begin() + pos,
                      std::make_unique<Solution>(assm, numAssemblies, disasm,
                                                 numSolutions));
  else
    solutions_.push_back(std::make_unique<Solution>(assm,
                                                    numAssemblies,
                                                    disasm,
                                                    numSolutions));
}

void Problem::addSolution(Assembly *assm,
                          SeparationInfo *disasm,
                          unsigned int pos) {
  bt_assert(assm);
  bt_assert(solveState == SS_SOLVING);

  // if the given index is behind the number of solutions add at the end
  if (pos < solutions_.size())
    solutions_.insert(solutions_.begin() + pos,
                      std::make_unique<Solution>(assm, numAssemblies, disasm,
                                                 numSolutions));
  else
    solutions_.push_back(std::make_unique<Solution>(assm,
                                                    numAssemblies,
                                                    disasm,
                                                    numSolutions));
}

void Problem::removeAllSolutions() {
  solutions_.clear();
  delete assm;
  assm = 0;
  assemblerState = "";
  solveState = SS_UNSOLVED;
  numAssemblies = 0;
  numSolutions = 0;
  usedTime = 0;
}

void Problem::removeSolution(unsigned int sol) {
  bt_assert(sol < solutions_.size());
  solutions_.erase(solutions_.begin() + sol);
}

AssemblerInterface::errState Problem::setAssembler(AssemblerInterface *assm) {

  if (assemblerState.length()) {

    bt_assert(solveState == SS_SOLVING);

    // if we have some assembler position data, try to load that
    AssemblerInterface::errState err =
        assm->setPosition(assemblerState.c_str(), assemblerVersion.c_str());

    // when we could not load, return with error and reset to unsolved
    if (err != AssemblerInterface::ERR_NONE) {
      solveState = SS_UNSOLVED;
      return err;
    }

    // and remove that data
    assemblerState = "";

  } else {

    bt_assert(solveState == SS_UNSOLVED);

    // if no data is available for assembler restoration reset counters
    bt_assert(numAssemblies == 0);
    bt_assert(numSolutions == 0);

    solveState = SS_SOLVING;
  }

  this->assm = assm;
  return AssemblerInterface::ERR_NONE;
}

void Problem::setShapeGroup(unsigned int shapeID,
                            unsigned short group,
                            unsigned short count) {
  bt_assert(shapeID < parts_.size());

  // not first look, if we already have this group number in our list
  for (unsigned int i = 0; i < parts_[shapeID]->groups.size(); i++)
    if (parts_[shapeID]->groups[i].group == group) {

      /* if we change count, change it, if we set count to 0 remove that entry */
      if (count)
        parts_[shapeID]->groups[i].count = count;
      else
        parts_[shapeID]->groups.erase(parts_[shapeID]->groups.begin() + i);
      return;
    }

  // not found add, but only groups not equal to 0
  if (group && count)
    parts_[shapeID]->addGroup(group, count);
}

unsigned short Problem::getShapeGroupNumber(unsigned int shapeID) const {
  bt_assert(shapeID < parts_.size());

  return parts_[shapeID]->groups.size();
}

unsigned short Problem::getShapeGroup(unsigned int shapeID,
                                      unsigned int groupID) const {
  bt_assert(shapeID < parts_.size());
  bt_assert(groupID < parts_[shapeID]->groups.size());

  return parts_[shapeID]->groups[groupID].group;
}

unsigned short Problem::getShapeGroupCount(unsigned int shapeID,
                                           unsigned int groupID) const {
  bt_assert(shapeID < parts_.size());
  bt_assert(groupID < parts_[shapeID]->groups.size());

  return parts_[shapeID]->groups[groupID].count;
}

unsigned int Problem::pieceToShape(unsigned int piece) const {

  unsigned int shape = 0;

  bt_assert(shape < parts_.size());

  while (piece >= parts_[shape]->max) {
    piece -= parts_[shape]->max;
    shape++;
    bt_assert(shape < parts_.size());
  }

  return shape;
}

unsigned int Problem::pieceToSubShape(unsigned int piece) const {

  unsigned int shape = 0;

  bt_assert(shape < parts_.size());

  while (piece >= parts_[shape]->max) {
    piece -= parts_[shape]->max;
    shape++;
    bt_assert(shape < parts_.size());
  }

  return piece;
}

const GridType *Problem::getGridType(void) const { return puzzle.getGridType(); }
GridType *Problem::getGridType() { return puzzle.getGridType(); }

void Problem::sortSolutions(int by) {
  stable_sort(solutions_.begin(), solutions_.end(), [by](auto &s1, auto &s2) {
    switch (by) {
      case 0:return s1->getAssemblyNumber() < s2->getAssemblyNumber();
      case 1:
        return s1->getDisassemblyInfo() && s2->getDisassemblyInfo() &&
            s1->getDisassemblyInfo()->compare(s2->getDisassemblyInfo()) < 0;
      case 2:
        return s1->getDisassemblyInfo() && s2->getDisassemblyInfo() &&
            s1->getDisassemblyInfo()->sumMoves() <
                s2->getDisassemblyInfo()->sumMoves();
      case 3:return s1->getAssembly()->comparePieces(s2->getAssembly()) > 0;
    }
  });
}

void Problem::editProblem(void) {
  if (solveState == SS_SOLVING || solveState == SS_SOLVED)
    makeUnknown();
}

void Problem::makeUnknown(void) {
  solveState = SS_UNKNOWN;

  if (assm) delete assm;
  assm = 0;
  assemblerState = "";
  assemblerVersion = "";

  numAssemblies = 0;
  numSolutions = 0;
  usedTime = 0;
}
