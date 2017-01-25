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

#include "lib/puzzle.h"
#include "lib/problem.h"
#include "assembler-interface.h"
#include "lib/assembly.h"
#include "disassembler-interface.h"
#include "simple-disassembler.h"
#include "lib/disassembly.h"
#include "lib/print.h"
#include "lib/voxel.h"
#include "lib/solution.h"
#include "tools/xml.h"
#include "tools/gzstream.h"

#include <stdlib.h>

#include <fstream>
#include <iostream>

using namespace std;

bool disassemble;
bool allProblems;
bool printDisassemble;
bool printSolutions;
bool quiet;

DisassemblerInterface * d;

class asm_cb : public AssemblerCallbackInterface {

public:

  int Assemblies;
  int Solutions;
  int pn;
  Problem * puzzle;

  asm_cb(Problem * p) : Assemblies(0), Solutions(0), pn(p->pieceNumber()), puzzle(p) {}

  bool assembly(Assembly * a) {


    Assemblies++;

    if (disassemble) {

      Separation * da = d->disassemble(a);

      if (da) {
        Solutions++;

        if (printSolutions)
          print(a, puzzle);

        if (!quiet || allProblems)
        {
          char lev[200];
          da->movesText(lev,200);
          printf("level: %s\n", lev);
        }

        if (printDisassemble)
          print(da, a, puzzle);
        delete da;
      }

    } else if (printSolutions)
      print(a, puzzle);

    delete a;

    return true;
  }
};

void usage() {

  cout << "burrTxt [options] file [options]\n\n";
  cout << "  file: puzzle file with the puzzle definition to solve\n\n";
  cout << "  -d    try to disassemble and only print solutions that do disassemble\n";
  cout << "  -p    print the disassembly plan\n";
  cout << "  -r    reduce the placements bevore starting to solve the puzzle\n";
  cout << "  -s    print the assemby\n";
  cout << "  -q    be quiet and only print statistics\n";
  cout << "  -n    don't print a newline at the end of the line\n";
  cout << "  -o n  select the problem to solve\n";
  cout << "  -o all solves all problems in file\n";
  cout << "  -x    only redisassemble the given solutions\n";
  cout << "  -a    ask for information about the current puzzle, the next letters must be:\n";
  cout << "     s0 print solutions with the only the used pieces\n";
  cout << "     s1 print solutions including the assemblies\n";
  cout << "     c  print comment\n";
}

