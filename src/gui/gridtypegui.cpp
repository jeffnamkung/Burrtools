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

#include "gridtypegui.h"

#include <memory>

#include "guigridtype.h"

gridTypeGui_0_c::gridTypeGui_0_c(int x,
                                 int y,
                                 int w,
                                 int h,
                                 GridType * /*gt*/) {

  new LFl_Box("In a distant future there might be parameters\n"
                  "to stretch and shear the cube into a parallelepiped\n"
                  "but not right now!!", x, y, w, h);

  end();
}

gridTypeGui_1_c::gridTypeGui_1_c(int x,
                                 int y,
                                 int w,
                                 int h,
                                 GridType * /*gt*/) {
  new LFl_Box("There are no parameters for this space grid!\n"
                  "This space grid also has no disassembler (yet)", x, y, w, h);

  end();
}

gridTypeGui_2_c::gridTypeGui_2_c(int x,
                                 int y,
                                 int w,
                                 int h,
                                 GridType * /*gt*/) {

  new LFl_Box("There are no parameters for this space grid!\n"
                  "This space grid has no disassembler", x, y, w, h);

  end();
}

class GridTypeInfo {
 public:
  explicit GridTypeInfo(std::unique_ptr<GridType> grid_type)
      : grid_type_(std::move(grid_type)),
        ggt(new GuiGridType(grid_type_.get())) {}

  ~GridTypeInfo() {
    if (ggt)
      delete ggt;
  }

  std::unique_ptr<GridType> grid_type_;
  GuiGridType *ggt;
  LFl_Radio_Button *btn;
  gridTypeGui_c *gui;

};

static void cb_WindowButton_stub(Fl_Widget * /*o*/,
                                 void *v) { ((Fl_Double_Window *) (v))->hide(); }

gridTypeParameterWindow_c::gridTypeParameterWindow_c(GuiGridType *ggt)
    : LFl_Double_Window(false) {
  label("Set parameters for grid type");

  ggt->getConfigurationDialog(0, 0, 1, 1);

  LFl_Button *b = new LFl_Button("Close", 0, 1);
  b->pitch(7);
  b->callback(cb_WindowButton_stub, this);
}

static void cb_gridTypeSelectorSelect_stub(Fl_Widget * /*o*/,
                                           void *v) { ((GridTypeSelectorWindow *) (v))->select_cb(); }
void GridTypeSelectorWindow::select_cb() {
  for (auto& grid_type_info : grid_type_infos_) {
    if (grid_type_info->btn->value()) {
      auto* current_grid_type_info = current_grid_type_info_;
      current_grid_type_info->gui->hide();
      current_grid_type_info_ = grid_type_info.get();
      current_grid_type_info->gui->show();
    }
  }
}

GridTypeSelectorWindow::GridTypeSelectorWindow(void)
    : LFl_Double_Window(false) {

  /* for each grid type available we need to create an instance
   * here and put it into a gridtypeinfo class and into the
   * vector. This vector will be later on the one
   * with all required information
   */
  for (int i = 0; i < GridType::GT_NUM_GRIDS; i++) {
    grid_type_infos_.push_back(std::make_unique<GridTypeInfo>(
        std::make_unique<GridType>(GridType::Type(i))));
  }

  /* from here on the code should not need changes when new grid types are added */

  label("Select space grid");

  LFl_Frame *fr;

  /* first the selector for all grid types */
  {
    fr = new LFl_Frame(0, 0, 1, 2);

    for (unsigned int i = 0; i < grid_type_infos_.size(); i++) {
      grid_type_infos_[i]->btn =
          new LFl_Radio_Button(grid_type_infos_[i]->ggt->getName(), 0, i);
      grid_type_infos_[i]->btn->callback(cb_gridTypeSelectorSelect_stub, this);
      if (i == 0)
        grid_type_infos_[i]->btn->set();
    }

    (new LFl_Box(0, grid_type_infos_.size()))->weight(0, 100);

    fr->end();
  }

  (new LFl_Box("Attention: Concrete values within the\n"
                   "parameters are never evaluated, the solver only\n"
                   "cares for equal and not equal. So if you want something\n"
                   "that is twice as high as wide, assemble is out of\n"
                   "several unit pieces.", 1, 0))->pitch(7);

  /* now all the parameter boxes, only the first one is visible, the others are hidden for now */
  {
    fr = new LFl_Frame(1, 1);

    for (unsigned int i = 0; i < grid_type_infos_.size(); i++) {
      grid_type_infos_[i]->gui =
          grid_type_infos_[i]->ggt->getConfigurationDialog(0, 0, 1, 1);

      if (i != 0)
        grid_type_infos_[i]->gui->hide();
    }

    fr->end();
  }

  /* finally the 3D view that contains exactly one voxel */


  /* now the buttons */
  {
    Layouter *l = new Layouter(0, 2, 3, 1);

    LFl_Button *b = new LFl_Button("OK", 0, 0);
    b->pitch(7);
    b->callback(cb_WindowButton_stub, this);

    l->end();
  }

  current_grid_type_info_ = grid_type_infos_[0].get();
}

GridTypeSelectorWindow::~GridTypeSelectorWindow() {}

std::unique_ptr<GridType> GridTypeSelectorWindow::getGridType() {
  return std::move(current_grid_type_info_->grid_type_);
}

