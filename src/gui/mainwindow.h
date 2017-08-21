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
#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include "Images.h"

#include "Layouter.h"

class VoxelEditGroup;
class ChangeSize;
class ToolTab;
class Puzzle;
class SolveThread;
class DisassemblyToMoves;
class GridType;
class GuiGridType;
class Layouter;

class PieceSelector;
class ProblemSelector;
class ColorSelector;
class ResultViewer;
class PiecesList;
class PieceVisibility;
class ColorConstraintsEdit;
class ToolTabContainer;
class ButtonGroup;
class FlatButton;
class LStatusLine;
class LBlockListGroup;
class LView3dGroup;

class Fl_Tabs;
class Fl_Group;
class Fl_Check_Button;
class Fl_Value_Output;
class Fl_Output;
class Fl_Value_Slider;
class Fl_Value_Input;
class Fl_Menu_Bar;
class Fl_Choice;
class Fl_Progress;

class MainWindow : public LFl_Double_Window {
  std::unique_ptr<Puzzle> puzzle_;
  std::unique_ptr<GuiGridType> gui_grid_type_;  // this is the guigridtype for the puzzle, is must
  // always be in sync
  char * fname;
  DisassemblyToMoves * disassemble;
  SolveThread *assmThread;
  bool SolutionEmpty;
  bool changed;
  int editSymmetries;

  bool expertMode;

  pixmapList_c pm;

  Fl_Tabs *TaskSelectionTab;
  Layouter *TabPieces;
  Fl_Group *MinSizeSelector;

  PieceSelector * PcSel;
  ProblemSelector * problemSelector;
  ProblemSelector * solutionProblem;
  ColorSelector * colorAssignmentSelector;
  PieceSelector * shapeAssignmentSelector;
  ResultViewer * problemResult;
  PiecesList * PiecesCountList;
  PieceVisibility * PcVis;
  ColorConstraintsEdit * colconstrList;

  Layouter *TabProblems;

  ToolTabContainer * pieceTools;
  ButtonGroup *editChoice;
  ButtonGroup *editMode;

  Layouter *TabSolve;
  Fl_Check_Button *SolveDisasm, *JustCount, *DropDisassemblies, *KeepMirrors, *KeepRotations, *CompleteRotations;

  FlatButton *BtnPrepare, *BtnStart, *BtnCont, *BtnStop, *BtnPlacement, *BtnStep, *BtnMovement;
  FlatButton *BtnNewShape, *BtnDelShape, *BtnCpyShape, *BtnRenShape, *BtnShapeLeft, *BtnShapeRight, *BtnWeightInc, *BtnWeightDec;
  FlatButton *BtnNewColor, *BtnDelColor, *BtnChnColor;
  FlatButton *BtnNewProb, *BtnDelProb, *BtnCpyProb, *BtnRenProb, *BtnProbLeft, *BtnProbRight;
  FlatButton *BtnColSrtPc, *BtnColSrtRes, *BtnColAdd, *BtnColRem;
  FlatButton *BtnSetResult, *BtnAddShape, *BtnRemShape, *BtnMinZero, *BtnAddAll, *BtnRemAll, *BtnGroup, *BtnProbShapeLeft, *BtnProbShapeRight;

  Fl_Progress *SolvingProgress;
  Fl_Value_Output *OutputAssemblies;
  Fl_Value_Output *OutputSolutions;
  Fl_Output *OutputActivity;
  Fl_Check_Button *ReducePositions;
  Fl_Value_Slider *SolutionSel;
  Fl_Value_Slider *SolutionAnim;
  Fl_Value_Output *SolutionsInfo;
  Fl_Output *MovesInfo;

  Fl_Output *TimeUsed, *TimeEst;

  LView3dGroup * View3D;

  Fl_Group *MinSizeTools;
  Fl_Menu_Bar *MainMenu;
  LStatusLine *StatusLine;
  static Fl_Menu_Item menu_MainMenu[];

  ColorSelector * colorSelector;

  VoxelEditGroup *pieceEdit;

  Fl_Choice * sortMethod;
  Fl_Value_Input *solDrop, *solLimit;

  Fl_Value_Output *SolutionNumber, *AssemblyNumber;

  FlatButton *BtnSrtFind, *BtnSrtLevel, *BtnSrtMoves, *BtnSrtPieces;
  FlatButton *BtnDelAll, *BtnDelBefore, *BtnDelAt, *BtnDelAfter, *BtnDelDisasm;
  FlatButton *BtnDisasmDel, *BtnDisasmDelAll, *BtnDisasmAdd, *BtnDisasmAddAll, *BtnDisasmAddMissing;

