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
#include "mainwindow.h"

#include "configuration.h"
#include "groupseditor.h"
#include "placementbrowser.h"
#include "movementbrowser.h"
#include "imageexport.h"
#include "stlexport.h"
#include "guigridtype.h"
#include "grideditor.h"
#include "gridtypegui.h"
#include "statuswindow.h"
#include "tooltabs.h"
#include "WindowWidgets.h"
#include "BlockList.h"
#include "Images.h"

#include "multilinewindow.h"
#include "assertwindow.h"
#include "togglebutton.h"
#include "voxeleditgroup.h"
#include "view3dgroup.h"
#include "statusline.h"
#include "resultviewer.h"
#include "separator.h"
#include "buttongroup.h"
#include "constraintsgroup.h"
#include "blocklistgroup.h"
#include "vectorexportwindow.h"
#include "convertwindow.h"
#include "assmimportwindow.h"

#include "LFl_Tile.h"

#include "../config.h"

#include "../lib/ps3dloader.h"
#include "../lib/voxel.h"
#include "../lib/puzzle.h"
#include "../lib/problem.h"
#include "../lib/assembler-interface.h"
#include "../lib/solvethread.h"
#include "../lib/disassembly.h"
#include "../lib/simple-disassembler.h"
#include "../lib/grid-type.h"
#include "../lib/disasmtomoves.h"
#include "../lib/assembly.h"
#include "../lib/converter.h"
#include "../lib/millable.h"
#include "../lib/voxeltable.h"
#include "../lib/solution.h"

#include "../tools/gzstream.h"
#include "../tools/xml.h"

#include "../flu/Flu_File_Chooser.h"

#include "../help/Fl_Help_Dialog.h"

#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Value_Input.H>
#include <FL/fl_ask.H>

#include <fstream>

/* returns true, if file exists, this is not the
 optimal way to do this. It would be better to open
 the directory the file is supposed to be in and look there
 but this is not really portable so this
 */
bool fileExists(const char *n) {

  FILE *f = fopen(n, "r");

  if (f) {
    fclose(f);
    return true;
  } else
    return false;
}

static void cb_AddColor_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AddColor(); }
void MainWindow::cb_AddColor() {

  unsigned char r, g, b;

  if (colorSelector->getSelection() == 0)
    r = g = b = 128;
  else
    puzzle_->getColor(colorSelector->getSelection()-1, &r, &g, &b);

  if (fl_color_chooser("New colour", r, g, b)) {
    puzzle_->addColor(r, g, b);

    // add this color as a default to the matrix, so that
    // pieces of color x can be plces into cubes of color x
    int col = puzzle_->colorNumber();
    for (unsigned int p = 0; p < puzzle_->problemNumber(); p++)
      puzzle_->getProblem(p)->allowPlacement(col, col);

    colorSelector->setSelection(puzzle_->colorNumber());
    changed = true;
    View3D->getView()->showColors(*puzzle_, StatusLine->getColorMode());
    updateInterface();
  }
}

static void cb_RemoveColor_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_RemoveColor(); }
void MainWindow::cb_RemoveColor() {

  if (colorSelector->getSelection() == 0)
    fl_message("Can not delete the Neutral colour, this colour has to be there");
  else {
    changeColor(colorSelector->getSelection());
    puzzle_->removeColor(colorSelector->getSelection());

    unsigned int current = colorSelector->getSelection();

    while ((current > 0) && (current > puzzle_->colorNumber()))
      current--;

    colorSelector->setSelection(current);

    changed = true;
    View3D->getView()->showColors(*puzzle_, StatusLine->getColorMode());
    activateShape(PcSel->getSelection());
    updateInterface();
  }
}

static void cb_ChangeColor_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ChangeColor(); }
void MainWindow::cb_ChangeColor() {

  if (colorSelector->getSelection() == 0)
    fl_message("Can not edit the Neutral colour");
  else {
    unsigned char r, g, b;
    puzzle_->getColor(colorSelector->getSelection()-1, &r, &g, &b);
    if (fl_color_chooser("Change colour", r, g, b)) {
      puzzle_->changeColor(colorSelector->getSelection()-1, r, g, b);
      changed = true;
      View3D->getView()->showColors(*puzzle_, StatusLine->getColorMode());
      updateInterface();
    }
  }
}

static void cb_NewShape_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_NewShape(); }
void MainWindow::cb_NewShape() {

  if (PcSel->getSelection() < puzzle_->shapeNumber()) {
    const Voxel * v = puzzle_->getShape(PcSel->getSelection());
    PcSel->setSelection(puzzle_->addShape(v->getX(), v->getY(), v->getZ()));
  } else
    PcSel->setSelection(puzzle_->addShape(gui_grid_type_->defaultSize(), gui_grid_type_->defaultSize(), gui_grid_type_->defaultSize()));
  pieceEdit->setZ(0);
  updateInterface();
  StatPieceInfo(PcSel->getSelection());
  changed = true;
}

static void cb_DeleteShape_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteShape(); }
void MainWindow::cb_DeleteShape() {

  unsigned int current = PcSel->getSelection();

  if (current < puzzle_->shapeNumber()) {

    puzzle_->removeShape(current);

    if (puzzle_->shapeNumber() == 0)
      current = (unsigned int)-1;
    else
      while (current >= puzzle_->shapeNumber())
        current--;

    activateShape(current);

    PcSel->setSelection(current);
    updateInterface();
    StatPieceInfo(PcSel->getSelection());

    changed = true;

  } else

    fl_message("No shape to delete selected!");

}

static void cb_CopyShape_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_CopyShape(); }
void MainWindow::cb_CopyShape() {

  unsigned int current = PcSel->getSelection();

  if (current < puzzle_->shapeNumber()) {

    PcSel->setSelection(puzzle_->addShape(puzzle_->getGridType()->getVoxel(puzzle_->getShape(current))));
    changed = true;

    updateInterface();
    StatPieceInfo(PcSel->getSelection());

  } else

    fl_message("No shape to copy selected!");

}

static void cb_NameShape_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_NameShape(); }
void MainWindow::cb_NameShape() {

  if (PcSel->getSelection() < puzzle_->shapeNumber()) {

    const char * name = fl_input("Enter name for the shape", puzzle_->getShape(PcSel->getSelection())->getName().c_str());

    if (name) {
      puzzle_->getShape(PcSel->getSelection())->setName(name);
      changed = true;
      updateInterface();
    }
  }
}

static void cb_WeightInc_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_WeightChange(1); }
static void cb_WeightDec_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_WeightChange(-1); }
void MainWindow::cb_WeightChange(int by) {

  if (PcSel->getSelection() < puzzle_->shapeNumber()) {

    Voxel * v = puzzle_->getShape(PcSel->getSelection());
    v->setWeight(v->getWeight() + by);
    changed = true;
    updateInterface();
  }
}


static void cb_TaskSelectionTab_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_TaskSelectionTab((Fl_Tabs*)o); }
void MainWindow::cb_TaskSelectionTab(Fl_Tabs* o) {

  if (o->value() == TabPieces) {
    activateShape(PcSel->getSelection());
    StatPieceInfo(PcSel->getSelection());
    if (!shapeEditorWithBig3DView)
      Small3DView();
    else
      Big3DView();
    ViewSizes[currentTab] = View3D->getZoom();
    if (ViewSizes[0] >= 0)
      View3D->setZoom(ViewSizes[0]);
    currentTab = 0;
  } else if(o->value() == TabProblems) {

    // make sure the selector has a valid problem selected, when there is one
    if (puzzle_->problemNumber() && (problemSelector->getSelection() >= puzzle_->problemNumber()))
      problemSelector->setSelection(puzzle_->problemNumber()-1);

    if (problemSelector->getSelection() < puzzle_->problemNumber()) {
      activateProblem(problemSelector->getSelection());
    }
    StatProblemInfo(problemSelector->getSelection());
    Big3DView();
    ViewSizes[currentTab] = View3D->getZoom();
    if (ViewSizes[1] >= 0)
      View3D->setZoom(ViewSizes[1]);
    currentTab = 1;
  } else if(o->value() == TabSolve) {

    // make sure the selector has a valid problem selected, when there is one
    if (puzzle_->problemNumber() && (solutionProblem->getSelection() >= puzzle_->problemNumber()))
      solutionProblem->setSelection(puzzle_->problemNumber()-1);

    if ((solutionProblem->getSelection() < puzzle_->problemNumber()) &&
        (SolutionSel->value()-1 < puzzle_->getProblem(solutionProblem->getSelection())->solutionNumber())) {
      activateSolution(solutionProblem->getSelection(), int(SolutionSel->value()-1));
    }
    Big3DView();
    StatusLine->setText("");
    ViewSizes[currentTab] = View3D->getZoom();
    if (ViewSizes[2] >= 0)
      View3D->setZoom(ViewSizes[2]);
    currentTab = 2;
  }

  updateInterface();
}

static void cb_TransformPiece_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_TransformPiece(); }
void MainWindow::cb_TransformPiece() {

  if (pieceTools->operationToAll()) {
    for (unsigned int i = 0; i < puzzle_->shapeNumber(); i++)
      changeShape(i);
  } else {
    changeShape(PcSel->getSelection());
  }

  StatPieceInfo(PcSel->getSelection());
  activateShape(PcSel->getSelection());

  changed = true;
}

static void cb_EditSym_stub(Fl_Widget* o, void* v) {
  ((MainWindow*)v)->cb_EditSym(((LToggleButton_c*)o)->value(), ((LToggleButton_c*)o)->ButtonVal());
}
void MainWindow::cb_EditSym(int onoff, int value) {
  if (onoff) {
    editSymmetries |= value;
  } else {
    editSymmetries &= ~value;
  }

  pieceEdit->editSymmetries(editSymmetries);
}

static void cb_EditChoice_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_EditChoice(); }
void MainWindow::cb_EditChoice() {
  switch(editChoice->getSelected()) {
    case 0:
      pieceEdit->editChoice(gridEditor_c::TSK_SET);
      break;
    case 1:
      pieceEdit->editChoice(gridEditor_c::TSK_VAR);
      break;
    case 2:
      pieceEdit->editChoice(gridEditor_c::TSK_RESET);
      break;
    case 3:
      pieceEdit->editChoice(gridEditor_c::TSK_COLOR);
      break;
  }
}

static void cb_EditMode_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_EditMode(); }
void MainWindow::cb_EditMode() {
  switch(editMode->getSelected()) {
    case 0:
      pieceEdit->editType(gridEditor_c::EDT_RUBBER);
      config.useRubberband(true);
      break;
    case 1:
      pieceEdit->editType(gridEditor_c::EDT_SINGLE);
      config.useRubberband(false);
      break;
  }
}

static void cb_PcSel_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_PcSel((LBlockListGroup*)o); }
void MainWindow::cb_PcSel(LBlockListGroup* grp) {
  int reason = grp->getReason();

  switch(reason) {
  case PieceSelector::RS_CHANGEDSELECTION:
    activateShape(PcSel->getSelection());
    updateInterface();
    StatPieceInfo(PcSel->getSelection());
    break;
  }
}

static void cb_SolProbSel_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_SolProbSel((LBlockListGroup*)o); }
void MainWindow::cb_SolProbSel(LBlockListGroup* grp) {
  int reason = grp->getReason();

  switch(reason) {
  case ProblemSelector::RS_CHANGEDSELECTION:

    unsigned int prob = solutionProblem->getSelection();

    if (prob < puzzle_->problemNumber()) {

      /* check the number of solutions on this tab and lower the slider value if necessary */
      if (SolutionSel->value() > puzzle_->getProblem(prob)->solutionNumber())
        SolutionSel->value(puzzle_->getProblem(prob)->solutionNumber());

      updateInterface();
      activateSolution(prob, (int)SolutionSel->value()-1);
    }
    break;
  }
}

static void cb_ColSel_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_ColSel((LBlockListGroup*)o); }
void MainWindow::cb_ColSel(LBlockListGroup* grp) {
  int reason = grp->getReason();

  switch(reason) {
  case PieceSelector::RS_CHANGEDSELECTION:
    pieceEdit->setColor(colorSelector->getSelection());
    updateInterface();
    activateShape(PcSel->getSelection());
    break;
  }
}

static void cb_ProbSel_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_ProbSel((LBlockListGroup*)o); }
void MainWindow::cb_ProbSel(LBlockListGroup* grp) {
  int reason = grp->getReason();

  switch(reason) {
  case PieceSelector::RS_CHANGEDSELECTION:
    updateInterface();
    activateProblem(problemSelector->getSelection());
    StatProblemInfo(problemSelector->getSelection());
    break;
  }
}

static void cb_pieceEdit_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_pieceEdit((VoxelEditGroup*)o); }
void MainWindow::cb_pieceEdit(VoxelEditGroup* o) {

  switch (o->getReason()) {
  case gridEditor_c::RS_MOUSEMOVE:
    if (o->getMouse())
      View3D->getView()->setMarker(o->getMouseX1(), o->getMouseY1(), o->getMouseX2(), o->getMouseY2(), o->getMouseZ(), editSymmetries);
    else
      View3D->getView()->hideMarker();
    break;
  case gridEditor_c::RS_CHANGESQUARE:
    View3D->getView()->showSingleShape(*puzzle_, PcSel->getSelection());
    StatPieceInfo(PcSel->getSelection());
    changeShape(PcSel->getSelection());
    changed = true;
    break;
  }

  View3D->redraw();
}

static void cb_NewProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_NewProblem(); }
void MainWindow::cb_NewProblem() {

  unsigned int prob = puzzle_->addProblem();

  for (unsigned int c = 0; c < puzzle_->colorNumber(); c++)
    puzzle_->getProblem(prob)->allowPlacement(c+1, c+1);

  problemSelector->setSelection(prob);

  changed = true;
  updateInterface();
  activateProblem(problemSelector->getSelection());
  StatProblemInfo(problemSelector->getSelection());
}

static void cb_DeleteProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteProblem(); }
void MainWindow::cb_DeleteProblem() {

  if (problemSelector->getSelection() < puzzle_->problemNumber()) {

    puzzle_->removeProblem(problemSelector->getSelection());

    changed = true;

    while ((problemSelector->getSelection() >= puzzle_->problemNumber()) &&
           (problemSelector->getSelection() > 0))
      problemSelector->setSelection(problemSelector->getSelection()-1);

    updateInterface();
    if (problemSelector->getSelection() < puzzle_->problemNumber())
      activateProblem(problemSelector->getSelection());
    else
      activateClear();
    StatProblemInfo(problemSelector->getSelection());
  }
}

static void cb_CopyProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_CopyProblem(); }
void MainWindow::cb_CopyProblem() {

  if (problemSelector->getSelection() < puzzle_->problemNumber()) {

    unsigned int prob = puzzle_->addProblem(puzzle_->getProblem(problemSelector->getSelection()));
    problemSelector->setSelection(prob);

    changed = true;
    updateInterface();
    activateProblem(problemSelector->getSelection());
    StatProblemInfo(problemSelector->getSelection());
  }
}

static void cb_RenameProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_RenameProblem(); }
void MainWindow::cb_RenameProblem() {

  if (problemSelector->getSelection() < puzzle_->problemNumber()) {

    const char * name = fl_input("Enter name for the problem",
        puzzle_->getProblem(problemSelector->getSelection())->getName().c_str());

    if (name) {

      puzzle_->getProblem(problemSelector->getSelection())->setName(name);
      changed = true;
      updateInterface();
      activateProblem(problemSelector->getSelection());
    }
  }
}

static void cb_ProblemLeft_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ProblemExchange(-1); }
static void cb_ProblemRight_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ProblemExchange(+1); }
void MainWindow::cb_ProblemExchange(int with) {

  unsigned int current = problemSelector->getSelection();
  unsigned int other = current + with;

  if ((current < puzzle_->problemNumber()) && (other < puzzle_->problemNumber())) {
    puzzle_->exchangeProblem(current, other);
    changed = true;
    problemSelector->setSelection(other);
  }
}

