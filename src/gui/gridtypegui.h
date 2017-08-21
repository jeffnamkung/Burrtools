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

#ifndef __GRID_TYPE_GUI_H__
#define __GRID_TYPE_GUI_H__

#include "Layouter.h"

class GridType;
class GuiGridType;

/* this is a fltk group that contains all elements to change the
 * parameters that a certain gridtype can have
 *
 * in fact this is only the base class, for each gridtype there will be
 * an inherited class that contains the elements
 */
class gridTypeGui_c : public Layouter {
};

/* the required elements for the brick grid type */
class gridTypeGui_0_c : public gridTypeGui_c {

 public:

  gridTypeGui_0_c(int x, int y, int w, int h, GridType *gt);
};

class gridTypeGui_1_c : public gridTypeGui_c {

 public:

  gridTypeGui_1_c(int x, int y, int w, int h, GridType *gt);
};

class gridTypeGui_2_c : public gridTypeGui_c {

 public:

  gridTypeGui_2_c(int x, int y, int w, int h, GridType *gt);
};

/* this window allows you to edit the parameters of one
 * grid type
 */
class gridTypeParameterWindow_c : public LFl_Double_Window {

 public:

  /* creates the parameter window for the given gui grid type */
  gridTypeParameterWindow_c(GuiGridType *ggt);
};

/* this class is used in the window below to hold all data necessary
 * for one grid type
 */
class GridTypeInfo;

/* this window allows you to select the grid type and
 * the parameters for the selected type
 */

class GridTypeSelectorWindow : public LFl_Double_Window {
 public:
  GridTypeSelectorWindow();
  ~GridTypeSelectorWindow() override;

  /* after the window has been close you can get the created grid type with this function
   */
  std::unique_ptr<GridType> getGridType();

  void select_cb();

 private:
  std::vector<std::unique_ptr<GridTypeInfo>> grid_type_infos_;

  /* currently selected grid type from the vector above */
  GridTypeInfo* current_grid_type_info_ = nullptr;
};

#endif