  // the zoom levels for all 3 tabs independent, so that the problem
  // tab can have a wider view
  double ViewSizes[3];
  int currentTab;

  bool tryToLoad(const char *fname);

  void CreateShapeTab();
  void CreateProblemTab();
  void CreateSolveTab();


  bool is3DViewBig;
  bool shapeEditorWithBig3DView;

  void Toggle3DView();
  void Big3DView();
  void Small3DView();

  void StatPieceInfo(unsigned int pc);
  void StatProblemInfo(unsigned int pr);

  void changeShape(unsigned int nr);
  void changeProblem(unsigned int nr);
  void changeColor(unsigned int nr);

  void ReplacePuzzle(Puzzle * newPuzzle);

  void activateShape(unsigned int number);
  void activateProblem(unsigned int prob);
  void activateSolution(unsigned int prob, unsigned int num);
  void activateClear();

  bool threadStopped();

  void updateInterface();

public:
  MainWindow(std::unique_ptr<GridType> grid_type);
  virtual ~MainWindow();

  int handle(int event);

  void show(int argn, char ** argv);

  // overwrite hide to check for changes in all possible exit situations
  void hide();

  /* this is used on assert to save the current puzzle */
  const Puzzle* getPuzzle() const { return puzzle_.get(); }

  /* update the interface to represent the latest state of
   * the solving progress, that works in background
   */
  void update();

  /* return an index into the main menu array with the given text */
  int findMenuEntry(const char * txt);

  /* the callback functions, as they are called from normal functions we need
   * to make them public, even though they should not be used from the outside
   */
  void cb_AddColor();
  void cb_RemoveColor();
  void cb_ChangeColor();

  void cb_NewShape();
  void cb_DeleteShape();
  void cb_CopyShape();
  void cb_NameShape();
  void cb_ShapeExchange(int with);
  void cb_WeightChange(int by);

  void cb_NewProblem();
  void cb_DeleteProblem();
  void cb_CopyProblem();
  void cb_RenameProblem();
  void cb_ProblemExchange(int with);

  void cb_ColorAssSel();
  void cb_ColorConstrSel();

  void cb_ShapeToResult();

  void cb_TaskSelectionTab(Fl_Tabs*);

  void cb_SelectProblemShape();
  void cb_AddShapeToProblem();
  void cb_SetShapeMinimumToZero();
  void cb_AddAllShapesToProblem();
  void cb_RemoveShapeFromProblem();
  void cb_RemoveAllShapesFromProblem();
  void cb_ProbShapeExchange(int with);

  void cb_PcSel(LBlockListGroup* reason);
  void cb_ColSel(LBlockListGroup* reason);
  void cb_ProbSel(LBlockListGroup* reason);

  void cb_PiecesClicked();

  void cb_TransformPiece();
  void cb_pieceEdit(VoxelEditGroup* o);
  void cb_EditChoice();
  void cb_EditSym(int onoff, int value);
  void cb_EditMode();

  void cb_TransformResult();

  void cb_AllowColor();
  void cb_DisallowColor();
  void cb_CCSort(bool byResult);

  void cb_BtnPrepare();
  void cb_BtnStart(bool prep_only);
  void cb_BtnCont(bool prep_only);
  void cb_BtnStop();
  void cb_BtnPlacementBrowser();
  void cb_BtnMovementBrowser();
  void cb_BtnAssemblerStep();

  void cb_SolutionSel(Fl_Value_Slider*);
  void cb_SolutionAnim(Fl_Value_Slider*);

  void cb_PcVis();

  void cb_Status();
  void cb_3dClick();

  void cb_New();
  void cb_Load();
  void cb_Load_Ps3d();
  void cb_Save();
  void cb_SaveAs();
  void cb_Convert();
  void cb_AssembliesToShapes();
  void cb_Quit();
  void cb_About();
  void cb_Help();
  void cb_Config();
  void cb_Coment();
  void cb_Toggle3D();
  void cb_SolProbSel(LBlockListGroup* reason);

  void cb_ShapeGroup();
  void cb_ImageExport();
  void cb_ImageExportVector();
  void cb_STLExport();
  void cb_StatusWindow();

  void cb_SortSolutions(unsigned int by);
  void cb_DeleteSolutions(unsigned int which);

  void cb_DeleteDisasm();
  void cb_DeleteAllDisasm();
  void cb_AddDisasm();
  void cb_AddAllDisasm(bool all);

  void activateConfigOptions();
};

#endif
