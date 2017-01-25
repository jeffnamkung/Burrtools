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
#ifndef __BUTTON_GROUP_H__
#define __BUTTON_GROUP_H__

#include "Layouter.h"

// a group that can contain only buttons and one button is
// pressed while others are not
class ButtonGroup : public Layouter {

  unsigned int currentButton;

public:

  ButtonGroup(int x, int y, int w, int h);

  LFl_Button * addButton(void);

  void cb_Push(Fl_Button * btn);

  unsigned int getSelected(void) const { return currentButton; }
  void select(int num);
};

#endif