static void cb_ShapeLeft_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ShapeExchange(-1); }
static void cb_ShapeRight_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ShapeExchange(+1); }
void MainWindow::cb_ShapeExchange(int with) {

  unsigned int current = PcSel->getSelection();
  unsigned int other = current + with;

  if ((current < puzzle_->shapeNumber()) && (other < puzzle_->shapeNumber())) {
    puzzle_->exchangeShape(current, other);
    changed = true;
    PcSel->setSelection(other);
  }
}

static void cb_ProbShapeLeft_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ProbShapeExchange(-1); }
static void cb_ProbShapeRight_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ProbShapeExchange(+1); }
void MainWindow::cb_ProbShapeExchange(int with) {

  unsigned int p = problemSelector->getSelection();
  unsigned int s = shapeAssignmentSelector->getSelection();

  Problem * pr = puzzle_->getProblem(p);

  // find out the index in the problem table
  unsigned int current;

  for (current = 0; current < pr->partNumber(); current++)
    if (pr->getShape(current) == s)
      break;

  unsigned int other = current + with;

  if ((current < pr->partNumber()) && (other < pr->partNumber())) {
    pr->exchangeShape(current, other);
    changed = true;
    updateInterface();
    activateProblem(problemSelector->getSelection());
  }
}

static void cb_ColorAssSel_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ColorAssSel(); }
void MainWindow::cb_ColorAssSel() {
  updateInterface();
}

static void cb_ColorConstrSel_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ColorConstrSel(); }
void MainWindow::cb_ColorConstrSel() {
  updateInterface();
}

static void cb_ShapeToResult_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ShapeToResult(); }
void MainWindow::cb_ShapeToResult() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  if (shapeAssignmentSelector->getSelection() >= puzzle_->shapeNumber())
    return;

  unsigned int prob = problemSelector->getSelection();
  Problem * pr = puzzle_->getProblem(prob);

  // check if this shape is already a piece of the problem
  pr->setShapeMaximum(shapeAssignmentSelector->getSelection(), 0);

  pr->setResultId(shapeAssignmentSelector->getSelection());
  problemResult->setPuzzle(puzzle_->getProblem(prob));
  activateProblem(prob);
  StatProblemInfo(prob);
  updateInterface();

  changed = true;
}

static void cb_ShapeSel_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_SelectProblemShape(); }
void MainWindow::cb_SelectProblemShape() {
  updateInterface();
  activateProblem(problemSelector->getSelection());
}

static void cb_PiecesClicked_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_PiecesClicked(); }
void MainWindow::cb_PiecesClicked() {

  Problem * pr = puzzle_->getProblem(problemSelector->getSelection());

  shapeAssignmentSelector->setSelection(pr->getShape(PiecesCountList->getClicked()));

  updateInterface();
  activateProblem(problemSelector->getSelection());
}

static void cb_AddShapeToProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AddShapeToProblem(); }
void MainWindow::cb_AddShapeToProblem() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }
  unsigned int shape = shapeAssignmentSelector->getSelection();
  if (shape >= puzzle_->shapeNumber())
    return;

  unsigned int prob = problemSelector->getSelection();

  changed = true;
  PiecesCountList->redraw();

  Problem * pr = puzzle_->getProblem(prob);

  // first see, if there is already a selected shape inside
  pr->setShapeMaximum(shape, pr->getShapeMaximum(shape) + 1);
  pr->setShapeMinimum(shape, pr->getShapeMinimum(shape) + 1);

  activateProblem(problemSelector->getSelection());
  updateInterface();
  StatProblemInfo(problemSelector->getSelection());
}

static void cb_AddAllShapesToProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AddAllShapesToProblem(); }
void MainWindow::cb_AddAllShapesToProblem() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int prob = problemSelector->getSelection();

  changed = true;
  PiecesCountList->redraw();

  Problem * pr = puzzle_->getProblem(prob);

  for (unsigned int j = 0; j < puzzle_->shapeNumber(); j++) {

    // we don't add the result shape
    if (pr->resultValid() && j == pr->getResultId())
      continue;

    pr->setShapeMaximum(j, pr->getShapeMaximum(j) + 1);
    pr->setShapeMinimum(j, pr->getShapeMinimum(j) + 1);
  }

  activateProblem(problemSelector->getSelection());
  PcVis->setPuzzle(puzzle_->getProblem(solutionProblem->getSelection()));
  updateInterface();
  StatProblemInfo(problemSelector->getSelection());
}

static void cb_RemoveShapeFromProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_RemoveShapeFromProblem(); }
void MainWindow::cb_RemoveShapeFromProblem() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int shape = shapeAssignmentSelector->getSelection();

  if (shape >= puzzle_->shapeNumber())
    return;

  unsigned int prob = problemSelector->getSelection();

  Problem * pr = puzzle_->getProblem(prob);

  if (pr->getShapeMinimum(shape) > 0) pr->setShapeMinimum(shape, pr->getShapeMinimum(shape)-1);
  if (pr->getShapeMaximum(shape) > 0) pr->setShapeMaximum(shape, pr->getShapeMaximum(shape)-1);

  changed = true;
  PiecesCountList->redraw();
  PcVis->setPuzzle(puzzle_->getProblem(solutionProblem->getSelection()));

  activateProblem(problemSelector->getSelection());
  StatProblemInfo(problemSelector->getSelection());
}


static void cb_SetShapeMinimumToZero_stub (Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_SetShapeMinimumToZero(); }
void MainWindow::cb_SetShapeMinimumToZero() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int shape = shapeAssignmentSelector->getSelection();

  if (shape >= puzzle_->shapeNumber())
    return;

  unsigned int prob = problemSelector->getSelection();
  changeProblem(prob);

  Problem * pr = puzzle_->getProblem(prob);

  pr->setShapeMinimum(shape, 0);

  changed = true;
  PiecesCountList->redraw();
  PcVis->setPuzzle(puzzle_->getProblem(solutionProblem->getSelection()));

  activateProblem(problemSelector->getSelection());
  StatProblemInfo(problemSelector->getSelection());
}


static void cb_RemoveAllShapesFromProblem_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_RemoveAllShapesFromProblem(); }
void MainWindow::cb_RemoveAllShapesFromProblem() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int prob = problemSelector->getSelection();
  changeProblem(prob);

  Problem * pr = puzzle_->getProblem(prob);

  for (unsigned int i = 0; i < puzzle_->shapeNumber(); i++)
    pr->setShapeMaximum(i, 0);

  changed = true;
  PiecesCountList->redraw();
  PcVis->setPuzzle(puzzle_->getProblem(solutionProblem->getSelection()));

  activateProblem(problemSelector->getSelection());
  StatProblemInfo(problemSelector->getSelection());
}

static void cb_ShapeGroup_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ShapeGroup(); }
void MainWindow::cb_ShapeGroup() {

  unsigned int prob = problemSelector->getSelection();

  auto groupEditWin = std::make_unique<GroupsEditor>(puzzle_.get(), prob);

  groupEditWin->show();

  while (groupEditWin->visible())
    Fl::wait();

  if (groupEditWin->changed()) {

    Problem * pr = puzzle_->getProblem(prob);

    /* if the user added the result shape to the problem, we inform him and
     * remove that shape again
     */
    if (pr->resultValid() && pr->getShapeMaximum(pr->getResultId()) > 0)
      pr->setShapeMaximum(pr->getResultId(), 0);

    /* as the user may have reset the counts of one shape to zero, go
     * through the list and remove entries of zero count */
    PiecesCountList->redraw();
    PcVis->setPuzzle(puzzle_->getProblem(solutionProblem->getSelection()));
    changed = true;
    activateProblem(problemSelector->getSelection());
    StatProblemInfo(problemSelector->getSelection());
    updateInterface();
  }
}

static void cb_BtnPlacementBrowser_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_BtnPlacementBrowser(); }
void MainWindow::cb_BtnPlacementBrowser() {

  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  if (!puzzle_->getProblem(prob)->getAssembler()->getPiecePlacementSupported()) {
    fl_message("Sorry no placement browser for this type of puzzle");
    return;
  }

  placementBrowser_c * plbr = new placementBrowser_c(puzzle_->getProblem(prob));

  plbr->show();

  while (plbr->visible())
    Fl::wait();

  delete plbr;
}

static void cb_BtnMovementBrowser_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_BtnMovementBrowser(); }
void MainWindow::cb_BtnMovementBrowser() {

  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  unsigned int sol = (int)SolutionSel->value()-1;

  if (sol >= puzzle_->getProblem(prob)->solutionNumber())
    return;

  movementBrowser_c * mvbr = new movementBrowser_c(puzzle_->getProblem(prob), sol);

  mvbr->show();

  while (mvbr->visible())
    Fl::wait();

  delete mvbr;
}

static void cb_BtnAssemblerStep_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_BtnAssemblerStep(); }
void MainWindow::cb_BtnAssemblerStep() {

  bt_assert(assmThread == 0);

  AssemblerInterface * assm = (AssemblerInterface *)puzzle_->getProblem(solutionProblem->getSelection())->getAssembler();

  bt_assert(assm);

  assm->debug_step(1);

  if (assm->getFinished() >= 1)
    puzzle_->getProblem(solutionProblem->getSelection())->finishedSolving();

  updateInterface();

  View3D->getView()->showAssemblerState(puzzle_->getProblem(solutionProblem->getSelection()), assm->getAssembly());
}

static void cb_AllowColor_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AllowColor(); }
void MainWindow::cb_AllowColor() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int prob = problemSelector->getSelection();
  Problem * pr = puzzle_->getProblem(prob);

  if (colconstrList->GetSortByResult())
    pr->allowPlacement(colorAssignmentSelector->getSelection()+1,
                       colconstrList->getSelection()+1);
  else
    pr->allowPlacement(colconstrList->getSelection()+1,
                       colorAssignmentSelector->getSelection()+1);
  changed = true;
  changeProblem(problemSelector->getSelection());
  updateInterface();
}

static void cb_DisallowColor_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DisallowColor(); }
void MainWindow::cb_DisallowColor() {

  if (problemSelector->getSelection() >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  unsigned int prob = problemSelector->getSelection();
  Problem * pr = puzzle_->getProblem(prob);

  if (colconstrList->GetSortByResult())
    pr->disallowPlacement(colorAssignmentSelector->getSelection()+1,
                              colconstrList->getSelection()+1);
  else
    pr->disallowPlacement(colconstrList->getSelection()+1,
                          colorAssignmentSelector->getSelection()+1);

  changed = true;
  changeProblem(problemSelector->getSelection());
  updateInterface();
}

static void cb_CCSortByResult_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_CCSort(1); }
static void cb_CCSortByPiece_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_CCSort(0); }
void MainWindow::cb_CCSort(bool byResult) {
  colconstrList->SetSortByResult(byResult);

  if (byResult) {
    BtnColSrtPc->activate();
    BtnColSrtRes->deactivate();
  } else {
    BtnColSrtPc->deactivate();
    BtnColSrtRes->activate();
  }
}

static void cb_BtnPrepare_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_BtnPrepare(); }
void MainWindow::cb_BtnPrepare() {
  cb_BtnStart(true);
}

static void cb_BtnStart_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_BtnStart(false); }
void MainWindow::cb_BtnStart(bool prep_only) {

  puzzle_->getProblem(solutionProblem->getSelection())->removeAllSolutions();
  SolutionEmpty = true;

  for (unsigned int i = 0; i < puzzle_->shapeNumber(); i++)
    puzzle_->getShape(i)->initHotspot();

  cb_BtnCont(prep_only);

  updateInterface();
  changed = true;
}

static void cb_BtnCont_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_BtnCont(false); }
void MainWindow::cb_BtnCont(bool prep_only) {

  unsigned int prob = solutionProblem->getSelection();

  if (!(gui_grid_type_->getGridType()->getCapabilities() & GridType::CAP_ASSEMBLE)) {
    fl_message("Sorry this space grid doesn't have an assembler (yet)!");
    return;
  }

  if (SolveDisasm->value() && !(gui_grid_type_->getGridType()->getCapabilities() & GridType::CAP_DISASSEMBLE)) {
    fl_message("Sorry this space grid doesn't have a disassembler (yet)!\n"
               "You must disable the disassembler first\n");
    return;
  }

  if (prob >= puzzle_->problemNumber()) {
    fl_message("First create a problem");
    return;
  }

  if (!puzzle_->getProblem(prob)->resultValid()) {
    fl_message("A result shape must be defined");
    return;
  }

  bt_assert(assmThread == 0);

  int par = SolveThread::PAR_REDUCE;
  if (KeepMirrors->value() != 0) par |= SolveThread::PAR_KEEP_MIRROR;
  if (KeepRotations->value() != 0) par |= SolveThread::PAR_KEEP_ROTATIONS;
  if (DropDisassemblies->value() != 0) par |= SolveThread::PAR_DROP_DISASSEMBLIES;
  if (SolveDisasm->value() != 0) par |= SolveThread::PAR_DISASSM;
  if (JustCount->value() != 0) par |= SolveThread::PAR_JUST_COUNT;
  if (CompleteRotations->value() != 0) par |= SolveThread::PAR_COMPLETE_ROTATIONS;

  assmThread = new SolveThread(puzzle_->getProblem(prob), par);

  assmThread->setSortMethod(sortMethod->value());
  assmThread->setSolutionLimits((int)solLimit->value(), (int)solDrop->value());

  if (!assmThread->start(prep_only)) {
    fl_message("Could not start the solving process, the thread creation failed, sorry.");
    delete assmThread;
    assmThread = 0;

  } else {

    updateInterface();
    changed = true;
  }
}

static void cb_BtnStop_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_BtnStop(); }
void MainWindow::cb_BtnStop() {

  bt_assert(assmThread);

  assmThread->stop();
}

static void cb_SolutionSel_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_SolutionSel((Fl_Value_Slider*)o); }
void MainWindow::cb_SolutionSel(Fl_Value_Slider* o) {
  o->take_focus();
  activateSolution(solutionProblem->getSelection(), int(o->value()-1));
  updateInterface();
}

static void cb_SolutionAnim_stub(Fl_Widget* o, void* v) { ((MainWindow*)v)->cb_SolutionAnim((Fl_Value_Slider*)o); }
void MainWindow::cb_SolutionAnim(Fl_Value_Slider* o) {
  o->take_focus();
  if (disassemble) {
    disassemble->setStep(o->value(), config.useBlendedRemoving(), true);
    View3D->getView()->updatePositions(disassemble);
  }
}

static void cb_SrtFind_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_SortSolutions(0); }
static void cb_SrtLevel_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_SortSolutions(1); }
static void cb_SrtMoves_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_SortSolutions(2); }
static void cb_SrtPieces_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_SortSolutions(3); }
void MainWindow::cb_SortSolutions(unsigned int by) {
  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  Problem * pr = puzzle_->getProblem(prob);

  unsigned int sol = pr->solutionNumber();

  if (sol < 2)
    return;

  pr->sortSolutions(by);
  activateSolution(prob, (int)SolutionSel->value()-1);
  updateInterface();
}

