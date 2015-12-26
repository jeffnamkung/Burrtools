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

#define BT_ASSERT_NO_FUNC

#include "../bitfield.h"

#include "tablesizes.inc"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double matrix[NUM_TRANSFORMATIONS_MIRROR][9] = {
#include "rotmatrix.inc"
};

/* this matix contains the concatenation of 2 transformations
 * if you first transform the piece around t1 and then around t2
 * you can as well transform around transMult[t1][t2]
 */
static int transMult[NUM_TRANSFORMATIONS_MIRROR][NUM_TRANSFORMATIONS_MIRROR];

/* this array contains all possible symmetry groups, meaning bitmasks with exactly the bits set
 * that correspond to transformations tha reorient the piece so that it looks identical
 */
static const bitfield_c<NUM_TRANSFORMATIONS_MIRROR> symmetries[NUM_SYMMETRY_GROUPS] = {
#include "symmetries.inc"
};

const char * longlong2string(unsigned long long s) {
  char hex[17] = "0123456789ABCDEF";
  static char output[100];


  for (int i = 0; i < 12; i++)
     output[11-i] = hex[(s >> (4*i)) & 0xF];

  output[12] = 0;

  return output;
}

unsigned char ssss(int trans, const bitfield_c<NUM_TRANSFORMATIONS_MIRROR> & s) {
  for (int t = 0; t < trans; t++)
    for (int t2 = 0; t2 < NUM_TRANSFORMATIONS_MIRROR; t2++)
      if (s.get(t2)) {
        int trrr = transMult[t2][t];
        if (trrr == trans) {
          return t;
        }
      }

  return trans;
}


/* this function creates the lookup table for the function in the symmetry c-file */
void outputMinimumSymmetries() {

  FILE * out = fopen("transformmini.inc", "w");

  for (int sy = 0; sy < NUM_SYMMETRY_GROUPS; sy++) {
    fprintf(out, "  {");
    for (int trans = 0; trans < NUM_TRANSFORMATIONS_MIRROR; trans++) {
      fprintf(out,"%2i", ssss(trans, symmetries[sy]));
      if (trans < NUM_TRANSFORMATIONS_MIRROR-1)
        fprintf(out, ",");
    }
    if (sy < NUM_SYMMETRY_GROUPS-1)
      fprintf(out, "},\n");
    else
      fprintf(out, "}\n");
  }

  fclose(out);
}

/* this function creates a table that contains ALL bits for the symmetries
* instead of only the ones for the current rotation, that means all bits
* are set that where the shape reproduces itself in one of the 48 possible
* rotations
*/

void outputCompleteSymmetries() {

  FILE * fout = fopen("unifiedsym.inc", "w");

  for (int sy = 0; sy < NUM_SYMMETRY_GROUPS; sy++) {

    bitfield_c<NUM_TRANSFORMATIONS_MIRROR> s = symmetries[sy];

    bitfield_c<NUM_TRANSFORMATIONS_MIRROR> out = s;

    for (int r = 1; r < NUM_TRANSFORMATIONS_MIRROR; r++) {

      int rinv = 0;

      while (transMult[r][rinv]) {
        rinv++;
      }

      // if r + x + rinv one of the symmetries of the initial shape
      // then x is a symmetriy of the by r rotated shape

      for (int x = 0; x < NUM_TRANSFORMATIONS_MIRROR; x++) {
        int res = transMult[r][x];

        if (res == -1) continue;

        res = transMult[res][rinv];

        if (res == -1) continue;

        if (s.get(res)) {
          out.set(x);
        }
      }

    }
    char line[100];
    out.print(line, 100);


    fprintf(fout, "  \"%s\",\n", line+4);
  }

  fclose(fout);
}

/* this function only one bit is set for each possible transformation that results in one possible orientation
 * of the shape
 */
void outputUniqueSymmetries() {

  FILE * fout = fopen("uniquesym.inc", "w");

  for (int sy = 0; sy < NUM_SYMMETRY_GROUPS; sy++) {

    bitfield_c<NUM_TRANSFORMATIONS_MIRROR> s = symmetries[sy];
    bitfield_c<NUM_TRANSFORMATIONS_MIRROR> ttt;
    bitfield_c<NUM_TRANSFORMATIONS_MIRROR> out;

    for (int r = 0; r < NUM_TRANSFORMATIONS_MIRROR; r++)
    {
      if (!ttt.get(r))
      {
        out.set(r);

        for (int r2 = 0; r2 < NUM_TRANSFORMATIONS_MIRROR; r2++)
        {
          if (s.get(r2))
          {
            if (transMult[r2][r] >= 0)
              ttt.set(transMult[r2][r]);
          }
        }
      }
    }
    char line[100];
    out.print(line, 100);

    fprintf(fout, "  \"%s\",\n", line+4);
  }

  fclose(fout);
}



