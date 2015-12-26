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
#ifndef __DISASSEMBLY_H__
#define __DISASSEMBLY_H__

/* this module contains the datastructures to store the instruction how to assemble
 * and disassemble a puzzle
 */

#include "bt_assert.h"

#include <deque>
#include <vector>

class xmlWriter_c;
class xmlParser_c;


/** the disassembly is a common base class for the
 * separation and separationInfo classes.
 */
class disassembly_c
{

  public:

    disassembly_c() {}
    virtual ~disassembly_c() {}

    /**
     * the number of moves to completely disassemble the puzzle, including
     * all sub separations
     */
    virtual unsigned int sumMoves(void) const = 0;
    /**
     * fill a string with dot separated numbers containing the moves
     * required to disassemble the puzzle
     * not more than len characters are written
     */
    virtual void movesText(char * txt, int len) const = 0;

    /**
     * compares this and the given separation, for a higher level.
     * one separation is bigger than the other if all levels
     * up to branch n are equal and on n+1 the level is larger then
     * the level of the other
     * if this assembly is bigger >0 is returned
     * if this assembler is smaller <0 is returned
     * if they are both equal =0 is returned
     */
    int compare(const disassembly_c * s2) const;

  protected:

    /**
     * helper function used for "compare" to get the number of moves for
     * the x-th place in the string
     */
    virtual unsigned int getSequenceLength(unsigned int x) const = 0;

    /** helper function used for "compare" to get the number of move sequences */
    virtual unsigned int getNumSequences(void) const = 0;

  private:

    // no copying and assigning
    disassembly_c(const disassembly_c&);
    void operator=(const disassembly_c&);
};


/**
 * defines one step in the separation process.
 * one state of relative piece positions on your way
 */
class state_c {

  /** contains the x positions of all the pieces that are handled */
  int *dx;
  /** contains the y positions of all the pieces that are handled */
  int *dy;
  /** contains the z positions of all the pieces that are handled */
  int *dz;

#ifndef NDEBUG
  /** we only keep the piecenumber for checking purposes */
  unsigned int piecenumber;
#endif

public:

  /**
   * create a new spate for pn pieces. you can not add more
   * pieces later on, so plan ahead
   */
  state_c(unsigned int pn);

  /** copy constructor */
  state_c(const state_c * cpy, unsigned int pn);

  /** load from an xml node */
  state_c(xmlParser_c & pars, unsigned int pn);

  ~state_c();

  /** save into an xml node */
  void save(xmlWriter_c & xml, unsigned int piecenumber) const;

  /** set the position of a piece */
  void set(unsigned int piece, int x, int y, int z);

  /** get the x position of a piece */
  int getX(unsigned int i) const {
    bt_assert(i < piecenumber);
    return dx[i];
  }
  /** get the y position of a piece */
  int getY(unsigned int i) const {
    bt_assert(i < piecenumber);
    return dy[i];
  }
  /** get the z position of a piece */
  int getZ(unsigned int i) const {
    bt_assert(i < piecenumber);
    return dz[i];
  }

  /** check, if the piece is removed in this state */
  bool pieceRemoved(unsigned int i) const;

#ifndef NDEBUG
  /** on assert needs to check the piecenumber */
  unsigned int getPiecenumber(void) const { return piecenumber; }
#endif

private:

  // no copying and assigning
  state_c(const state_c&);
  void operator=(const state_c&);
};


/**
 * A list of states that lead to a separation of the
 * puzzle into 2 parts.
 * The separation of a puzzle into lots of single pieces consists
 * of a tree of such separations. The root class starts with the assembled
 * puzzle and the lower parts divide the puzzle more and more until only
 * single pieces are left
 */
class separation_c : public disassembly_c
{

  /**
   * this array is here to identify the piecenumber for the given pieces
   * that are in this part of the tree
   */
  std::vector<unsigned int> pieces;

  /**
   * vector with all the states that finally lead to 2 separate
   * subpuzzles. one state represents the  position of
   * all the pieces inside this subpuzzle relative to their
   * position in the completely assembled puzzle
   *
   * the last state will move all the pieces, that can be removed
   * a long way out of the puzzle (more than 10000 units)
   *
   * the first state is the beginning state, for this partition, e.g
   * for the root node the first state represents the assembles puzzle
   * with all values 0
   */
  std::deque <state_c *> states;