static void cb_DelAll_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteSolutions(0); }
static void cb_DelBefore_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteSolutions(1); }
static void cb_DelAt_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteSolutions(2); }
static void cb_DelAfter_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteSolutions(3); }
static void cb_DelDisasmless_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteSolutions(4); }
void MainWindow::cb_DeleteSolutions(unsigned int which) {

  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  unsigned int sol = (int)SolutionSel->value()-1;

  Problem * pr = puzzle_->getProblem(prob);

  if (sol >= pr->solutionNumber())
    return;

  unsigned int cnt;

  changed = true;

  switch (which) {
  case 0:
    cnt = pr->solutionNumber();
    for (unsigned int i = 0; i < cnt; i++)
      pr->removeSolution(0);
    break;
  case 1:
    for (unsigned int i = 0; i < sol; i++)
      pr->removeSolution(0);
    SolutionSel->value(1);
    break;
  case 2:
    pr->removeSolution(sol);
    break;
  case 3:
    cnt = pr->solutionNumber() - sol - 1;
    for (unsigned int i = 0; i < cnt; i++)
      pr->removeSolution(sol+1);
    break;
  case 4:
    cnt = pr->solutionNumber();
    {
      unsigned int i = 0;
      while (i < cnt) {
        if (pr->getSolution(i)->getDisassembly() || pr->getSolution(i)->getDisassemblyInfo())
          i++;
        else {
          pr->removeSolution(i);
          cnt--;
          if (SolutionSel->value() > i)
            SolutionSel->value(SolutionSel->value()-1);
        }
      }
    }
    break;
  }

  if (SolutionSel->value() > pr->solutionNumber())
    SolutionSel->value(pr->solutionNumber());

  activateSolution(prob, (int)SolutionSel->value()-1);
  updateInterface();
}

static void cb_DelDisasm_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteDisasm(); }
void MainWindow::cb_DeleteDisasm() {
  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  Problem * pr = puzzle_->getProblem(prob);

  unsigned int sol = (int)SolutionSel->value()-1;

  if (sol >= pr->solutionNumber())
    return;

  pr->getSolution(sol)->removeDisassembly();

  changed = true;

  activateSolution(prob, (int)SolutionSel->value()-1);
  updateInterface();
}

static void cb_DelAllDisasm_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_DeleteAllDisasm(); }
void MainWindow::cb_DeleteAllDisasm() {
  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  Problem * pr = puzzle_->getProblem(prob);

  for (unsigned int i = 0; i < pr->solutionNumber(); i++)
    pr->getSolution(i)->removeDisassembly();

  changed = true;

  activateSolution(prob, (int)SolutionSel->value()-1);
  updateInterface();
}

static void cb_AddDisasm_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AddDisasm(); }
void MainWindow::cb_AddDisasm() {
  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  Problem * pr = puzzle_->getProblem(prob);

  unsigned int sol = (int)SolutionSel->value()-1;

  if (sol >= pr->solutionNumber())
    return;

  if (!(gui_grid_type_->getGridType()->getCapabilities() & GridType::CAP_DISASSEMBLE)) {
    fl_message("Sorry this space grid doesn't have a disassembler (yet)!");
    return;
  }

  DisassemblerInterface * dis = new SimpleDisassembler(pr);

  Separation * d = dis->disassemble(pr->getSolution(sol)->getAssembly());

  changed = true;

  if (d)
    pr->getSolution(sol)->setDisassembly(d);

  activateSolution(prob, (int)SolutionSel->value()-1);
  updateInterface();

  delete dis;
}

static void cb_AddAllDisasm_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AddAllDisasm(true); }
static void cb_AddMissingDisasm_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AddAllDisasm(false); }
void MainWindow::cb_AddAllDisasm(bool all) {
  unsigned int prob = solutionProblem->getSelection();

  if (prob >= puzzle_->problemNumber())
    return;

  if (!(gui_grid_type_->getGridType()->getCapabilities() & GridType::CAP_DISASSEMBLE)) {
    fl_message("Sorry this space grid doesn't have a disassembler (yet)!");
    return;
  }

  Problem * pr = puzzle_->getProblem(prob);

  changed = true;

  DisassemblerInterface * dis = new SimpleDisassembler(pr);

  Fl_Double_Window * w = new Fl_Double_Window(20, 20, 300, 30);
  Fl_Box * b = new Fl_Box(0, 0, 300, 30);
  w->end();
  w->label("Disassembling...");
  w->set_modal();
  char txt[100];
  w->show();

  for (unsigned int sol = 0; sol < pr->solutionNumber(); sol++) {

    snprintf(txt, 100, "solved %i of %i disassemblies\n", sol, pr->solutionNumber());
    b->label(txt);

    Fl::wait(0);

    if (all || !pr->getSolution(sol)->getDisassembly()) {

      Separation * d = dis->disassemble(pr->getSolution(sol)->getAssembly());

      if (d)
        pr->getSolution(sol)->setDisassembly(d);
    }
  }

  delete dis;
  delete w;

  activateSolution(prob, (int)SolutionSel->value()-1);
  updateInterface();
}


static void cb_PcVis_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_PcVis(); }
void MainWindow::cb_PcVis() {
  View3D->getView()->updateVisibility(PcVis);
}

static void cb_Status_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Status(); }
void MainWindow::cb_Status() {
  View3D->getView()->showColors(*puzzle_, StatusLine->getColorMode());
}

static void cb_3dClick_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_3dClick(); }
void MainWindow::cb_3dClick() {


  if (TaskSelectionTab->value() == TabPieces) {

    if (Fl::event_ctrl()) {
      unsigned int shape, face;
      unsigned long voxel;

      Voxel * sh = puzzle_->getShape(PcSel->getSelection());

      if (View3D->getView()->pickShape(Fl::event_x(),
            View3D->getView()->h()-Fl::event_y(),
            &shape, &voxel, &face))
        sh->setState(voxel, Voxel::VX_EMPTY);

      View3D->getView()->showSingleShape(*puzzle_, PcSel->getSelection());
      StatPieceInfo(PcSel->getSelection());
      changeShape(PcSel->getSelection());
      redraw();
      changed = true;

    } else if (Fl::event_shift() || Fl::event_alt()) {

      unsigned int shape, face;
      unsigned long voxel;

      Voxel * sh = puzzle_->getShape(PcSel->getSelection());

      if (View3D->getView()->pickShape(Fl::event_x(),
            View3D->getView()->h()-Fl::event_y(),
            &shape, &voxel, &face)) {

        unsigned int x, y, z;
        if (sh->indexToXYZ(voxel, &x, &y, &z)) {

          int nx, ny, nz;

          if (sh->getNeighbor(face, 0, x, y, z, &nx, &ny, &nz)) {

            sh->resizeInclude(nx, ny, nz);

            if (Fl::event_alt())
              sh->setState(nx, ny, nz, Voxel::VX_VARIABLE);
            else
              sh->setState(nx, ny, nz, Voxel::VX_FILLED);

            sh->setColor(nx, ny, nz, colorSelector->getSelection());

            View3D->getView()->showSingleShape(*puzzle_, PcSel->getSelection());
            StatPieceInfo(PcSel->getSelection());
            changeShape(PcSel->getSelection());
            activateShape(PcSel->getSelection());
            redraw();
            changed = true;
          }
        }
      }
    }
  } else if (TaskSelectionTab->value() == TabProblems) {

    unsigned int shape;

    if (View3D->getView()->pickShape(Fl::event_x(),
        View3D->getView()->h()-Fl::event_y(),
        &shape, 0, 0)) {

      if (shape >= 2)
        shapeAssignmentSelector->setSelection(puzzle_->getProblem(problemSelector->getSelection())->getShape(shape-2));
    }
  } else if (TaskSelectionTab->value() == TabSolve) {
    if (Fl::event_shift()) {
      unsigned int shape;

      if (View3D->getView()->pickShape(Fl::event_x(),
            View3D->getView()->h()-Fl::event_y(),
            &shape, 0, 0)) {

        PcVis->hidePiece(shape);
        View3D->getView()->updateVisibility(PcVis);
        redraw();
      }
    }
  }
}

static void cb_New_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_New(); }
void MainWindow::cb_New() {

  if (threadStopped()) {

    if (changed)
      if (fl_choice("Puzzle changed are you sure?", "Cancel", "New Puzzle", 0) == 0)
        return;

    GridTypeSelectorWindow w;
    w.show();

    while (w.visible())
      Fl::wait();

    ReplacePuzzle(new Puzzle(w.getGridType()));

    if (fname) {
      delete [] fname;
      fname = 0;
      label("BurrTools - unknown");
    }

    changed = false;

    StatusLine->setText("");
    updateInterface();
    activateShape(0);
  }
}

static void cb_Load_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Load(); }
void MainWindow::cb_Load() {

  if (threadStopped()) {

    if (changed)
      if (fl_choice("Puzzle changed are you sure?", "Cancel", "Load", 0) == 0)
        return;

    const char * f = flu_file_chooser("Load Puzzle", "*.xmpuzzle", "");

    tryToLoad(f);
  }
}

static void cb_Load_Ps3d_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Load_Ps3d(); }
void MainWindow::cb_Load_Ps3d() {

  if (threadStopped()) {

    if (changed)
      if (fl_choice("Puzzle changed are you sure?", "Cancel", "Load", 0) == 0)
        return;

    const char * f = flu_file_chooser("Import PuzzleSolver3D File", "*.puz", "");

    if (f) {

      std::ifstream in(f);

      Puzzle * newPuzzle = loadPuzzlerSolver3D(&in);
      if (!newPuzzle) {
        fl_alert("Could not load puzzle, sorry!");
        return;
      }

      if (fname) delete [] fname;
      fname = new char[strlen(f)+1];
      strcpy(fname, f);

      char nm[300];
      snprintf(nm, 299, "BurrTools - %s", fname);
      label(nm);

      ReplacePuzzle(newPuzzle);
      updateInterface();

      TaskSelectionTab->value(TabPieces);
      activateShape(PcSel->getSelection());
      StatPieceInfo(PcSel->getSelection());

      changed = false;
    }
  }
}

static void cb_Save_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Save(); }
void MainWindow::cb_Save() {

  if (threadStopped()) {

    if (!fname)
      cb_SaveAs();

    else {
      ogzstream ostr(fname);

      if (ostr) {
        XmlWriter xml(ostr);
        puzzle_->save(xml);
      }

      if (!ostr)
        fl_alert("puzzle NOT saved!!");
      else
        changed = false;
    }
  }
}

static void cb_Convert_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Convert(); }
void MainWindow::cb_Convert() {

  convertWindow_c win(puzzle_->getGridType()->getType());

  win.show();

  while (win.visible())
    Fl::wait();

  if (win.okSelected())
  {
    Puzzle * p = doConvert(*puzzle_, win.getTargetType());

    if (p)
    {
      ReplacePuzzle(p);
      updateInterface();
      activateShape(0);
      changed = true;
    }
  }
}

class voxelTableVector_c : public voxelTable_c
{
  private:

    const std::vector<Voxel *> *shapes;

  public:

    voxelTableVector_c(const std::vector<Voxel *> *s) : shapes(s) {}

  protected:

    const Voxel * findSpace(unsigned int index) const { return (*shapes)[index]; }
};

static void cb_AssembliesToShapes_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_AssembliesToShapes(); }
void MainWindow::cb_AssembliesToShapes() {

  assmImportWindow_c win(puzzle_);

  win.show();

  while (win.visible())
    Fl::wait();

  if (win.okSelected())
  {
    Problem * pr = puzzle_->getProblem(win.getSrcProblem());

    std::vector<Voxel *> sh;

    unsigned int filter = win.getFilter();

    voxelTableVector_c voxelTab(&sh);

    for (unsigned int s = 0; s < pr->solutionNumber(); s++)
    {
      Voxel * shape = pr->getSolution(s)->getAssembly()->createSpace(pr);

      if ((filter & assmImportWindow_c::dropDisconnected) && !shape->connected(0, true, Voxel::VX_EMPTY))
      {
        delete shape;
        continue;
      }

      symmetries_t sym = shape->selfSymmetries();

      if ((filter & assmImportWindow_c::dropMirror) && shape->getGridType()->getSymmetries()->symmetryContainsMirror(sym))
      {
        delete shape;
        continue;
      }

      if ((filter & assmImportWindow_c::dropSymmetric) && !unSymmetric(sym))
      {
        delete shape;
        continue;
      }

      if ((filter & assmImportWindow_c::dropNonMillable) && !isMillable(shape))
      {
        delete shape;
        continue;
      }

      if ((filter & assmImportWindow_c::dropNonNotchable) && !isNotchable(shape))
      {
        delete shape;
        continue;
      }

      unsigned int voxels = shape->countState(Voxel::VX_FILLED);
      if (voxels < win.getShapeMin() || voxels > win.getShapeMax())
      {
        delete shape;
        continue;
      }

      // if the user wants no identical shapes, we look up the current
      // shape in the known shapes table and drop it if we find it
      if (filter & assmImportWindow_c::dropIdentical)
      {
        if (voxelTab.getSpace(shape))
        {
          delete shape;
          continue;
        }
      }

      sh.push_back(shape);

      // we only need to add the current shape to the shape table
      // if the user wants to drop identical shapes and we use the table
      if (filter & assmImportWindow_c::dropIdentical)
      {
        voxelTab.addSpace(sh.size()-1);
      }
    }

    if (win.getAction() == assmImportWindow_c::A_ADD_NEW)
      pr = puzzle_->getProblem(puzzle_->addProblem());
    else if (win.getAction() == assmImportWindow_c::A_ADD_DST)
      pr = puzzle_->getProblem(win.getDstProblem());

    // add the shapes to the problem of the problem tab
    for (unsigned int s = 0; s < sh.size(); s++)
    {
      int i = puzzle_->addShape(sh[s]);

      if (win.getAction() == assmImportWindow_c::A_ADD_DST || win.getAction() == assmImportWindow_c::A_ADD_NEW)
      {
        pr->setShapeMaximum(i, win.getMax());
        pr->setShapeMinimum(i, win.getMin());
      }
    }

    changed = true;
    PiecesCountList->redraw();

    updateInterface();
  }
}

static void cb_SaveAs_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_SaveAs(); }
void MainWindow::cb_SaveAs() {

  if (threadStopped()) {
    const char * f = flu_file_chooser("Save Puzzle As", "*.xmpuzzle", "");

    if (f) {

      if (!fileExists(f) || fl_choice("File exists overwrite?", "Cancel", "Overwrite", 0)) {

        char f2[1000];

        // check, if the last characters are ".xmpuzzle"
        if (strcmp(f + strlen(f) - strlen(".xmpuzzle"), ".xmpuzzle")) {
          snprintf(f2, 1000, "%s.xmpuzzle", f);

        } else

          snprintf(f2, 1000, "%s", f);

        ogzstream ostr(f2);

        if (ostr)
        {
          XmlWriter xml(ostr);
          puzzle_->save(xml);
        }

        if (!ostr)
          fl_alert("puzzle NOT saved!!!");
        else
          changed = false;

        if (fname) delete [] fname;
        fname = new char[strlen(f2)+1];
        strcpy(fname, f2);

        char nm[300];
        snprintf(nm, 299, "BurrTools - %s", fname);
        label(nm);

      } else {

        fl_message("File not saved!\n");
      }
    }
  }
}

static void cb_Quit_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->hide(); }
void MainWindow::hide() {
  if ((!changed) || fl_choice("Puzzle changed do you want to quit and loose the changes?", "Cancel", "Quit", 0))
    Fl_Double_Window::hide();
}

static void cb_Config_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Config(); }
void MainWindow::cb_Config() {
  config.dialog();
  activateConfigOptions();
}

static void cb_Comment_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Coment(); }
void MainWindow::cb_Coment() {

  multiLineWindow_c win("Edit Comment", "Change the comment for the current puzzle", puzzle_->getComment().c_str());

  win.show();

  while (win.visible())
    Fl::wait();

  if (win.saveChanges()) {
    puzzle_->setComment(win.getText());
    changed = true;
  }
}

static void cb_ImageExportVector_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ImageExportVector(); }
void MainWindow::cb_ImageExportVector() {

  vectorExportWindow_c w;

  w.show();
  while (w.visible())
    Fl::wait();

  if (!w.cancelled)
    View3D->getView()->exportToVector(w.getFileName(), w.getVectorType());
}

