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
#include "lib/solvethread.h"
#include "lib/voxel.h"
#include "tools/xml.h"
#include "tools/gzstream.h"

#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#ifdef WIN32
#include <winsock.h>
#endif

using namespace std;

void usage() {
  cout << R"(
burrTxt [options] file [options]
  file: puzzle file with the puzzle definition to solve
  -R    restart and throw away all found solutions, otherwise continue
  -d    try to disassemble and only keep solutions that do disassemble
  -c    just count solutions
  -m    keep mirror solutions
  -r    keep rotated solutions
  -p    drop disassemblies and replace by information about disassembly
  -b    selecte problem, else 0)";
}


bool checkInput() {
  /* Initialize the file descriptor set. */
  fd_set set;
  FD_ZERO(&set);
  FD_SET(STDIN_FILENO, &set);

  /* Initialize the timeout data structure. */
  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  /* select returns 0 if timeout, 1 if input available, -1 if error. */
  return select(FD_SETSIZE, &set, NULL, NULL, &timeout);
}


int main(int argv, char* args[]) {
  if (argv < 1) {
    usage();
    return 2;
  }

  int par = SolveThread::PAR_REDUCE;
  bool restart = false;
  int filenumber = 0;
  int firstProblem = 0;
  int lastProblem = 1;

  for(int i = 1; i < argv; i++) {

    if (strcmp(args[i], "-R") == 0)
      restart = true;
    else if (strcmp(args[i], "-d") == 0)
      par |= SolveThread::PAR_DISASSM;
    else if (strcmp(args[i], "-c") == 0)
      par |= SolveThread::PAR_JUST_COUNT;
    else if (strcmp(args[i], "-m") == 0)
      par |= SolveThread::PAR_KEEP_MIRROR;
    else if (strcmp(args[i], "-r") == 0)
      par |= SolveThread::PAR_KEEP_ROTATIONS;
    else if (strcmp(args[i], "-p") == 0)
      par |= SolveThread::PAR_DROP_DISASSEMBLIES;
    else if (strcmp(args[i], "-b") == 0) {
      firstProblem = atoi(args[i+1]);
      lastProblem = firstProblem + 1;
      i++;
    }
    else
      filenumber = i;
  }

  if (filenumber == 0) {
    usage();
    return 1;
  }

  std::istream * str = openGzFile(args[filenumber]);
  XmlParser pars(*str);
  Puzzle p(pars);
  delete str;

  std::string outname = args[filenumber];
  outname += "ttt";
  cout << "outputting into " << outname << std::endl;

  ofstream ostr(outname.c_str());
  if (!ostr)
  {
    cout << "Can not open output file, aborting\n";
    return 2;
  }

  for (unsigned int i = 0; i < p.shapeNumber(); i++)
    p.getShape(i)->initHotspot();


  for (int pr = firstProblem ; pr < lastProblem ; pr++) {

    if (restart)
      p.getProblem(pr)->removeAllSolutions();


    SolveThread assmThread(p.getProblem(pr), par);

    if (!assmThread.start(false)) {
      cout << "Could not start Solver\n";
      continue;
    }

    while (assmThread.currentAction() != SolveThread::ACT_FINISHED &&
        assmThread.currentAction() != SolveThread::ACT_ERROR) {

      if (checkInput()) {
        cout << "abborting \n";
        assmThread.stop();

        while (assmThread.currentAction() != SolveThread::ACT_FINISHED &&
            assmThread.currentAction() != SolveThread::ACT_ERROR &&
            assmThread.currentAction() != SolveThread::ACT_PAUSING)
#ifdef WIN32
          Sleep(1);
#else
          sleep(1);
#endif

        break;
      }

      if (assmThread.currentAction() == SolveThread::ACT_ERROR) {
        cout << "Exception in Solver\n";
        cout << " file      : " << assmThread.getAssertException().file;
        cout << " function  : " << assmThread.getAssertException().function;
        cout << " line      : " << assmThread.getAssertException().line;
        cout << " expression: " << assmThread.getAssertException().expr;
        return 1;
      }


      float finished = (p.getProblem(pr)->getAssembler())
        ? p.getProblem(pr)->getAssembler()->getFinished()
        : 0;

      switch (assmThread.currentAction()) {
        case SolveThread::ACT_PREPARATION:
          cout << "\rpreparing piece " << assmThread.currentActionParameter()+1;
          break;
        case SolveThread::ACT_REDUCE:
          cout << "\rreducing piece " << assmThread.currentActionParameter()+1;
          break;
        case SolveThread::ACT_ASSEMBLING:
          cout << "\rassembling " << finished*100 << "% done";
          break;
        case SolveThread::ACT_DISASSEMBLING:
          cout << "\rdisassembling " << finished*100 << "% done";
          break;
        case SolveThread::ACT_WAIT_TO_STOP:
          cout << "\rwaitin";
          break;
        case SolveThread::ACT_ERROR:
          cout << "\rerror: ";
          switch (assmThread.getErrorState()) {
            case AssemblerInterface::ERR_TOO_MANY_UNITS:
              cout << "Pieces contain " << assmThread.getErrorParam() << " units too many\n";
              break;
            case AssemblerInterface::ERR_TOO_FEW_UNITS:
              cout << "Pieces contain " << assmThread.getErrorParam() << " units less than required\n";
              break;
            case AssemblerInterface::ERR_CAN_NOT_PLACE:
              cout << "Piece " << assmThread.getErrorParam()+1 << " can be placed nowhere within the result";
              break;
            case AssemblerInterface::ERR_CAN_NOT_RESTORE_VERSION:
              cout << "Impossible to restore the saved state because the internal format changed.\n";
              cout << "You either have to start from the beginning or finish with the old version of BurrTools, sorry";
              break;
            case AssemblerInterface::ERR_CAN_NOT_RESTORE_SYNTAX:
              cout << "Impossible to restore the saved state because something with the data is wrong.\n";
              cout << "You have to start from the beginning, sorry";
              break;
            case AssemblerInterface::ERR_PUZZLE_UNHANDABLE:
              cout << "Something went wrong the program can not solve your puzzle definitions.\n";
              cout << "You should send the puzzle file to the programmer!";
              break;
            case AssemblerInterface::ERR_NONE:
              break;
          }
          break;
        case SolveThread::ACT_FINISHED:
          cout << "\rdone";
          break;
      }

      cout.flush();

    }
  }

  XmlWriter xml(ostr);
  p.save(xml);

  return 0;
}