  /* the 2 parts the puzzle gets divided with the
   * last move. If one of this parts consists of only
   * one piece there will be a null pointer
   */
  separation_c * removed, *left;

  /** used in movesText to find out if a branch has a movesequence longer than 1 */
  bool containsMultiMoves(void);

  /** the number of sequences this separation and all its sub
   * separations contain
   */
  unsigned int numSequences;

  /** helper function for movesTxt */
  int movesText2(char * txt, int len) const;

public:

  /**
   * create a separation with sub separations r and l
   * and the pieces in the array pcs
   */
  separation_c(separation_c * r, separation_c * l, const std::vector<unsigned int> & pcs);

  /** load a separation from an xml node */
  separation_c(xmlParser_c & pars, unsigned int pieces);

  /** copy constructor */
  separation_c(const separation_c * cpy);

  /* save into an xml node, please always call with just xml, the type is for internal use */
  void save(xmlWriter_c & xml, int type = 0) const;

  ~separation_c();

  /**
   * return the number of moves that are required to separate the puzzle.
   * this number is one smaller than the number of states
   */
  unsigned int getMoves(void) const { return states.size() - 1; }

  /** get one state from the separation process */
  const state_c * getState(unsigned int num) const {
    bt_assert(num < states.size());
    return states[num];
  }

  /** get the separation for the pieces that were removed */
  const separation_c * getLeft(void) const { return left; }

  /** get the separation for the pieces that were left over */
  const separation_c * getRemoved(void) const { return removed; }

  /**
   * add a new state to the FRONT of the current state list.
   * this is necessary because the list is generated when back
   * tracking from the graph search, so we visit the states in
   * the wrong order
   * keep in mind that the new state must have the same number
   * of pieces as all the other states
   */
  void addstate(state_c *st);

  /** return the number of pieces that are in this separation */
  unsigned int getPieceNumber(void) const { return pieces.size(); }

  /** get the number for the num-th piece that is in this separation */
  unsigned int getPieceName(unsigned int num) const {
    bt_assert(num < pieces.size());
    return pieces[num];
  }

  /** 2 pieces have exchanged their place in the problem list */
  void exchangeShape(unsigned int s1, unsigned int s2);

  /* implementation of the base class functions */
  virtual unsigned int getSequenceLength(unsigned int x) const;
  virtual unsigned int getNumSequences(void) const;
  virtual unsigned int sumMoves(void) const;
  virtual void movesText(char * txt, int len) const { movesText2(txt, len); }

  void removePieces(unsigned int from, unsigned int cnt);
  void addNonPlacedPieces(unsigned int from, unsigned int cnt);

private:

  // no copying and assigning
  separation_c(const separation_c&);
  void operator=(const separation_c&);
};

/**
 * this class is used to store the complexity of a disassembly. it
 * is used as a memory efficient replacement for the basic information
 * in separation_c without the detailed disassembly instructions
 */
class separationInfo_c : public disassembly_c {

  private:

    /**
     * this array contains the whole disassembly tree in prefix order, root, left, removed.
     * When a certain subtree is empty a zero is included, otherwise the number is the number
     * of states within this tree node
     *
     * example:
     * \verbatim
       3 2 1 0 0 0 0     tree root 3 --> 2 --> 1 \endverbatim
     *
     * another example
     *
     * \verbatim
       3 1 1 0 0 0 1 1 0 0 0   tree root  3 --> 1 --> 1
                                           \--> 1 --> 1 \endverbatim
     */
    std::vector<unsigned int> values;

    /** used in movesText to find out if a branch has a movesequence longer than 1 */
    bool containsMultiMoves(unsigned int root) const;

    /** used in constructor for create separationInfo from normal separation */
    void recursiveConstruction(const separation_c * sep);

    int movesText2(char * txt, int len, unsigned int idx) const;

  public:

    /** load separationInfo from parser */
    separationInfo_c(xmlParser_c & pars);

    /** create a separation infor from a normal separation */
    separationInfo_c(const separation_c * sep);

    /** save into an xml node */
    void save(xmlWriter_c & xml) const;

    /* implement abstract functions */
    virtual unsigned int sumMoves(void) const;
    virtual void movesText(char * txt, int len) const { movesText2(txt, len, 0); }
    virtual unsigned int getSequenceLength(unsigned int x) const;
    virtual unsigned int getNumSequences(void) const;

  private:

    // no copying and assigning
    separationInfo_c(const separationInfo_c&);
    void operator=(const separationInfo_c&);
};


#endif