static void cb_ImageExport_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_ImageExport(); }
void MainWindow::cb_ImageExport() {
  imageExport_c w(puzzle_);
  w.show();

  while (w.visible()) {
    w.update();
    if (w.isWorking())
      Fl::wait(0);
    else
      Fl::wait(1);
  }
}

static void cb_STLExport_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_STLExport(); }
void MainWindow::cb_STLExport() {
  StlExport w(puzzle_);
  w.show();

  while (w.visible()) {
    Fl::wait();
  }
}

static void cb_StatusWindow_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_StatusWindow(); }
void MainWindow::cb_StatusWindow() {

  bool again;

  do {

    statusWindow_c w(puzzle_);
    w.show();

    while (w.visible()) {
      Fl::wait();
    }

    again = w.getAgain();

    if (again)
      changed = true;

  } while (again);

  unsigned int current = PcSel->getSelection();

  if (puzzle_->shapeNumber() == 0)
    current = (unsigned int)-1;
  else
    while (current >= puzzle_->shapeNumber())
      current--;

  activateShape(current);

  PcSel->setSelection(current);

  updateInterface();
}

static void cb_Toggle3D_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Toggle3D(); }
void MainWindow::cb_Toggle3D() {

  if (TaskSelectionTab->value() == TabPieces) {
    shapeEditorWithBig3DView = !shapeEditorWithBig3DView;
    if (!shapeEditorWithBig3DView)
      Small3DView();
    else
      Big3DView();
  }
}

static void cb_Help_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_Help(); }
void MainWindow::cb_Help() {

  Fl_Help_Dialog * help = new Fl_Help_Dialog;

  help->load("Prologue.html");

  help->show();
}

static void cb_About_stub(Fl_Widget* /*o*/, void* v) { ((MainWindow*)v)->cb_About(); }
void MainWindow::cb_About() {

  fl_message("This is the GUI for BurrTools version " VERSION "\n"
             "BurrTools (c) 2003-2011 by Andreas Röver\n"
             "The latest version is available at burrtools.sourceforge.net\n"
             "\n"
             "This software is distributed under the GPL\n"
             "You should have received a copy of the GNU General Public License\n"
             "along with this program (COPYING); if not, write to the Free Software\n"
             "Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA\n"
             "or see www.fsf.org\n"
             "\n"
             "The program uses\n"
             "- Fltk, FLU, libZ, libpng, gzstream, gl2ps\n"
             "- Fl_Table (http://3dsite.com/people/erco/Fl_Table/)\n"
             "- tr by Brian Paul (http://www.mesa3d.org/brianp/TR.html)\n"
            );
}

void MainWindow::StatPieceInfo(unsigned int pc) {

  if (pc < puzzle_->shapeNumber()) {
    char txt[100];

    unsigned int fx = puzzle_->getShape(pc)->countState(Voxel::VX_FILLED);
    unsigned int vr = puzzle_->getShape(pc)->countState(Voxel::VX_VARIABLE);

    snprintf(txt, 100, "Shape S%i has %i voxels (%i fixed, %i variable)", pc+1, fx+vr, fx, vr);
    StatusLine->setText(txt);
  }
}

void MainWindow::StatProblemInfo(unsigned int prob) {

  if ((prob < puzzle_->problemNumber()) && (puzzle_->getProblem(prob)->resultValid())) {

    Problem * pr = puzzle_->getProblem(prob);

    char txt[100];

    unsigned int cnt = 0;
    unsigned int cntMin = 0;

    for (unsigned int i = 0; i < pr->partNumber(); i++) {
      cnt += pr->getShapeShape(i)->countState(Voxel::VX_FILLED) * pr->getShapeMax(i);
      cntMin += pr->getShapeShape(i)->countState(Voxel::VX_FILLED) * pr->getShapeMin(i);
    }

    if (cnt == cntMin) {

      snprintf(txt, 100, "Problem P%i result can contain %i - %i voxels, pieces (n = %i) contain %i voxels", prob+1,
          pr->getResultShape()->countState(Voxel::VX_FILLED),
               pr->getResultShape()->countState(Voxel::VX_FILLED) +
               pr->getResultShape()->countState(Voxel::VX_VARIABLE),
          pr->pieceNumber(), cnt);

    } else {

      snprintf(txt, 100, "Problem P%i result can contain %i - %i voxels, pieces (n = %i) contain %i-%i voxels", prob+1,
          pr->getResultShape()->countState(Voxel::VX_FILLED),
               pr->getResultShape()->countState(Voxel::VX_FILLED) +
               pr->getResultShape()->countState(Voxel::VX_VARIABLE),
          pr->pieceNumber(), cntMin, cnt);
    }

    StatusLine->setText(txt);

  } else

    StatusLine->setText("");
}

void MainWindow::changeColor(unsigned int nr) {

  for (unsigned int i = 0; i < puzzle_->shapeNumber(); i++)
    for (unsigned int j = 0; j < puzzle_->getShape(i)->getXYZ(); j++)
      if (puzzle_->getShape(i)->getColor(j) == nr) {
        changeShape(i);
        break;
      }
}

void MainWindow::changeShape(unsigned int nr) {
  for (unsigned int i = 0; i < puzzle_->problemNumber(); i++)
    if (puzzle_->getProblem(i)->usesShape(nr))
      puzzle_->getProblem(i)->removeAllSolutions();
}

void MainWindow::changeProblem(unsigned int nr) {
  puzzle_->getProblem(nr)->removeAllSolutions();
}

bool MainWindow::threadStopped() {

  if (assmThread) {

    fl_message("Stop solving process first!");
    return false;
  }

  return true;
}

bool MainWindow::tryToLoad(const char * f) {

  // it may well be that the file doesn't exist, if it came from the command line
  if (!f) return false;
  if (!fileExists(f)) return false;

  std::istream * str = openGzFile(f);
  XmlParser pars(*str);

  Puzzle * newPuzzle;

  try {
    newPuzzle = new Puzzle(pars);
  }

  catch (xmlParserException_c e)
  {
    fl_message("%s", (std::string("load error: ") + e.what()).c_str());
    delete str;
    return false;
  }

  delete str;

  if (fname) delete [] fname;
  fname = new char[strlen(f)+1];
  strcpy(fname, f);

  char nm[300];
  snprintf(nm, 299, "BurrTools - %s", fname);
  label(nm);

  ReplacePuzzle(newPuzzle);
  updateInterface();

  TaskSelectionTab->value(TabPieces);
  activateShape(PcSel->getSelection());
  StatPieceInfo(PcSel->getSelection());
  View3D->getView()->showColors(puzzle_, StatusLine->getColorMode());

  changed = false;

  // check for a started assemblies, and warn user about it
  bool containsStarted = false;

  for (unsigned int p = 0; p < puzzle_->problemNumber(); p++) {
    if (puzzle_->getProblem(p)->getSolveState() == SS_SOLVING) {
      containsStarted = true;
      break;
    }
  }

  if (containsStarted)
    fl_message("This puzzle file contains started but not finished search for solutions.");

  if (puzzle_->getCommentPopup())
    fl_message("%s", puzzle_->getComment().c_str());

  return true;
}

void MainWindow::ReplacePuzzle(Puzzle * NewPuzzle) {

  // inform everybody
  colorSelector->setPuzzle(NewPuzzle);
  PcSel->setPuzzle(NewPuzzle);
  pieceEdit->setPuzzle(NewPuzzle, 0);
  problemSelector->setPuzzle(NewPuzzle);
  colorAssignmentSelector->setPuzzle(NewPuzzle);
  colconstrList->setPuzzle(NewPuzzle, 0);
  if (NewPuzzle->problemNumber() > 0) {
    problemResult->setPuzzle(NewPuzzle->getProblem(0));
    PiecesCountList->setPuzzle(NewPuzzle->getProblem(0));
    PcVis->setPuzzle(NewPuzzle->getProblem(0));
  } else {
    problemResult->setPuzzle(0);
    PiecesCountList->setPuzzle(0);
    PcVis->setPuzzle(0);
  }
  shapeAssignmentSelector->setPuzzle(NewPuzzle);
  solutionProblem->setPuzzle(NewPuzzle);

  SolutionSel->value(1);
  SolutionAnim->value(0);

  if (NewPuzzle != puzzle_) {
    delete puzzle_;
    puzzle_ = NewPuzzle;
  }

  GuiGridType * nggt = new GuiGridType(puzzle_->getGridType());

  // now replace all gridtype dependent gui elements with
  // instances from the guigridtype
  pieceEdit->newGridType(nggt, puzzle_);
  pieceTools->newGridType(nggt);

  // for the pieceEditor we need to reset all the edit mode fields
  pieceEdit->editSymmetries(editSymmetries);
  switch(editChoice->getSelected()) {
    case 0: pieceEdit->editChoice(gridEditor_c::TSK_SET); break;
    case 1: pieceEdit->editChoice(gridEditor_c::TSK_VAR); break;
    case 2: pieceEdit->editChoice(gridEditor_c::TSK_RESET); break;
    case 3: pieceEdit->editChoice(gridEditor_c::TSK_COLOR); break;
  }
  switch(editMode->getSelected()) {
    case 0: pieceEdit->editType(gridEditor_c::EDT_RUBBER); break;
    case 1: pieceEdit->editType(gridEditor_c::EDT_SINGLE); break;
  }

  delete gui_grid_type_;
  gui_grid_type_ = nggt;
}

Fl_Menu_Item MainWindow::menu_MainMenu[] = {
  { "&File",           0, 0, 0, FL_SUBMENU },
    {"New",            0, cb_New_stub,         0, 0, 0, 0, 14, 56},
    {"Load",    FL_F + 3, cb_Load_stub,        0, 0, 0, 0, 14, 56},
    {"Import",         0, cb_Load_Ps3d_stub,   0, 0, 0, 0, 14, 56},
    {"Save",    FL_F + 2, cb_Save_stub,        0, 0, 0, 0, 14, 56},
    {"Save As",        0, cb_SaveAs_stub,      0, FL_MENU_DIVIDER, 0, 0, 14, 56},
    {"Convert",        0, cb_Convert_stub,     0, 0, 0, 0, 14, 56},
    {"Import Assms",   0, cb_AssembliesToShapes_stub,     0, 0, 0, 0, 14, 56},
    {"Quit",           0, cb_Quit_stub,        0, 0, 3, 0, 14, 56},
    { 0 },
  {"Toggle 3D", FL_F + 4, cb_Toggle3D_stub,    0, 0, 0, 0, 14, 56},
  { "&Export",         0, 0, 0, FL_SUBMENU },
    {"Images",             0, cb_ImageExport_stub, 0, 0, 0, 0, 14, 56},
    {"Vector Image",       0, cb_ImageExportVector_stub, 0, 0, 0, 0, 14, 56},
    {"STL",             0, cb_STLExport_stub, 0, 0, 0, 0, 14, 56},
    { 0 },
  {"Status",           0, cb_StatusWindow_stub,  0, 0, 0, 0, 14, 56},
  {"Edit Comment",     0, cb_Comment_stub,     0, 0, 0, 0, 14, 56},
  {"Config",           0, cb_Config_stub,      0, 0, 0, 0, 14, 56},
  {"Help",      FL_F + 1, cb_Help_stub,        0, 0, 0, 0, 14, 56},
  {"About",            0, cb_About_stub,       0, 0, 3, 0, 14, 56},
  {0}
};

void MainWindow::show(int argn, char ** argv) {
  LFl_Double_Window::show();

  int arg = 1;

  // try all command line switches, until either they are
  // all tried or one got loaded successfully
  while (arg < argn)
    if (tryToLoad(argv[arg]))
      break;
    else
      arg++;
}

void MainWindow::activateClear() {
  View3D->getView()->showNothing();
  pieceEdit->clearPuzzle();
  pieceTools->setVoxelSpace(0, 0);

  SolutionEmpty = true;
}

void MainWindow::activateShape(unsigned int number) {

  if ((number < puzzle_->shapeNumber())) {

    View3D->getView()->showSingleShape(puzzle_, number);
    pieceEdit->setPuzzle(puzzle_, number);
    pieceTools->setVoxelSpace(puzzle_, number);

    PcSel->setSelection(number);

  } else {

    View3D->getView()->showNothing();
    pieceEdit->clearPuzzle();
    pieceTools->setVoxelSpace(0, 0);
  }

  SolutionEmpty = true;
}

void MainWindow::activateProblem(unsigned int prob) {

  if (prob < puzzle_->problemNumber())
    View3D->getView()->showProblem(puzzle_, prob, shapeAssignmentSelector->getSelection());
  else
    View3D->getView()->showNothing();

  SolutionEmpty = true;
}

void MainWindow::activateSolution(unsigned int prob, unsigned int num) {

  if (disassemble) {
    delete disassemble;
    disassemble = 0;
  }

  if ((prob < puzzle_->problemNumber()) && (num < puzzle_->getProblem(prob)->solutionNumber())) {

    Problem * pr = puzzle_->getProblem(prob);

    PcVis->setPuzzle(puzzle_->getProblem(prob));
    PcVis->setAssembly(pr->getSolution(num)->getAssembly());
    AssemblyNumber->show();
    AssemblyNumber->value(pr->getSolution(num)->getAssemblyNumber()+1);

    if (pr->getSolution(num)->getDisassembly()) {
      SolutionAnim->show();
      SolutionAnim->range(0, pr->getSolution(num)->getDisassembly()->sumMoves());

      SolutionsInfo->show();

      MovesInfo->show();

      char levelText[50];
      int len = snprintf(levelText, 50, "%i (", pr->getSolution(num)->getDisassembly()->sumMoves());
      pr->getSolution(num)->getDisassembly()->movesText(levelText + len, 50-len);
      levelText[strlen(levelText)+1] = 0;
      levelText[strlen(levelText)] = ')';

      MovesInfo->value(levelText);

      disassemble = new DisassemblyToMoves(pr->getSolution(num)->getDisassembly(),
                                      2*pr->getResultShape()->getBiggestDimension(),
                                           pr->pieceNumber());
      disassemble->setStep(SolutionAnim->value(), config.useBlendedRemoving(), true);

      if (prob < puzzle_->problemNumber()) View3D->getView()->showAssembly(puzzle_->getProblem(prob), num);
      View3D->getView()->updatePositions(disassemble);
      View3D->getView()->updateVisibility(PcVis);

      SolutionNumber->show();
      SolutionNumber->value(pr->getSolution(num)->getSolutionNumber()+1);

    } else if (pr->getSolution(num)->getDisassemblyInfo()) {

      SolutionAnim->range(0, 0);
      SolutionAnim->hide();

      SolutionsInfo->show();

      MovesInfo->show();

      char levelText[50];
      int len = snprintf(levelText, 50, "%i (", pr->getSolution(num)->getDisassemblyInfo()->sumMoves());
      pr->getSolution(num)->getDisassemblyInfo()->movesText(levelText + len, 50-len);
      levelText[strlen(levelText)+1] = 0;
      levelText[strlen(levelText)] = ')';

      MovesInfo->value(levelText);

      if (prob < puzzle_->problemNumber()) View3D->getView()->showAssembly(puzzle_->getProblem(prob), num);
      View3D->getView()->updateVisibility(PcVis);

      SolutionNumber->show();
      SolutionNumber->value(pr->getSolution(num)->getSolutionNumber()+1);

    } else {

      SolutionAnim->range(0, 0);
      SolutionAnim->hide();
      MovesInfo->value(0);
      MovesInfo->hide();

      if (prob < puzzle_->problemNumber()) View3D->getView()->showAssembly(puzzle_->getProblem(prob), num);
      View3D->getView()->updateVisibility(PcVis);

      SolutionNumber->hide();
    }

    SolutionEmpty = false;

  } else {

    View3D->getView()->showNothing();
    SolutionEmpty = true;

    SolutionAnim->hide();
    MovesInfo->hide();

    PcVis->setPuzzle(0);

    AssemblyNumber->hide();
    SolutionNumber->hide();
  }
}