int main(int argv, char* args[]) {

  if (argv < 1) {
    usage();
    return 2;
  }

  int state = 0;
  disassemble = false;
  allProblems = false;
  printDisassemble = false;
  printSolutions = false;
  quiet = false;
  bool assemble = true;
  unsigned int problem = 0;
  unsigned int firstProblem = 0;
  unsigned int lastProblem = 0;
  int filenumber = 0;
  bool reduce = false;
  bool newline = true;
  bool ask = false;
  enum {
    W_NUM_SOLUTIONS,
    W_SOLUTION_PIECES,
    W_SOLUTION_ASSM,
    W_COMMENT
  } what;

  for(int i = 1; i < argv; i++) {

    switch (state) {

    case 0:

      if (strcmp(args[i], "-d") == 0)
        disassemble = true;
      else if (strcmp(args[i], "-p") == 0)
        printDisassemble = true;
      else if (strcmp(args[i], "-s") == 0)
        printSolutions = true;
      else if (strcmp(args[i], "-r") == 0)
        reduce = true;
      else if (strcmp(args[i], "-n") == 0)
        newline = false;
      else if (strcmp(args[i], "-x") == 0)
        assemble = false;
      else if (strcmp(args[i], "-o") == 0) {
        if (strcmp(args[i+1],"all")==0)
          allProblems = true;
        else
          problem = atoi(args[i+1]);
        i++;
      } else if (strcmp(args[i], "-q") == 0) {
        quiet = true;
        printDisassemble = false;
        printSolutions = false;
      } else if (strcmp(args[i], "-a") == 0) {

        ask = true;
        what = W_NUM_SOLUTIONS;

        if (strcmp(args[i+1], "s0") == 0)
          what = W_SOLUTION_PIECES;
        else if (strcmp(args[i+1], "s1") == 0)
          what = W_SOLUTION_ASSM;
        else if (strcmp(args[i+1], "c") == 0)
          what = W_COMMENT;

        i++;

      } else
        filenumber = i;

      break;
    }
  }

  if (filenumber == 0) {
    usage();
    return 1;
  }

  std::istream * str = openGzFile(args[filenumber]);
  XmlParser pars(*str);
  Puzzle p(pars);
  delete str;

  if (ask) {

    switch (what) {
      case W_COMMENT:
        printf("%s\n", p.getComment().c_str());
        break;
      case W_NUM_SOLUTIONS:
        for (unsigned int i = 0; i < p.problemNumber(); i++)
          printf("number of solutions for problem %i: %li\n", i, p.getProblem(i)->getNumSolutions());
        break;
      case W_SOLUTION_PIECES:
      case W_SOLUTION_ASSM:
        for (unsigned int i = 0; i < p.problemNumber(); i++) {
          printf("problem %i\n", i);
          for (unsigned int s = 0; s < p.getProblem(i)->getNumSolutions(); s++) {

            printf("%03i: ", s+1);
            const Assembly * a = p.getProblem(i)->getSolution(s)->getAssembly();

            unsigned int pnum = 0;

            for (unsigned int pie = 0; pie < p.getProblem(i)->partNumber(); pie++) {
              for (unsigned int pp = 0; pp < p.getProblem(i)->getShapeMax(pie); pp++) {
                if (a->isPlaced(pnum)) {
                  printf("S%i ", p.getProblem(i)->getShape(pie)+1);
                }
                pnum++;
              }
            }

            printf("\n");
            if (what == W_SOLUTION_ASSM)
              print(a, p.getProblem(i));
          }
        }


        break;

    }

    return 0;
  }

  if (allProblems)
    {
      firstProblem = 0;
      lastProblem = p.problemNumber();
    }
  else
    {
      firstProblem = problem;
      lastProblem = problem+1;
    }
  if (assemble) {

    for (unsigned int i = 0; i < p.shapeNumber(); i++)
      p.getShape(i)->initHotspot();

    if (!quiet) {
      cout << " The puzzle:\n\n";
      print(&p);
    }




    for (unsigned int pr = firstProblem ; pr < lastProblem ; pr++) {

      Problem * problem = p.getProblem(pr);

      AssemblerInterface *assm = p.getGridType()->findAssembler(problem);

      switch (assm->createMatrix(problem, false, false, false)) {
      case AssemblerInterface::ERR_TOO_MANY_UNITS:
        printf("%i units too many for the result shape\n", assm->getErrorsParam());
        return 0;
      case AssemblerInterface::ERR_TOO_FEW_UNITS:
        printf("%i units too few for the result shape\n", assm->getErrorsParam());
        return 0;
      case AssemblerInterface::ERR_CAN_NOT_PLACE:
        printf("Piece %i can be place nowhere in the result shape\n", assm->getErrorsParam());
        return 0;
      case AssemblerInterface::ERR_NONE:
        /* no error case */
        break;
      case AssemblerInterface::ERR_PUZZLE_UNHANDABLE:
        printf("The puzzles contains features not yet supported by burrTxt\n");
        return 0;
      case AssemblerInterface::ERR_CAN_NOT_RESTORE_VERSION:
      case AssemblerInterface::ERR_CAN_NOT_RESTORE_SYNTAX:
        /* all other errors should not occur */
        printf("Oops internal error\n");
        return 0;
      }

      if (reduce) {
        if (!quiet)
          cout << "start reduce\n\n";
        assm->reduce();
        if (!quiet)
          cout << "finished reduce\n\n";
      }

      if (allProblems)
        cout << "problem: " << problem->getName() << endl;

      asm_cb a(problem);

      d = 0;
      if (disassemble)
        d = new SimpleDisassembler(problem);

      assm->assemble(&a);

      cout << a.Assemblies << " assemblies and " << a.Solutions << " solutions found with " << assm->getIterations() << " iterations ";

      if (newline)
        cout << endl;

      delete assm;
      delete d;
      d = 0;
      assm = 0;
    }
  } else {

    for (unsigned int pr = firstProblem ; pr < lastProblem; pr ++) {

      Problem * problem = p.getProblem(pr);

      d = new SimpleDisassembler(problem);

      for (unsigned int sol = 0; sol < problem->solutionNumber(); sol++) {

        if (problem->getSolution(sol)->getAssembly()) {

          Separation * da = d->disassemble(problem->getSolution(sol)->getAssembly());

          if (da) {
            if (printSolutions)
              print(problem->getSolution(sol)->getAssembly(), problem);

            if (!quiet)
              printf("level: %i\n", da->getMoves());

            if (printDisassemble)
              print(da, problem->getSolution(sol)->getAssembly(),problem);
            delete da;
          }
        }
      }

      delete d;
    }
  }

  return 0;
}