/* this function creates a decision tree for symmetry creation trying to optimize for the
 * lowest number of checks (6-7 should be possible, if we can subdivide each time#
 * with nearly equal subparts
 */
void makeSymmetryTree(bitfield_c<NUM_TRANSFORMATIONS_MIRROR> taken,bitfield_c<NUM_TRANSFORMATIONS_MIRROR> val, FILE * out) {

  /* greedy implementation: find the subdivision that is most equal */
  int best_div = -100;
  int best_bit = 0;
  int b1, b2;

  for (int t = 0; t < NUM_TRANSFORMATIONS_MIRROR; t++)
    if (!taken.get(t)) {
      b1 = 0;
      b2 = 0;
      int lastfound = 0;
      for (int s = 0; s < NUM_SYMMETRY_GROUPS; s++) {
        if ((symmetries[s] & taken) == val) {
          b1++;
          lastfound = s;
        }
        taken.set(t);
        if ((symmetries[s] & taken) == val)
          b2++;
        taken.reset(t);
      }

      if (b1 == 1) {
        fprintf(out, "return (symmetries_t)%i;\n", lastfound);
        return;
      }

      if ((b2 > 0) && (b1-b2>0) && (abs(b1/2 - best_div) > abs(b1/2 - b2))) {
        best_div = b2;
        best_bit = t;
      }
    }

  fprintf(out, "voxel_2_c v(pp);\nif (v.transform(%i) && pp->identicalInBB(&v)) {\n", best_bit);

  taken.set(best_bit);
  val.set(best_bit);
  makeSymmetryTree(taken, val, out);
  val.reset(best_bit);

  fprintf(out, "} else {\n");

  makeSymmetryTree(taken, val, out);

  fprintf(out, "}\n");

}

void mmult(double * m, int num) {

  double n[9];

  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++) {
      n[y*3+x] = 0;
      for (int c = 0; c < 3; c++) {
        n[y*3+x] += m[c*3+x]*matrix[num][y*3+c];
      }
    }

  for (int i = 0; i < 9; i++)
    m[i] = n[i];
}

bool mequal(double *m, int num) {

  for (int i = 0; i < 9; i++)
    if (fabs(m[i]-matrix[num][i]) > 0.00001)
      return false;

  return true;
}


void multTranformationsMatrix() {

  FILE * out = fopen("transmult.inc", "w");

  for (int tr1 = 0; tr1 < NUM_TRANSFORMATIONS_MIRROR; tr1++) {
    fprintf(out, "{");


    for (int tr2 = 0; tr2 < NUM_TRANSFORMATIONS_MIRROR; tr2++) {

      double m[9];
      m[0] = m[4] = m[8] = 1;
      m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0;

      mmult(m, tr1);
      mmult(m, tr2);

      bool found = false;

      for (int t = 0; t < NUM_TRANSFORMATIONS_MIRROR; t++) {

        if (mequal(m, t)) {
          if (tr2 < NUM_TRANSFORMATIONS_MIRROR-1)
            fprintf(out, "%3i,", t);
          else
            fprintf(out, "%3i", t);
          transMult[tr1][tr2] = t;
          found = true;
          break;
        }
      }

      if (!found) {
        if (tr2 < NUM_TRANSFORMATIONS_MIRROR-1)
          fprintf(out, "TND,");
        else
          fprintf(out, "TND");
        transMult[tr1][tr2] = -1;
      }

    }
    if (tr1 < NUM_TRANSFORMATIONS_MIRROR-1)
      fprintf(out, "},\n");
    else
      fprintf(out, "}\n");
  }
  fclose(out);
}

int main(int /*argv*/, char** /*args[]*/) {

  multTranformationsMatrix();
  outputMinimumSymmetries();
  outputCompleteSymmetries();
  outputUniqueSymmetries();

  FILE * out = fopen("symcalc.inc", "w");
  makeSymmetryTree("0", "0", out);
  fclose(out);
}