const char * timeToString(float time) {

  static char tmp[50];

  if (time < 60)                               snprintf(tmp, 50, "%i seconds",     int(time/(1                 )));
  else if (time < 60*60)                       snprintf(tmp, 50, "%.1f minutes",   time/(60                    ));
  else if (time < 60*60*24)                    snprintf(tmp, 50, "%.1f hours",     time/(60*60                 ));
  else if (time < 60*60*24*30)                 snprintf(tmp, 50, "%.1f days",      time/(60*60*24              ));
  else if (time < 60*60*24*365.2422)           snprintf(tmp, 50, "%.1f months",    time/(60*60*24*30           ));
  else if (time < 60*60*24*365.2422*1000)      snprintf(tmp, 50, "%.1f years",     time/(60*60*24*365.2422     ));
  else if (time < 60*60*24*365.2422*1000*1000) snprintf(tmp, 50, "%.1f millennia", time/(60*60*24*365.2422*1000));
  else                                         snprintf(tmp, 50, "ages");

  return tmp;
}

int MainWindow::findMenuEntry(const char * txt) {

  int found = -1;

  for (unsigned int i = 0; i < (sizeof(menu_MainMenu) / sizeof(menu_MainMenu[0])); i++)
    if (menu_MainMenu[i].text && (strcmp(menu_MainMenu[i].label(), txt) == 0)) {
      bt_assert(found == -1);
      found = i;
    }

  bt_assert(found >= 0);
  return found;
}

void MainWindow::updateInterface() {

  // update the menu items activate state

  // there must be at least one shape before there is something to export...
  if (puzzle_->shapeNumber() > 0)
    menu_MainMenu[findMenuEntry("Images")].activate();
  else
    menu_MainMenu[findMenuEntry("Images")].deactivate();

  if (gui_grid_type_->getGridType()->getCapabilities() & GridType::CAP_STLEXPORT &&
      puzzle_->shapeNumber() > 0)
    menu_MainMenu[findMenuEntry("STL")].activate();
  else
    menu_MainMenu[findMenuEntry("STL")].deactivate();

  MainMenu->copy(menu_MainMenu, this);

  unsigned int prob = solutionProblem->getSelection();

  if (TaskSelectionTab->value() == TabPieces) {
    // shapes tab

    // we can only delete colours, when something valid is selected
    // and no assembler is running
    if ((colorSelector->getSelection() > 0) && !assmThread)
      BtnDelColor->activate();
    else
      BtnDelColor->deactivate();

    // colours can be changed for all colours except the neutral colour
    if (colorSelector->getSelection() > 0)
      BtnChnColor->activate();
    else
      BtnChnColor->deactivate();

    // we can only edit and copy shapes, when something valid is selected
    if (PcSel->getSelection() < puzzle_->shapeNumber()) {
      BtnCpyShape->activate();
      BtnRenShape->activate();
      pieceEdit->activate();
    } else {
      BtnCpyShape->deactivate();
      BtnRenShape->deactivate();
      pieceEdit->deactivate();
    }

    // shapes can only be moved, when the neighbour shape is there
    if ((PcSel->getSelection() > 0) && (PcSel->getSelection() < puzzle_->shapeNumber()) && !assmThread)
      BtnShapeLeft->activate();
    else
      BtnShapeLeft->deactivate();
    if ((PcSel->getSelection()+1 < puzzle_->shapeNumber()) && !assmThread)
      BtnShapeRight->activate();
    else
      BtnShapeRight->deactivate();

    // we can only delete shapes, when something valid is selected
    // and no assembler is running
    if ((PcSel->getSelection() < puzzle_->shapeNumber()) && !assmThread) {
      BtnDelShape->activate();
    } else {
      BtnDelShape->deactivate();
    }

    const Problem * pr = (assmThread) ? assmThread->getProblem() : 0;

    // we can only edit shapes, when something valid is selected and
    // either no assembler is running or the shape is not in the problem that the assembler works on
    if ((PcSel->getSelection() < puzzle_->shapeNumber()) &&
        (!assmThread || !pr->usesShape(PcSel->getSelection()))) {
      pieceTools->activate();
    } else {
      pieceTools->deactivate();
    }

    // when the current shape is in the assembler we lock the editor, only viewing is possible
    if (assmThread && (pr->usesShape(PcSel->getSelection())))
      pieceEdit->deactivate();
    else
      pieceEdit->activate();

    if (puzzle_->colorNumber() < 63)
      BtnNewColor->activate();
    else
      BtnNewColor->deactivate();

  } else if (TaskSelectionTab->value() == TabProblems) {

    Problem * pr = (problemSelector->getSelection() < puzzle_->problemNumber())
      ? puzzle_->getProblem(problemSelector->getSelection()) : 0;

    // problem tab
    PiecesCountList->setPuzzle(pr);
    colconstrList->setPuzzle(puzzle_, problemSelector->getSelection());
    problemResult->setPuzzle(pr);

    // problems can only be renames and copied, when something valid is selected
    if (problemSelector->getSelection() < puzzle_->problemNumber()) {
      BtnCpyProb->activate();
      BtnRenProb->activate();
    } else {
      BtnCpyProb->deactivate();
      BtnRenProb->deactivate();
    }

    // problems can only be shifted around when the corresponding neighbour is
    // available
    if ((problemSelector->getSelection() > 0) && (problemSelector->getSelection() < puzzle_->problemNumber()) && !assmThread)
      BtnProbLeft->activate();
    else
      BtnProbLeft->deactivate();
    if ((problemSelector->getSelection()+1 < puzzle_->problemNumber()) && !assmThread)
      BtnProbRight->activate();
    else
      BtnProbRight->deactivate();

    if (problemSelector->getSelection() < puzzle_->problemNumber() && !assmThread)
    {
      unsigned int current;
      unsigned int p = problemSelector->getSelection();
      unsigned int s = shapeAssignmentSelector->getSelection();

      Problem * pr = puzzle_->getProblem(p);

      for (current = 0; current < pr->partNumber(); current++)
        if (pr->getShape(current) == s)
          break;

      if (current && (current < pr->partNumber()))
        BtnProbShapeLeft->activate();
      else
        BtnProbShapeLeft->deactivate();
      if (current+1 < pr->partNumber())
        BtnProbShapeRight->activate();
      else
        BtnProbShapeRight->deactivate();

    } else {
      BtnProbShapeRight->deactivate();
      BtnProbShapeLeft->deactivate();
    }

    // problems can only be deleted, something valid is selected and the
    // assembler is not running
    if ((problemSelector->getSelection() < puzzle_->problemNumber()) && !assmThread)
      BtnDelProb->activate();
    else
      BtnDelProb->deactivate();

    // we can only edit colour constraints when a valid problem is selected
    // the selected colour is valid
    // the assembler is not running or not busy with the selected problem
    if ((problemSelector->getSelection() < puzzle_->problemNumber()) &&
        (colorAssignmentSelector->getSelection() < puzzle_->colorNumber()) &&
        (!assmThread || (assmThread->getProblem() != puzzle_->getProblem(problemSelector->getSelection())))) {

      Problem * pr = puzzle_->getProblem(problemSelector->getSelection());

      // check, if the given colour is already added
      if (colconstrList->GetSortByResult()) {
        if (pr->placementAllowed(colorAssignmentSelector->getSelection()+1,
                                 colconstrList->getSelection()+1)) {
          BtnColAdd->deactivate();
          BtnColRem->activate();
        } else {
          BtnColAdd->activate();
          BtnColRem->deactivate();
        }
      } else {
        if (pr->placementAllowed(colconstrList->getSelection()+1,
                                 colorAssignmentSelector->getSelection()+1)) {
          BtnColAdd->deactivate();
          BtnColRem->activate();
        } else {
          BtnColAdd->activate();
          BtnColRem->deactivate();
        }
      }
    } else {
      BtnColAdd->deactivate();
      BtnColRem->deactivate();
    }

    // we can only change shapes, when a valid problem is selected
    // a valid shape is selected
    // the assembler is not running or not busy with out problem
    if ((problemSelector->getSelection() < puzzle_->problemNumber()) &&
        (shapeAssignmentSelector->getSelection() < puzzle_->shapeNumber()) &&
        (!assmThread || (assmThread->getProblem() != puzzle_->getProblem(problemSelector->getSelection())))) {
      BtnSetResult->activate();

      Problem * pr = puzzle_->getProblem(problemSelector->getSelection());

      // we can only add a shape, when it's not the result of the current problem
      if (!pr->resultValid() || pr->getResultId() != shapeAssignmentSelector->getSelection())
        BtnAddShape->activate();
      else
        BtnAddShape->deactivate();

      bool found = false;

      for (unsigned int p = 0; p < pr->partNumber(); p++)
        if (pr->getShape(p) == shapeAssignmentSelector->getSelection()) {
          found = true;
          break;
        }

      if (found) {
        BtnRemShape->activate();
        BtnMinZero->activate();
      } else {
        BtnRemShape->deactivate();
        BtnMinZero->deactivate();
      }

    } else {
      BtnSetResult->deactivate();
      BtnAddShape->deactivate();
      BtnRemShape->deactivate();
      BtnMinZero->deactivate();
    }

    // we can edit the groups, when we have a problem with at least one shape and
    // the assembler is not working on the current problem
    if ((problemSelector->getSelection() < puzzle_->problemNumber()) &&
        (!assmThread || (assmThread->getProblem() != puzzle_->getProblem(problemSelector->getSelection())))) {
      BtnGroup->activate();
    } else {
      BtnGroup->deactivate();
    }

    if ((problemSelector->getSelection() < puzzle_->problemNumber()) &&
        (!assmThread || (assmThread->getProblem() != puzzle_->getProblem(problemSelector->getSelection())))) {
      BtnAddAll->activate();
      BtnRemAll->activate();
    } else {
      BtnAddAll->deactivate();
      BtnRemAll->deactivate();
    }

  } else {

    float finished = ((prob < puzzle_->problemNumber()) &&
        puzzle_->getProblem(prob)->getAssembler())
          ? puzzle_->getProblem(prob)->getAssembler()->getFinished()
          : 0;

    if (prob < puzzle_->problemNumber()) {

      Problem * pr = puzzle_->getProblem(prob);

      // solution tab
      PcVis->setPuzzle(pr);

      // we have a valid problem selected, so update the information visible

      SolvingProgress->value(100*finished);
      SolvingProgress->show();

      {
        static char tmp[100];
        snprintf(tmp, 100, "%.4f%%", 100*finished);
        SolvingProgress->label(tmp);
      }

      unsigned long numSol = pr->solutionNumber();

      if (numSol > 0) {

        SolutionSel->show();
        SolutionsInfo->show();

        SolutionSel->range(1, numSol);
        if (SolutionSel->value() > numSol)
          SolutionSel->value(numSol);
        SolutionsInfo->value(numSol);

        // if we are in the solve tab and have a valid solution
        // we can activate that
        if (SolutionEmpty && (numSol > 0)) {
          activateSolution(prob, 0);
          SolutionSel->value(1);
        }

      } else {

        SolutionSel->range(1, 1);
        SolutionSel->hide();
        SolutionsInfo->hide();
        SolutionAnim->hide();
        MovesInfo->hide();

        AssemblyNumber->hide();
        SolutionNumber->hide();
      }

      if (pr->numAssembliesKnown()) {
        OutputAssemblies->value(pr->getNumAssemblies());
        OutputAssemblies->show();
      } else {
        OutputAssemblies->hide();
      }

      if (pr->numSolutionsKnown()) {
        OutputSolutions->value(pr->getNumSolutions());
        OutputSolutions->show();
      } else {
        OutputSolutions->hide();
      }

      // the placement browser can only be activated when an assembler is available and not assembling is active
      if (pr->getAssembler() && !assmThread) {
        BtnPlacement->activate();
      } else {
        BtnPlacement->deactivate();
      }

      // the step button is only active when the placements browser can be active AND when the puzzle is not
      // yet completely solved
      if (pr->getAssembler() && !assmThread && (pr->getSolveState() != SS_SOLVED)) {
        if (BtnStep) BtnStep->activate();
      } else {
        if (BtnStep) BtnStep->deactivate();
      }

      if (pr->solutionNumber() >= 2) {
        BtnSrtFind->activate();
        BtnSrtLevel->activate();
        BtnSrtMoves->activate();
        BtnSrtPieces->activate();
      } else {
        BtnSrtFind->deactivate();
        BtnSrtLevel->deactivate();
        BtnSrtMoves->deactivate();
        BtnSrtPieces->deactivate();
      }

      if (pr->solutionNumber() > 0) {
        BtnDelAll->activate();
        if (SolutionSel->value() > 1)
          BtnDelBefore->activate();
        else
          BtnDelBefore->deactivate();

        BtnDelAt->activate();

        if ((SolutionSel->value()-1) < (pr->solutionNumber()-1))
          BtnDelAfter->activate();
        else
          BtnDelAfter->deactivate();

        BtnDelDisasm->activate();
      } else {
        BtnDelAll->deactivate();
        BtnDelBefore->deactivate();
        BtnDelAt->deactivate();
        BtnDelAfter->deactivate();
        BtnDelDisasm->deactivate();
      }

      if ((SolutionSel->value() >= 1) &&
          ((int)SolutionSel->value()-1) < (int)pr->solutionNumber() &&
          pr->getSolution((int)SolutionSel->value()-1)->getDisassembly()) {
        BtnDisasmDel->activate();
      } else {
        BtnDisasmDel->deactivate();
      }

      if (pr->solutionNumber() > 0) {
        BtnDisasmDelAll->activate();
      } else {
        BtnDisasmDelAll->deactivate();
      }

      if (gui_grid_type_->getGridType()->getCapabilities() & GridType::CAP_DISASSEMBLE) {

        if (pr->solutionNumber() > 0) {
          BtnDisasmAdd->activate();
          BtnDisasmAddAll->activate();
          BtnDisasmAddMissing->activate();
        } else {
          BtnDisasmAdd->deactivate();
          BtnDisasmAddAll->deactivate();
          BtnDisasmAddMissing->deactivate();
        }
        SolveDisasm->activate();
      } else {
        BtnDisasmAdd->deactivate();
        BtnDisasmAddAll->deactivate();
        BtnDisasmAddMissing->deactivate();
        SolveDisasm->deactivate();
        SolveDisasm->value(0);
      }

      if (gui_grid_type_->getGridType()->getCapabilities() & GridType::CAP_DISASSEMBLE &&
          !assmThread &&
          solutionProblem->getSelection() < puzzle_->problemNumber() &&
          (int)SolutionSel->value()-1 < puzzle_->getProblem(solutionProblem->getSelection())->solutionNumber()
         )
      {
        BtnMovement->activate();
      }
      else
      {
        BtnMovement->deactivate();
      }
    } else {

      // no valid problem available, hide all information

      SolutionSel->hide();
      SolutionsInfo->hide();
      OutputSolutions->hide();
      SolutionAnim->hide();
      MovesInfo->hide();

      AssemblyNumber->hide();
      SolutionNumber->hide();

      SolvingProgress->hide();
      OutputAssemblies->hide();

      BtnPlacement->deactivate();
      BtnMovement->deactivate();
      if (BtnStep) BtnStep->deactivate();
      if (BtnPrepare) BtnPrepare->deactivate();

      BtnSrtFind->deactivate();
      BtnSrtLevel->deactivate();
      BtnSrtMoves->deactivate();
      BtnSrtPieces->deactivate();
      BtnDelAll->deactivate();
      BtnDelBefore->deactivate();
      BtnDelAt->deactivate();
      BtnDelAfter->deactivate();
      BtnDelDisasm->deactivate();
      BtnDisasmDel->deactivate();
      BtnDisasmDelAll->deactivate();
      BtnDisasmAdd->deactivate();
      BtnDisasmAddAll->deactivate();
      BtnDisasmAddMissing->deactivate();

      PcVis->setPuzzle(0);
    }


    if (assmThread && (assmThread->getProblem() == puzzle_->getProblem(prob))) {

      Problem * pr = puzzle_->getProblem(prob);

      // a thread is currently running

      unsigned int ut;
      if (pr->usedTimeKnown())
        ut = pr->getUsedTime() + assmThread->getTime();
      else
        ut = assmThread->getTime();

      TimeUsed->value(timeToString(ut));
      if (finished != 0)
        TimeEst->value(timeToString(ut/finished-ut));
      else
        TimeEst->value("unknown");

      TimeUsed->show();
      TimeEst->show();

    } else {

      if ((prob < puzzle_->problemNumber()) && puzzle_->getProblem(prob)->usedTimeKnown()) {
        Problem * pr = puzzle_->getProblem(prob);
        TimeUsed->value(timeToString(pr->getUsedTime()));
        TimeUsed->show();
      } else {
        TimeUsed->hide();
      }

      TimeEst->hide();
    }

    if (assmThread) {

      Problem * pr = puzzle_->getProblem(prob);

      switch(assmThread->currentAction()) {
      case SolveThread::ACT_PREPARATION:
        {
          char tmp[20];
          snprintf(tmp, 20, "prepare piece %i", assmThread->currentActionParameter()+1);
          OutputActivity->value(tmp);
        }
        break;
      case SolveThread::ACT_REDUCE:
        if (pr->getAssembler()) {
          char tmp[20];
          snprintf(tmp, 20, "optimize piece %i", pr->getAssembler()->getReducePiece()+1);
          OutputActivity->value(tmp);
        } else {
          char tmp[20];
          snprintf(tmp, 20, "optimize piece %i", assmThread->currentActionParameter()+1);
          OutputActivity->value(tmp);
        }
        break;
      case SolveThread::ACT_ASSEMBLING:
        OutputActivity->value("assemble");
        break;
      case SolveThread::ACT_DISASSEMBLING:
        OutputActivity->value("disassemble");
        break;
      case SolveThread::ACT_PAUSING:
        OutputActivity->value("pause");
        break;
      case SolveThread::ACT_FINISHED:
        OutputActivity->value("finished");
        break;
      case SolveThread::ACT_WAIT_TO_STOP:
        OutputActivity->value("please wait");
        break;
      case SolveThread::ACT_ERROR:
        OutputActivity->value("error");
        break;
      }

      if (assmThread->getProblem() == puzzle_->getProblem(prob)) {

        // for the actually solved problem we enable the stop button
        BtnStart->deactivate();
        if (BtnPrepare) BtnPrepare->deactivate();
        BtnCont->deactivate();
        BtnStop->activate();

        // we can not edit solutions for a currently solved problem
        BtnSrtFind->deactivate();
        BtnSrtLevel->deactivate();
        BtnSrtMoves->deactivate();
        BtnSrtPieces->deactivate();
        BtnDelAll->deactivate();
        BtnDelBefore->deactivate();
        BtnDelAt->deactivate();
        BtnDelAfter->deactivate();
        BtnDelDisasm->deactivate();
        BtnDisasmDel->deactivate();
        BtnDisasmDelAll->deactivate();
        BtnDisasmAdd->deactivate();
        BtnDisasmAddAll->deactivate();
        BtnDisasmAddMissing->deactivate();

      } else {

        // all other problems can do nothing
        BtnStart->deactivate();
        if (BtnPrepare) BtnPrepare->deactivate();
        BtnCont->deactivate();
        BtnStop->deactivate();
        if (BtnPrepare) BtnPrepare->deactivate();

      }

    } else {

      pieceEdit->activate();

      // no thread currently calculating

      // so we can not stop the thread
      BtnStop->deactivate();

      // the stop button might be pressed when the thread finished, this might happen
      // relatively often, so we clear the state of that button
      BtnStop->clear();

      if (prob < puzzle_->problemNumber()) {

        Problem * pr = puzzle_->getProblem(prob);
        // a valid problem is selected

        switch(pr->getSolveState()) {
        case SS_UNSOLVED:
          OutputActivity->value("nothing");
          BtnCont->deactivate();
          break;
        case SS_SOLVED:
          OutputActivity->value("finished");
          BtnCont->deactivate();
          break;
        case SS_SOLVING:
          OutputActivity->value("pause");
          BtnCont->activate();
          break;
        case SS_UNKNOWN:
          OutputActivity->value("partial");
          BtnCont->deactivate();
          break;

        }

        // if we have a result and at least one piece, we can give it a try
        if ((pr->pieceNumber() > 0) && (pr->resultValid())) {
          BtnStart->activate();
          if (BtnPrepare) BtnPrepare->activate();
        } else {
          BtnStart->deactivate();
          if (BtnPrepare) BtnPrepare->deactivate();
        }

      } else {

        // no start possible, when no valid problem selected
        BtnStart->deactivate();
        if (BtnPrepare) BtnPrepare->deactivate();
        BtnCont->deactivate();
        if (BtnPrepare) BtnPrepare->deactivate();
      }
    }
  }

  TaskSelectionTab->redraw();
}

