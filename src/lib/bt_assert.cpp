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

/* this module contains an assert that throws an exception with information about
 * the assert, this exception can be caught at the end of the program and
 * the information displayed
 */
#include "bt_assert.h"

assert_log_c *assert_log;

void bt_assert_init() {
  assert_log = new assert_log_c();
}

void bt_te(const char *expr,
           const char *file,
           unsigned int line,
           const char *function) {
  throw assert_exception(expr, file, line, function);
}