void MainWindow::update() {

  if (assmThread) {

    // check, if the thread has thrown an exception, if so re-throw it
    if (assmThread->currentAction() == SolveThread::ACT_ASSERT) {

      assertWindow_c * aw = new assertWindow_c("Because of an internal error the current puzzle\n"
                                               "can not be solved\n",
                                               &(assmThread->getAssertException()));

      aw->show();

      while (aw->visible())
        Fl::wait();

      delete aw;
      delete assmThread;
      assmThread = 0;
      updateInterface();
      return;
    }

    // check, if the thread has stopped, if so then delete the object
    if ((assmThread->currentAction() == SolveThread::ACT_PAUSING) ||
        (assmThread->currentAction() == SolveThread::ACT_FINISHED)) {

      delete assmThread;
      assmThread = 0;

    } else if (assmThread->currentAction() == SolveThread::ACT_ERROR) {

      unsigned int selectShape = 0xFFFFFFFF;

      switch(assmThread->getErrorState()) {
      case AssemblerInterface::ERR_TOO_MANY_UNITS:
        fl_message("Pieces contain %i units too many", assmThread->getErrorParam());
        break;
      case AssemblerInterface::ERR_TOO_FEW_UNITS:
        fl_message("Pieces contain %i units less than required\n"
                   "See user guide sections\n"
                   "1.3.2.1 'Voxel States'\n"
                   "3.4.2 'Basic Drawing Tools' and\n"
                   "3.8 'Miscellaneous Editing Tools'", assmThread->getErrorParam());
        break;
      case AssemblerInterface::ERR_CAN_NOT_PLACE:
        fl_message("Piece %i can be placed nowhere within the result", assmThread->getErrorParam()+1);
        selectShape = assmThread->getErrorParam();
        break;
      case AssemblerInterface::ERR_CAN_NOT_RESTORE_VERSION:
        fl_message("Impossible to restore the saved state because the internal format changed.\n"
                   "You either have to start from the beginning or finish with the old version of BurrTools, sorry");
        break;
      case AssemblerInterface::ERR_CAN_NOT_RESTORE_SYNTAX:
        fl_message("Impossible to restore the saved state because something with the data is wrong.\n"
                   "You have to start from the beginning, sorry");
        break;
      case AssemblerInterface::ERR_PUZZLE_UNHANDABLE:
        fl_message("Something went wrong the program can not solve your puzzle definitions.\n"
                   "You should send the puzzle file to the programmer!");
        break;
      default:
        break;
      }

      if (selectShape < puzzle_->shapeNumber()) {
        TaskSelectionTab->value(TabPieces);
        PcSel->setSelection(selectShape);
        activateShape(PcSel->getSelection());
        updateInterface();
        StatPieceInfo(PcSel->getSelection());
      }

      delete assmThread;
      assmThread = 0;
    }

    // update the window, either when the thread stopped and so the buttons need to
    // be updated, or then the thread works for the currently selected problem
    if (!assmThread || assmThread->getProblem() == puzzle_->getProblem(solutionProblem->getSelection()))
      updateInterface();
  }
}

void MainWindow::Toggle3DView(void)
{
  // select the pieces tab, as exchanging widgets while they are invisible
  // didn't work. Save the current tab before that
  // this is required when changing the tab and we need to get the 3D view back
  // in the large window
  TaskSelectionTab->when(0);
  Fl_Widget *v = TaskSelectionTab->value();
  if (v != TabPieces) TaskSelectionTab->value(TabPieces);

  // exchange widget positions
  Fl_Group * tmp = pieceEdit->parent();
  View3D->parent()->add(pieceEdit);
  tmp->add(View3D);

  // exchange sizes
  {
    int x = pieceEdit->x();
    int y = pieceEdit->y();
    int w = pieceEdit->w();
    int h = pieceEdit->h();
    pieceEdit->resize(View3D->x(), View3D->y(), View3D->w(), View3D->h());
    View3D->resize(x, y, w, h);
  }

  // exchange grid positions
  {
    unsigned int x1, y1, w1, h1, x2, y2, w2, h2;
    pieceEdit->getGridValues(&x1, &y1, &w1, &h1);
    View3D->getGridValues   (&x2, &y2, &w2, &h2);

    pieceEdit->setGridValues( x2,  y2,  w2,  h2);
    View3D->setGridValues   ( x1,  y1,  w1,  h1);
  }

  is3DViewBig = !is3DViewBig;

  if (is3DViewBig)
    pieceEdit->parent()->resizable(pieceEdit);
  else
    View3D->parent()->resizable(View3D);

  // restore the old selected tab
  if (v != TabPieces) TaskSelectionTab->value(v);
  TaskSelectionTab->when(FL_WHEN_CHANGED);
}

void MainWindow::Big3DView() {
  if (!is3DViewBig) Toggle3DView();
  View3D->show();
  redraw();
}

void MainWindow::Small3DView() {
  if (is3DViewBig) Toggle3DView();
  pieceEdit->show();
  redraw();
}

int MainWindow::handle(int event) {

  if (Fl_Double_Window::handle(event))
    return 1;

  switch(event) {
  case FL_SHORTCUT:
    if (Fl::event_length()) {
      switch (Fl::event_text()[0]) {
      case '+':
        if (TaskSelectionTab->value() == TabPieces) {
          pieceEdit->setZ(pieceEdit->getZ()+1);
          return 1;
        }
        break;
      case '-':
        if (TaskSelectionTab->value() == TabPieces) {
          if (pieceEdit->getZ() > 0)
            pieceEdit->setZ(pieceEdit->getZ()-1);
          return 1;
        }
        break;
      }
    }
    switch(Fl::event_key()) {
      case FL_F + 5:
        if (TaskSelectionTab->value() == TabPieces) {
          editChoice->select(0);
          return 1;
        }
        break;
      case FL_F + 6:
        if (TaskSelectionTab->value() == TabPieces) {
          editChoice->select(1);
          return 1;
        }
        break;
      case FL_F + 7:
        if (TaskSelectionTab->value() == TabPieces) {
          editChoice->select(2);
          return 1;
        }
        break;
      case FL_F + 8:
        if (TaskSelectionTab->value() == TabPieces) {
          editChoice->select(3);
          return 1;
        }
        break;
    }
  }

  return 0;
}

#define SZ_GAP 5                               // gap between elements

void MainWindow::CreateShapeTab() {

  TabPieces = new Layouter();
  TabPieces->label("Entities");
  TabPieces->tooltip("Edit shapes");
  TabPieces->clear_visible_focus();

  LFl_Tile * tile = new LFl_Tile(0, 0, 1, 1);
  tile->pitch(SZ_GAP);

  {
    Layouter * group = new Layouter(0, 0);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Shapes", false);

    Layouter * o = new Layouter(0, 1);

    BtnNewShape =   new LFlatButton_c(0, 0, 1, 1, "New", " Add another piece ", cb_NewShape_stub, this);
    ((LFlatButton_c*)BtnNewShape)->weight(1, 0);
    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDelShape =   new LFlatButton_c(2, 0, 1, 1, "Delete", " Delete selected piece ", cb_DeleteShape_stub, this);
    ((LFlatButton_c*)BtnDelShape)->weight(1, 0);
    (new LFl_Box(3, 0))->setMinimumSize(SZ_GAP, 0);
    BtnCpyShape =   new LFlatButton_c(4, 0, 1, 1, "Copy", " Copy selected piece ", cb_CopyShape_stub, this);
    ((LFlatButton_c*)BtnCpyShape)->weight(1, 0);
    (new LFl_Box(5, 0))->setMinimumSize(SZ_GAP, 0);
    BtnRenShape =   new LFlatButton_c(6, 0, 1, 1, "Label", " Give the selected shape a name ", cb_NameShape_stub, this);
    ((LFlatButton_c*)BtnRenShape)->weight(1, 0);
    (new LFl_Box(7, 0))->setMinimumSize(SZ_GAP, 0);
    BtnWeightInc =  new LFlatButton_c(8, 0, 1, 1, "W+", " Increase Weight of the selected shape ",cb_WeightInc_stub, this);
    (new LFl_Box(9, 0))->setMinimumSize(SZ_GAP, 0);
    BtnWeightDec =  new LFlatButton_c(10, 0, 1, 1, "W-", " Decrease Weight of the selected shape ",cb_WeightDec_stub, this);
    (new LFl_Box(11, 0))->setMinimumSize(SZ_GAP, 0);
    BtnShapeLeft =  new LFlatButton_c(12, 0, 1, 1, "@-14->", " Exchange current shape with previous shape ", cb_ShapeLeft_stub, this);
    (new LFl_Box(13, 0))->setMinimumSize(SZ_GAP, 0);
    BtnShapeRight = new LFlatButton_c(14, 0, 1, 1, "@-16->", " Exchange current shape with next shape ", cb_ShapeRight_stub, this);

    o->end();

    (new LFl_Box(0, 2))->setMinimumSize(0, SZ_GAP);

    PcSel = new PieceSelector(0, 0, 200, 200, puzzle_);
    LBlockListGroup * selGroup = new LBlockListGroup(0, 3, 1, 1, PcSel);
    selGroup->callback(cb_PcSel_stub, this);
    selGroup->tooltip(" Select the shape that you want to edit ");
    selGroup->weight(1, 1);

    group->end();
  }

  {
    Layouter * group = new Layouter(0, 1);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Edit", true);

    pieceTools = new ToolTabContainer(0, 1, 1, 1, gui_grid_type_);
    pieceTools->callback(cb_TransformPiece_stub, this);

    (new LFl_Box(0, 2, 1, 1))->setMinimumSize(0, 5);

    Layouter * o2 = new Layouter(0, 3, 1, 1);

    editChoice = new ButtonGroup(0, 0, 1, 1);

    Fl_Button * b;
    b = editChoice->addButton();
    b->image(pm.get(TB_Color_Pen_Fixed_xpm));
    b->tooltip(" Add normal voxels to the shape F5 ");

    b = editChoice->addButton();
    b->image(pm.get(TB_Color_Pen_Variable_xpm));
    b->tooltip(" Add variable voxels to the shape F6 ");

    b = editChoice->addButton();
    b->image(pm.get(TB_Color_Eraser_xpm));
    b->tooltip(" Remove voxels from the shape F7 ");

    b = editChoice->addButton();
    b->image(pm.get(TB_Color_Brush_xpm));
    b->tooltip(" Change the constrain colour of voxels in the shape F8 ");

    editChoice->callback(cb_EditChoice_stub, this);

    (new LFl_Box(1, 0, 1, 1))->setMinimumSize(5, 0);

    editMode = new ButtonGroup(2, 0, 1, 1);

    b = editMode->addButton();
    b->image(pm.get(TB_Color_Mouse_Rubber_Band_xpm));
    b->tooltip(" Make changes by dragging rectangular areas in the grid editor ");

    b = editMode->addButton();
    b->image(pm.get(TB_Color_Mouse_Drag_xpm));
    b->tooltip(" Make changes by painting in the grid editor ");

    editMode->callback(cb_EditMode_stub, this);

    (new LFl_Box(3, 0, 1, 1))->setMinimumSize(5, 0);

    LToggleButton_c * btn;

    btn = new LToggleButton_c(4, 0, 1, 1, cb_EditSym_stub, this, gridEditor_c::TOOL_MIRROR_X);
    btn->image(pm.get(TB_Color_Symmetrical_X_xpm));
    btn->tooltip(" Toggle mirroring along the y-z-plane ");

    btn = new LToggleButton_c(5, 0, 1, 1, cb_EditSym_stub, this, gridEditor_c::TOOL_MIRROR_Y);
    btn->image(pm.get(TB_Color_Symmetrical_Y_xpm));
    btn->tooltip(" Toggle mirroring along the x-z-plane ");

    btn = new LToggleButton_c(6, 0, 1, 1, cb_EditSym_stub, this, gridEditor_c::TOOL_MIRROR_Z);
    btn->image(pm.get(TB_Color_Symmetrical_Z_xpm));
    btn->tooltip(" Toggle mirroring along the x-y-plane ");

    (new LFl_Box(7, 0, 1, 1))->setMinimumSize(5, 0);

    btn = new LToggleButton_c(8, 0, 1, 1, cb_EditSym_stub, this, gridEditor_c::TOOL_STACK_X);
    btn->image(pm.get(TB_Color_Columns_X_xpm));
    btn->tooltip(" Toggle drawing in all x layers ");

    btn = new LToggleButton_c(9, 0, 1, 1, cb_EditSym_stub, this, gridEditor_c::TOOL_STACK_Y);
    btn->image(pm.get(TB_Color_Columns_Y_xpm));
    btn->tooltip(" Toggle drawing in all y layers ");

    btn = new LToggleButton_c(10, 0, 1, 1, cb_EditSym_stub, this, gridEditor_c::TOOL_STACK_Z);
    btn->image(pm.get(TB_Color_Columns_Z_xpm));
    btn->tooltip(" Toggle drawing in all z layers ");

    (new LFl_Box(11, 0, 1, 1))->weight(1, 0);

    o2->end();

    (new LFl_Box(0, 4, 1, 1))->setMinimumSize(0, 5);

    pieceEdit = new VoxelEditGroup(0, 5, 1, 1, puzzle_, gui_grid_type_);
    pieceEdit->callback(cb_pieceEdit_stub, this);
    pieceEdit->end();
    pieceEdit->editType(gridEditor_c::EDT_RUBBER);
    pieceEdit->weight(0, 1);

    group->end();
  }

  {
    Layouter * group = new Layouter(0, 2);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Colours", true);

    Layouter * o = new Layouter(0, 1);

    BtnNewColor = new LFlatButton_c(0, 0, 1, 1, "Add", " Add another colour ", cb_AddColor_stub, this);
    ((LFlatButton_c*)BtnNewColor)->weight(1, 0);
    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDelColor = new LFlatButton_c(2, 0, 1, 1, "Remove", " Remove selected colour ", cb_RemoveColor_stub, this);
    ((LFlatButton_c*)BtnDelColor)->weight(1, 0);
    (new LFl_Box(3, 0))->setMinimumSize(SZ_GAP, 0);
    BtnChnColor = new LFlatButton_c(4, 0, 1, 1, "Edit", " Change selected colour ", cb_ChangeColor_stub, this);
    ((LFlatButton_c*)BtnChnColor)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 2))->setMinimumSize(0, SZ_GAP);

    colorSelector = new ColorSelector(0, 0, 200, 200, puzzle_, true);
    LBlockListGroup * colGroup = new LBlockListGroup(0, 3, 1, 1, colorSelector);
    colGroup->callback(cb_ColSel_stub, this);
    colGroup->tooltip(" Select colour to use for all editing operations ");
    colGroup->weight(1, 1);

    group->end();
  }

  tile->end();

  TabPieces->resizable(tile);
  TabPieces->end();

  Fl_Group::current()->resizable(TabPieces);
}

void MainWindow::CreateProblemTab() {

  TabProblems = new Layouter();
  TabProblems->label("Puzzle");
  TabProblems->tooltip("Edit problems");
  TabProblems->hide();
  TabProblems->clear_visible_focus();

  LFl_Tile * tile = new LFl_Tile(0, 0, 1, 1);
  tile->pitch(SZ_GAP);

  {
    Layouter * group = new Layouter(0, 0);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Problems", false);

    Layouter * o = new Layouter(0, 1);

    BtnNewProb = new LFlatButton_c(0, 0, 1, 1, "New", " Add another problem ", cb_NewProblem_stub, this);
    ((LFlatButton_c*)BtnNewProb)->weight(1, 0);
    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDelProb = new LFlatButton_c(2, 0, 1, 1, "Delete", " Delete selected problem ", cb_DeleteProblem_stub, this);
    ((LFlatButton_c*)BtnDelProb)->weight(1, 0);
    (new LFl_Box(3, 0))->setMinimumSize(SZ_GAP, 0);
    BtnCpyProb = new LFlatButton_c(4, 0, 1, 1, "Copy", " Copy selected problem ", cb_CopyProblem_stub, this);
    ((LFlatButton_c*)BtnCpyProb)->weight(1, 0);
    (new LFl_Box(5, 0))->setMinimumSize(SZ_GAP, 0);
    BtnRenProb = new LFlatButton_c(6, 0, 1, 1, "Label", " Rename selected problem ", cb_RenameProblem_stub, this);
    ((LFlatButton_c*)BtnRenProb)->weight(1, 0);
    (new LFl_Box(7, 0))->setMinimumSize(SZ_GAP, 0);

    BtnProbLeft = new LFlatButton_c(8, 0, 1, 1, "@-14->", " Exchange current problem with previous problem ", cb_ProblemLeft_stub, this);
    (new LFl_Box(9, 0))->setMinimumSize(SZ_GAP, 0);
    BtnProbRight = new LFlatButton_c(10, 0, 1, 1, "@-16->", " Exchange current problem with next problem ", cb_ProblemRight_stub, this);

    o->end();

    (new LFl_Box(0, 2))->setMinimumSize(0, SZ_GAP);

    problemSelector = new ProblemSelector(0, 0, 100, 100, puzzle_);
    LBlockListGroup * probGroup = new LBlockListGroup(0, 3, 1, 1, problemSelector);
    probGroup->callback(cb_ProbSel_stub, this);
    probGroup->tooltip(" Select problem to edit ");
    probGroup->weight(1, 1);

    group->end();
  }

  {
    Layouter * group = new Layouter(0, 1);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Piece Assignment", true);

    Layouter * o = new Layouter(0, 1);

    problemResult = new ResultViewer(0, 0, 1, 1);
    problemResult->tooltip(" The result shape for the current problem ");
    problemResult->weight(1, 0);

    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);

    BtnSetResult = new LFlatButton_c(2, 0, 1, 1, "Set Result", " Set selected shape as result ", cb_ShapeToResult_stub, this);

    o->end();

    (new LFl_Box(0, 2))->setMinimumSize(0, SZ_GAP);

    shapeAssignmentSelector = new PieceSelector(0, 0, 100, 100, puzzle_);
    LBlockListGroup * shapeGroup = new LBlockListGroup(0, 3, 1, 1, shapeAssignmentSelector);
    shapeGroup->callback(cb_ShapeSel_stub, this);
    shapeGroup->tooltip(" Select a shape to set as result or to add or remove from problem ");
    shapeGroup->weight(1, 1);

    group->end();
  }

  {
    Layouter * group = new Layouter(0, 2);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, 0, true);

    Layouter * o = new Layouter(0, 1);

    int xp = 0;

    BtnAddShape = new LFlatButton_c(xp++, 0, 1, 1, "+1", " Add another one of the selected shape ", cb_AddShapeToProblem_stub, this);
    ((LFlatButton_c*)BtnAddShape)->weight(1, 0);
    (new LFl_Box(xp++, 0))->setMinimumSize(SZ_GAP, 0);
    BtnRemShape = new LFlatButton_c(xp++, 0, 1, 1, "-1", " Remove one of the selected shapes ", cb_RemoveShapeFromProblem_stub, this);
    ((LFlatButton_c*)BtnRemShape)->weight(1, 0);
    (new LFl_Box(xp++, 0))->setMinimumSize(SZ_GAP, 0);
    BtnMinZero = new LFlatButton_c(xp++, 0, 1, 1, "min=0", " Set minimum number of pieces to 0 ", cb_SetShapeMinimumToZero_stub, this);
    ((LFlatButton_c*)BtnMinZero)->weight(1, 0);
    (new LFl_Box(xp++, 0))->setMinimumSize(SZ_GAP, 0);
    BtnAddAll = new LFlatButton_c(xp++, 0, 1, 1, "all+1", " Add one of all shapes except result ", cb_AddAllShapesToProblem_stub, this);
    ((LFlatButton_c*)BtnAddAll)->weight(1, 0);
    (new LFl_Box(xp++, 0))->setMinimumSize(SZ_GAP, 0);
    BtnRemAll = new LFlatButton_c(xp++, 0, 1, 1, "Clr", " Remove all pieces ", cb_RemoveAllShapesFromProblem_stub, this);
    ((LFlatButton_c*)BtnRemAll)->weight(1, 0);
    (new LFl_Box(xp++, 0))->setMinimumSize(SZ_GAP, 0);
    BtnGroup =    new LFlatButton_c(xp++, 0, 1, 1, "Detail", " Edit details of the problem ", cb_ShapeGroup_stub, this);
    ((LFlatButton_c*)BtnGroup)->weight(1, 0);
    (new LFl_Box(xp++, 0))->setMinimumSize(SZ_GAP, 0);
    BtnProbShapeLeft = new LFlatButton_c(xp++, 0, 1, 1, "@-14->", " Exchange current shape with previous shape ", cb_ProbShapeLeft_stub, this);
    (new LFl_Box(xp++, 0))->setMinimumSize(SZ_GAP, 0);
    BtnProbShapeRight = new LFlatButton_c(xp++, 0, 1, 1, "@-16->", " Exchange current shape with next shape ", cb_ProbShapeRight_stub, this);

    o->end();

    (new LFl_Box(0, 2))->setMinimumSize(0, SZ_GAP);

    PiecesCountList = new PiecesList(0, 0, 100, 100);
    LBlockListGroup * shapeGroup = new LBlockListGroup(0, 3, 1, 1, PiecesCountList);
    shapeGroup->callback(cb_PiecesClicked_stub, this);
    shapeGroup->tooltip(" Show which shapes are used in the current problem and how often they are used, can be used to select shapes ");
    shapeGroup->weight(1, 1);

    group->end();
  }

  {
    Layouter * group = new Layouter(0, 3);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Colour Assignment", true);

    colorAssignmentSelector = new ColorSelector(0, 0, 100, 100, puzzle_, false);
    LBlockListGroup * colGroup = new LBlockListGroup(0, 1, 1, 1, colorAssignmentSelector);
    colGroup->callback(cb_ColorAssSel_stub, this);
    colGroup->tooltip(" Select colour to add or remove from constraints ");
    colGroup->weight(1, 1);

    group->end();
  }

  {
    Layouter * group = new Layouter(0, 4);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, 0, true);

    Layouter * o = new Layouter(0, 1);

    BtnColSrtPc = new LFlatButton_c(0, 0, 1, 1, "Sort by Piece", " Sort colour constraints by piece ", cb_CCSortByPiece_stub, this);
    ((LFlatButton_c*)BtnColSrtPc)->weight(1, 0);
    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);
    BtnColAdd = new LFlatButton_c(2, 0, 1, 1, "@-12->", " Add colour to constraint ", cb_AllowColor_stub, this);
    BtnColRem = new LFlatButton_c(3, 0, 1, 1, "@-18->", " Add colour to constraint ", cb_DisallowColor_stub, this);
    (new LFl_Box(4, 0))->setMinimumSize(SZ_GAP, 0);
    BtnColSrtRes = new LFlatButton_c(5, 0, 1, 1, "Sort by Result", " Sort Colour Constraints by Result ", cb_CCSortByResult_stub, this);
    ((LFlatButton_c*)BtnColSrtRes)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 2))->setMinimumSize(0, SZ_GAP);

    colconstrList = new ColorConstraintsEdit(0, 0, 100, 100, puzzle_);
    LConstraintsGroup_c * colGroup = new LConstraintsGroup_c(0, 3, 1, 1, colconstrList);
    colGroup->callback(cb_ColorConstrSel_stub, this);
    colGroup->tooltip(" Colour constraints for the current problem ");
    colGroup->weight(1, 1);

    group->end();
  }

  tile->end();

  TabProblems->resizable(tile);
  TabProblems->end();
}

void MainWindow::CreateSolveTab() {

  TabSolve = new Layouter();
  TabSolve->label("Solver");
  TabSolve->tooltip("Solve problems");
  TabSolve->hide();
  TabSolve->clear_visible_focus();

  LFl_Tile * tile = new LFl_Tile(0, 0, 1, 1);
  tile->pitch(SZ_GAP);

  {
    Layouter * group = new Layouter(0, 0);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Parameters", false);

    solutionProblem = new ProblemSelector(0, 0, 100, 100, puzzle_);
    LBlockListGroup * shapeGroup = new LBlockListGroup(0, 1, 1, 1, solutionProblem);
    shapeGroup->callback(cb_SolProbSel_stub, this);
    shapeGroup->tooltip(" Select problem to solve ");
    shapeGroup->weight(1, 1);

    Layouter * o = new Layouter(0, 2);

    SolveDisasm = new LFl_Check_Button("Disassemble", 0, 0, 1, 1);
    SolveDisasm->tooltip(" Do also try to disassemble the assembled puzzles. Only puzzles that can be disassembled will be added to solutions ");
    SolveDisasm->clear_visible_focus();

    JustCount = new LFl_Check_Button("Just Count", 0, 1, 1, 1);
    JustCount->tooltip(" Don\'t save the solutions, just count the number of them ");
    JustCount->clear_visible_focus();

    CompleteRotations = new LFl_Check_Button("Expnsv Rot Check", 0, 2, 1, 1);
    CompleteRotations->tooltip(" Do expensive and thorough rotation check, eliminating translations and rotations not in symmetry of the result shape ");
    CompleteRotations->clear_visible_focus();

    DropDisassemblies = new LFl_Check_Button("Drop Disassemblies", 1, 0, 1, 1);
    DropDisassemblies->tooltip(" Don\'t save the Disassemblies, just the information about them ");
    DropDisassemblies->clear_visible_focus();

    KeepMirrors = new LFl_Check_Button("Keep Mirror Solutions", 1, 1, 1, 1);
    KeepMirrors->tooltip(" Don't remove solutions that are mirrors of another solution ");
    KeepMirrors->clear_visible_focus();

    KeepRotations = new LFl_Check_Button("Keep Rotated Solutions", 1, 2, 1, 1);
    KeepRotations->tooltip(" Don't remove solutions that are rotations of other solutions ");
    KeepRotations->clear_visible_focus();

    o->end();

    o = new Layouter(0, 3);

    new LFl_Box("Sort by: ", 0, 0, 1, 1);

    sortMethod = new LFl_Choice(1, 0, 1, 1);
    ((LFl_Choice*)sortMethod)->weight(1, 0);

    // be careful the order in here must correspond with the enumeration in assembler thread
    sortMethod->add("Unsorted");
    sortMethod->add("Moves for Complete Disassembly");
    sortMethod->add("Level");

    sortMethod->value(1);

    o->end();

    (new LFl_Box(0, 4))->setMinimumSize(0, SZ_GAP);

    o = new Layouter(0, 5);

    new LFl_Box("Drop ", 0, 0, 1, 1);
    new LFl_Box("Limit ", 3, 0, 1, 1);
    (new LFl_Box(2, 0))->setMinimumSize(SZ_GAP, 0);

    solDrop = new LFl_Value_Input(1, 0, 1, 1);
    solLimit = new LFl_Value_Input(4, 0, 1, 1);
    solDrop->bounds(1, 100000000);
    solLimit->bounds(1, 100000000);
    solLimit->step(1, 1);
    solDrop->step(1, 1);

    solDrop->value(1);
    solLimit->value(100);

    ((LFl_Value_Input*)solDrop)->weight(1, 0);
    ((LFl_Value_Input*)solLimit)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 6))->setMinimumSize(0, SZ_GAP);

    o = new Layouter(0, 7);

    if (expertMode) {
      BtnPrepare = new LFlatButton_c(0, 0, 1, 1, "Prepare", " Do the preparation phase and then stop, this removes old results ", cb_BtnPrepare_stub, this);
      ((LFlatButton_c*)BtnPrepare)->weight(1, 0);
      (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);
    } else
      BtnPrepare = 0;
    BtnStart = new LFlatButton_c(2, 0, 1, 1, "Start", " Start new solving process, removing old result ", cb_BtnStart_stub, this);
    ((LFlatButton_c*)BtnStart)->weight(1, 0);
    (new LFl_Box(3, 0))->setMinimumSize(SZ_GAP, 0);
    BtnCont = new LFlatButton_c(4, 0, 1, 1, "Continue", " Continue started process ", cb_BtnCont_stub, this);
    ((LFlatButton_c*)BtnCont)->weight(1, 0);
    (new LFl_Box(5, 0))->setMinimumSize(SZ_GAP, 0);
    BtnStop = new LFlatButton_c(6, 0, 1, 1, "Stop", " Stop a currently running solution process ", cb_BtnStop_stub, this);
    ((LFlatButton_c*)BtnStop)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 8))->setMinimumSize(0, SZ_GAP);

    o = new Layouter(0, 9);

    BtnPlacement = new LFlatButton_c(0, 0, 1, 1, "Placements", " Browse the calculated placement of pieces ", cb_BtnPlacementBrowser_stub, this);
    ((LFlatButton_c*)BtnPlacement)->weight(1, 0);

    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);

    BtnMovement = new LFlatButton_c(2, 0, 1, 1, "Movements", " Browse the possible movements for an assembly ", cb_BtnMovementBrowser_stub, this);
    ((LFlatButton_c*)BtnMovement)->weight(1, 0);

    if (expertMode)
    {
      (new LFl_Box(3, 0))->setMinimumSize(SZ_GAP, 0);

      BtnStep = new LFlatButton_c(4, 0, 1, 1, "Step", " Make one step in the assembler ", cb_BtnAssemblerStep_stub, this);
      ((LFlatButton_c*)BtnStep)->weight(1, 0);
    } else
      BtnStep = 0;

    o->end();

    (new LFl_Box(0, 10))->setMinimumSize(0, SZ_GAP);

    SolvingProgress = new LFl_Progress(0, 11, 1, 1);
    SolvingProgress->tooltip(" Percentage of solution space searched ");
    SolvingProgress->box(FL_ENGRAVED_BOX);
    SolvingProgress->selection_color((Fl_Color)4);
    SolvingProgress->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

    o = new Layouter(0, 12);

    (new LFl_Box("Activity: ", 0, 0, 1, 1))->stretchRight();
    OutputActivity = new LFl_Output(1, 0, 3, 1);
    OutputActivity->box(FL_FLAT_BOX);
    OutputActivity->color(FL_BACKGROUND_COLOR);
    OutputActivity->tooltip(" What is currently done ");
    OutputActivity->clear_visible_focus();

    (new LFl_Box("Assemblies: ", 0, 1, 1, 1))->stretchRight();
    OutputAssemblies = new LFl_Value_Output(1, 1, 1, 1);
    OutputAssemblies->box(FL_FLAT_BOX);
    OutputAssemblies->step(1);   // make output NOT use scientific presentation for big numbers
    OutputAssemblies->tooltip(" Number of assemblies found so far ");
    ((LFl_Value_Output*)OutputAssemblies)->weight(2, 0);

    (new LFl_Box("Solutions: ", 0, 2, 1, 1))->stretchRight();
    OutputSolutions = new LFl_Value_Output(1, 2, 1, 1);
    OutputSolutions->box(FL_FLAT_BOX);
    OutputSolutions->step(1);    // make output NOT use scientific presentation for big numbers
    OutputSolutions->tooltip(" Number of solutions (assemblies that can be disassembled) found so far ");

    (new LFl_Box("Time used: ", 2, 1, 1, 1))->stretchRight();
    TimeUsed = new LFl_Output(3, 1, 1, 1);
    TimeUsed->box(FL_NO_BOX);
    ((LFl_Output*)TimeUsed)->weight(4, 0);

    (new LFl_Box("Time left: ", 2, 2, 1, 1))->stretchRight();
    TimeEst = new LFl_Output(3, 2, 1, 1);
    TimeEst->box(FL_NO_BOX);
    TimeEst->tooltip(" This is a very approximate estimate and can be totally wrong, to take with a grain of salt ");

    o->end();

    group->end();
  }

  {
    Layouter * group = new Layouter(0, 1);
    group->box(FL_FLAT_BOX);

    new LSeparator_c(0, 0, 1, 1, "Solutions", true);

    Layouter * o = new Layouter(0, 1);

    new LFl_Box("Solution", 0, 0, 1, 1);

    // Gap between Solution and value
    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);

    SolutionsInfo = new LFl_Value_Output(2, 0, 1, 1);
    SolutionsInfo->tooltip(" Number of solutions ");
    SolutionsInfo->box(FL_FLAT_BOX);
    ((LFl_Value_Output*)SolutionsInfo)->weight(1, 0);

    SolutionSel = new LFl_Value_Slider(0, 1, 3, 1);
    SolutionSel->tooltip(" Select one Solution ");
    SolutionSel->value(1);
    SolutionSel->type(1);
    SolutionSel->step(1);
    SolutionSel->callback(cb_SolutionSel_stub, this);
    SolutionSel->align(FL_ALIGN_TOP_LEFT);

    (new LFl_Box(0, 3))->setMinimumSize(0, SZ_GAP);

    new LFl_Box("Move", 0, 4, 1, 1);

    MovesInfo = new LFl_Output(2, 4, 1, 1);
    MovesInfo->tooltip(" Steps for complete disassembly ");
    MovesInfo->box(FL_FLAT_BOX);
    MovesInfo->color(FL_BACKGROUND_COLOR);
    ((LFl_Output*)MovesInfo)->weight(1, 0);

    SolutionAnim = new LFl_Value_Slider(0, 5, 3, 1);
    SolutionAnim->tooltip(" Animate the disassembly ");
    SolutionAnim->type(1);
    SolutionAnim->step(0.02);
    SolutionAnim->callback(cb_SolutionAnim_stub, this);
    SolutionAnim->align(FL_ALIGN_TOP_LEFT);

    o->end();

    (new LFl_Box(0, 2))->setMinimumSize(0, SZ_GAP);

    o = new Layouter(0, 3);

    new LFl_Box("Assembly:", 0, 0, 1, 1);
    new LFl_Box("Solution:", 2, 0, 1, 1);

    AssemblyNumber = new LFl_Value_Output(1, 0, 1, 1);
    SolutionNumber = new LFl_Value_Output(3, 0, 1, 1);
    AssemblyNumber->box(FL_FLAT_BOX);
    SolutionNumber->box(FL_FLAT_BOX);
    AssemblyNumber->step(1);    // make output NOT use scientific presentation for big numbers
    SolutionNumber->step(1);    // make output NOT use scientific presentation for big numbers
    ((LFl_Value_Output*)AssemblyNumber)->weight(1, 0);
    ((LFl_Value_Output*)SolutionNumber)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 4))->setMinimumSize(0, SZ_GAP);

    o = new Layouter(0, 5);

    new LFl_Box("Sort by: ", 0, 0);

    BtnSrtFind =  new LFlatButton_c(1, 0, 1, 1, "Number", " Sort in the order the solutions were found ", cb_SrtFind_stub, this);
    ((LFlatButton_c*)BtnSrtFind)->weight(1, 0);
    (new LFl_Box(2, 0))->setMinimumSize(SZ_GAP, 0);
    BtnSrtLevel = new LFlatButton_c(3, 0, 1, 1, "Level", " Sort in the order of increasing level ", cb_SrtLevel_stub, this);
    ((LFlatButton_c*)BtnSrtLevel)->weight(1, 0);
    (new LFl_Box(4, 0))->setMinimumSize(SZ_GAP, 0);
    BtnSrtMoves = new LFlatButton_c(5, 0, 1, 1, "Disasm", " Sort in the order of increasing moves for complete disassembly ", cb_SrtMoves_stub, this);
    ((LFlatButton_c*)BtnSrtMoves)->weight(1, 0);
    (new LFl_Box(6, 0))->setMinimumSize(SZ_GAP, 0);
    BtnSrtPieces = new LFlatButton_c(7, 0, 1, 1, "Pieces", " Sort in the order of used pieces ", cb_SrtPieces_stub, this);
    ((LFlatButton_c*)BtnSrtPieces)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 6))->setMinimumSize(0, SZ_GAP);

    o = new Layouter(0, 7);

    new LFl_Box("Delete: ", 0, 0);

    BtnDelAll =    new LFlatButton_c(1, 0, 1, 1, "All", " Delete all solutions ", cb_DelAll_stub, this);
    ((LFlatButton_c*)BtnDelAll)->weight(1, 0);
    (new LFl_Box(2, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDelBefore = new LFlatButton_c(3, 0, 1, 1, "Before", " Delete all before the currently selected one ", cb_DelBefore_stub, this);
    ((LFlatButton_c*)BtnDelBefore)->weight(1, 0);
    (new LFl_Box(4, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDelAt =     new LFlatButton_c(5, 0, 1, 1, "At", " Delete current solution ", cb_DelAt_stub, this);
    ((LFlatButton_c*)BtnDelAt)->weight(1, 0);
    (new LFl_Box(6, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDelAfter =  new LFlatButton_c(7, 0, 1, 1, "After", " Delete all solutions after the currently selected one ", cb_DelAfter_stub, this);
    ((LFlatButton_c*)BtnDelAfter)->weight(1, 0);
    (new LFl_Box(8, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDelDisasm = new LFlatButton_c(9, 0, 1, 1, "w/o DA", " Delete all solutions without valid disassembly ", cb_DelDisasmless_stub, this);
    ((LFlatButton_c*)BtnDelDisasm)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 8))->setMinimumSize(0, SZ_GAP);

    o = new Layouter(0, 9);

    BtnDisasmDel    = new LFlatButton_c(0, 0, 1, 1, "D DA", " Remove the disassembly for the current solution ", cb_DelDisasm_stub, this);
    ((LFlatButton_c*)BtnDisasmDel)->weight(1, 0);
    (new LFl_Box(1, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDisasmDelAll = new LFlatButton_c(2, 0, 1, 1, "D A DA", " Remove the disassemblies for all solutions ", cb_DelAllDisasm_stub, this);
    ((LFlatButton_c*)BtnDisasmDelAll)->weight(1, 0);
    (new LFl_Box(3, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDisasmAdd    = new LFlatButton_c(4, 0, 1, 1, "A DA", " Recalculate the disassembly for the current solution ", cb_AddDisasm_stub, this);
    ((LFlatButton_c*)BtnDisasmAdd)->weight(1, 0);
    (new LFl_Box(5, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDisasmAddAll = new LFlatButton_c(6, 0, 1, 1, "A A DA", " Recalculate the disassemblies for all solutions ", cb_AddAllDisasm_stub, this);
    ((LFlatButton_c*)BtnDisasmAddAll)->weight(1, 0);
    (new LFl_Box(7, 0))->setMinimumSize(SZ_GAP, 0);
    BtnDisasmAddMissing=new LFlatButton_c(8, 0, 1, 1, "A M DA", " Recalculate the missing disassemblies for all solutions without valid disassembly ", cb_AddMissingDisasm_stub, this);
    ((LFlatButton_c*)BtnDisasmAddMissing)->weight(1, 0);

    o->end();

    (new LFl_Box(0, 10))->setMinimumSize(0, SZ_GAP);

    PcVis = new PieceVisibility(0, 0, 100, 100);
    LBlockListGroup * shapeGroup = new LBlockListGroup(0, 11, 1, 1, PcVis);
    shapeGroup->callback(cb_PcVis_stub, this);
    shapeGroup->tooltip(" Change appearance of the pieces between normal, grid and invisible ");
    shapeGroup->weight(1, 1);

    group->end();
  }
  tile->end();

  TabSolve->resizable(tile);
  TabSolve->end();
}

void MainWindow::activateConfigOptions() {

  if (config.useTooltips())
    Fl_Tooltip::enable();
  else
    Fl_Tooltip::disable();

  View3D->getView()->useLightning(config.useLightning());
  View3D->getView()->setRotaterMethod(config.rotationMethod());
}

MainWindow::MainWindow(std::unique_ptr<GridType> grid_type)
  : LFl_Double_Window(true) {

  assmThread = 0;
  fname = 0;
  disassemble = 0;
  editSymmetries = 0;
  expertMode = true;

  puzzle_ = std::make_unique<Puzzle>(std::move(grid_type));
  gui_grid_type_ = std::make_unique<GuiGridType>(puzzle_->getGridType());
  changed = false;

  label("BurrTools - unknown");
  user_data((void*)(this));

  MainMenu = new LFl_Menu_Bar(0, 0, 1, 1);
  MainMenu->copy(menu_MainMenu, this);

  StatusLine = new LStatusLine(0, 2, 1, 1);
  StatusLine->callback(cb_Status_stub, this);

  LFl_Tile * mainTile = new LFl_Tile(0, 1, 1, 1);
  mainTile->weight(0, 1);

  Layouter * lay = new Layouter(1, 0, 1, 1);
  View3D = new LView3dGroup(0, 0, 1, 1);
  lay->weight(1, 0);
  lay->end();
  lay->setMinimumSize(400, 400);
  View3D->weight(0, 1);
  View3D->callback(cb_3dClick_stub, this);

  // this box paints the background behind the tab, because the tabs are partly transparent
  (new LFl_Box(0, 0, 1, 1))->color(FL_BACKGROUND_COLOR);

  // the tab for the tool bar
  TaskSelectionTab = new LFl_Tabs(0, 0, 1, 1);
  TaskSelectionTab->callback(cb_TaskSelectionTab_stub, this);
  TaskSelectionTab->clear_visible_focus();

  // the three tabs
  CreateShapeTab();
  CreateProblemTab();
  CreateSolveTab();

  currentTab = 0;
  ViewSizes[0] = -1;
  ViewSizes[1] = -1;
  ViewSizes[2] = -1;

  resize(config.windowPosX(), config.windowPosY(), config.windowPosW(), config.windowPosH());

  if (!config.useRubberband())
    editMode->select(1);
  else
    editMode->select(0);

  is3DViewBig = true;
  shapeEditorWithBig3DView = true;

  updateInterface();
  activateClear();

  // set the edit mode from the selected mode
  cb_EditChoice();

  activateConfigOptions();
}

MainWindow::~MainWindow() {

  config.windowPos(x(), y(), w(), h());

  if (assmThread) {
    delete assmThread;
    assmThread = 0;
  }

  delete puzzle_;

  if (fname) {
    delete [] fname;
    fname = 0;
  }

  if (disassemble) {
    delete disassemble;
    disassemble = 0;
  }

  if (gui_grid_type_)
    delete gui_grid_type_;
}
