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
#include "Layouter.h"

/* task      what we want to do with the endresult
 * widths    the widths of the different columns of the endgrid
 * heights   the heights of the rows in the calculated grid
 * widgetsW  the minimum widths calculated for each widget
 * widgetsH  the minimum heights for each widget
 * targetW and targetH only used when task = 1, it should be bigger than min size
 *      or strange thing may happen
 */
void layouter_c::calcLayout(int task, std::vector<int> *widths, std::vector<int> *heights,
                            std::vector<int> *widgetsW, std::vector<int> *widgetsH, int targetW, int targetH) const {

  /* right this routine is stolen from the gridbaglayouter of the gcc java awt class set */

  /* task = 0: minsize; task = 1: SIZE */

  /* these 2 contain the maximum column and rownumber */
  unsigned int max_x = 0;
  unsigned int max_y = 0;

  /* as the caller is not required to provide the last two vectors
   * we'll have local instances and set the pointer to them if
   * we need that
   */
  std::vector<int> ww, wh;

  if (!widgetsW) widgetsW = &ww;
  if (!widgetsH) widgetsH = &wh;

  /* resize the vectors to the correct size */
  widgetsW->resize(children());
  widgetsH->resize(children());

  Fl_Widget *const * _widgets = array();

  // first we figure out how many rows/columns
  for (int i = 0; i < children(); i++) {

    unsigned int gX, gY, gW, gH;

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    widget->getGridValues(&gX, &gY, &gW, &gH);

    int w, h;

    widget->getMinSize(&w, &h);

    /* check the min sizes of the current widget */

    if (w < widget->getMinWidth()) w = widget->getMinWidth();
    if (h < widget->getMinHeight()) h = widget->getMinHeight();

    (*widgetsW)[i] = w;
    (*widgetsH)[i] = h;

    max_x = (max_x > gX+gW) ? (max_x) : (gX+gW);
    max_y = (max_y > gY+gH) ? (max_y) : (gY+gH);
  }

  std::vector<int> weightsW(max_x), weightsH(max_y);
  widths->resize(max_x);
  heights->resize(max_y);

  /* initialize the vectors */
  for (unsigned int ii = 0; ii < max_x; ii++) {
    (*widths)[ii] = 0;
    weightsW[ii] = 0;
  }

  for (unsigned int ii = 0; ii < max_y; ii++) {
    (*heights)[ii] = 0;
    weightsH[ii] = 0;
  }

  /* separate the process into two steps. First put in the
   * widgets that have a fixed column and then place the other
   * ones. The wider ones can now have a more optimal
   * stretching of the column
   */
  for (int i = 0; i < children(); i++) {
    unsigned int gX, gY, gW, gH;

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    widget->getGridValues(&gX, &gY, &gW, &gH);

    if (gW > 1) continue;

    int width = (*widgetsW)[i];

    width += 2 * widget->getPitch();

    if (width > (*widths)[gX])
      (*widths)[gX] = width;

    if (widget->getWeightX() > weightsW[gX])
      weightsW[gX] = widget->getWeightX();
  }

  for (int i = 0; i < children(); i++) {
    unsigned int gX, gY, gW, gH;

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    widget->getGridValues(&gX, &gY, &gW, &gH);

    if (gW < 2) continue;

    /* find the column with the biggest weight */
    unsigned int bestW = gX;
    for (unsigned int w = gX + 1; w < gX+gW; w++)
      if (weightsW[w] >= weightsW[bestW])
        bestW = w;

    int width = (*widgetsW)[i];

    width += 2 * widget->getPitch();

    for (unsigned int w = gX; w < gX+gW; w++)
      if (w != bestW)
        width -= (*widths)[w];

    if (width < 0) width = 0;

    if (width > (*widths)[bestW])
      (*widths)[bestW] = width;

    if (widget->getWeightX() > weightsW[bestW])
      weightsW[bestW] = widget->getWeightX();
  }

  /* calculate rows, see columns */
  for (int i = 0; i < children(); i++) {
    unsigned int gX, gY, gW, gH;

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    widget->getGridValues(&gX, &gY, &gW, &gH);

    if (gH > 1) continue;

    int height = (*widgetsH)[i];

    height += 2 * widget->getPitch();

    if (height > (*heights)[gY])
      (*heights)[gY] = height;

    if (widget->getWeightY() > weightsH[gY])
      weightsH[gY] = widget->getWeightY();
  }

  for (int i = 0; i < children(); i++) {
    unsigned int gX, gY, gW, gH;

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    widget->getGridValues(&gX, &gY, &gW, &gH);

    if (gH < 2) continue;

    /* find the column with the biggest weight */
    unsigned int bestH = gY;
    for (unsigned int h = gY + 1; h < gY+gH; h++)
      if (weightsH[h] >= weightsH[bestH])
        bestH = h;

    int height = (*widgetsH)[i];

    height += 2 * widget->getPitch();

    for (unsigned int h = gY; h < gY+gH; h++)
      if (h != bestH)
        height -= (*heights)[h];

    if (height < 0) height = 0;

    if (height > (*heights)[bestH])
      (*heights)[bestH] = height;

    if (widget->getWeightY() > weightsH[bestH])
      weightsH[bestH] = widget->getWeightY();
  }

  if (task == 1) {
    /* stretch to final size if we do an actual layout we need to do this
     * here because the weight vectors are not exported
     */

    /* sum up the weight values */
    int sumWeightX = 0, sumWeightY = 0, sumW = 0, sumH = 0;

    for (unsigned int x = 0; x < max_x; x++) {
      sumWeightX += weightsW[x];
      sumW += (*widths)[x];
    }
    for (unsigned int y = 0; y < max_y; y++) {
      sumWeightY += weightsH[y];
      sumH += (*heights)[y];
    }

    /* now add the missing difference to the rows and columns */
    int dW = targetW - sumW;
    int dH = targetH - sumH;

    if (dW > 0)
      for (unsigned int x = 0; x < max_x; x++) {

        int add = (sumWeightX) ? (dW * weightsW[x] / sumWeightX) : (dW / (max_x-x));

        (*widths)[x] += add;
        dW -= add;
        sumWeightX -= weightsW[x];

        if (dW <= 0)
          break;
      }

    if (dH > 0)
      for (unsigned int y = 0; y < max_y; y++) {

        int add = (sumWeightY) ? (dH * weightsH[y] / sumWeightY) : (dH / (max_y-y));

        (*heights)[y] += add;
        dH -= add;
        sumWeightY -= weightsH[y];

        if (dH <= 0)
          break;
      }
  }
}

void layouter_c::resize(int xt, int yt, int w, int h) {

  /* first step calculate the widths of the rows and columns */
  std::vector<int> widths;
  std::vector<int> heights;

  std::vector<int> widgetW;
  std::vector<int> widgetH;

  calcLayout(1, &widths, &heights, &widgetW, &widgetH, w, h);

  /* check, if we need to make our widget bigger to accommodate all subwidgets */
  int wi = 0, hi = 0;
  for (unsigned int i = 0; i < widths.size(); i++)
    wi += widths[i];
  for (unsigned int i = 0; i < heights.size(); i++)
    hi += heights[i];

//  if (wi > w) w = wi;
//  if (hi > h) h = hi;

  Fl_Widget::resize(xt, yt, w, h);

  Fl_Widget *const * _widgets = array();

  /* and now layout widgets according to the plan */
  for (int i = 0; i < children(); i++) {

    int xp, yp, w, h;
    xp = yp = h = w = 0;

    unsigned int gx, gy, gw, gh;

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    widget->getGridValues(&gx, &gy, &gw, &gh);

    for (unsigned int j = 0; j < gx; j++) xp += widths[j];
    for (unsigned int j = gx; j < gx+gw; j++) w += widths[j];

    for (unsigned int j = 0; j < gy; j++) yp += heights[j];
    for (unsigned int j = gy; j < gy+gh; j++) h += heights[j];

    /* pitch handling */
    int p = widget->getPitch();
    xp += p;
    yp += p;
    w -= 2*p;
    h -= 2*p;

    unsigned int minHW = (widgetW[i] < widgetH[i]) ? (widgetW[i]) : (widgetH[i]);

    /* stretch handling */
    switch (widget->getStretchX()) {
    case STRETCH_MINUS:
      w = widgetW[i];
      break;
    case STRETCH_PLUS:
      xp += w - widgetW[i];
      w = widgetW[i];
      break;
    case STRETCH_MIDDLE:
      xp += (w - widgetW[i]) / 2;
      w = widgetW[i];
      break;
    case STRETCH_SQUARE:
      xp += (w - minHW) / 2;
      w = minHW;
      break;
    case STRETCH:
      break;
    }

    switch (widget->getStretchY()) {
    case STRETCH_MINUS:
      h = widgetH[i];
      break;
    case STRETCH_PLUS:
      yp += h - widgetH[i];
      h = widgetH[i];
      break;
    case STRETCH_MIDDLE:
      yp += (h - widgetH[i]) / 2;
      h = widgetH[i];
      break;
    case STRETCH_SQUARE:
      yp += (h - minHW) / 2;
      h = minHW;
      break;
    case STRETCH:
      break;
    }

    /* finally place the widget */
    _widgets[i]->resize(xp+x(), yp+y(), w, h);
  }
}

void layouter_c::getMinSize(int *width, int *height) const {

  if (!minsizeValid) {

    std::vector<int> widths;
    std::vector<int> heights;

    ((layouter_c*)this)->mw = ((layouter_c*)this)->mh = 0;

    if (children()) {

      calcLayout(0, &widths, &heights, 0, 0);

      /* accumulate the rows and columns */
      for (unsigned int i = 0; i < widths.size(); i++) ((layouter_c*)this)->mw += widths[i];
      for (unsigned int i = 0; i < heights.size(); i++) ((layouter_c*)this)->mh += heights[i];
    }

    ((layouter_c*)this)->minsizeValid = true;
  }

  *width = mw;
  *height = mh;
}

void layouter_c::remove(Fl_Widget &w) {
  minsizeValid = false;
  Fl_Group::remove(w);
}
void layouter_c::remove(Fl_Widget *w) {
  minsizeValid = false;
  Fl_Group::remove(w);
}
void layouter_c::add(Fl_Widget &w) {
  minsizeValid = false;
  Fl_Group::add(w);
}

void layouter_c::add(Fl_Widget *w) {
  minsizeValid = false;
  Fl_Group::add(w);
}


void LFl_Tabs::getMinSize(int *width, int *height) const {

  Fl_Widget *const * _widgets = array();
  *height = *width = 10;

  for (int i = 0; i < children(); i++) {

    int w, h;

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    widget->getMinSize(&w, &h);

    w += 2 * widget->getPitch();
    h += 2 * widget->getPitch();

    if (w > *width) *width = w;
    if (h > *height) *height = h;
  }

  *height += 20;
}

void LFl_Tabs::resize(int x, int y, int w, int h) {

  Fl_Tabs::resize(x, y, w, h);

  Fl_Widget *const * _widgets = array();

  for (int i = 0; i < children(); i++) {

    layoutable_c * widget = dynamic_cast<layoutable_c*>(_widgets[i]);

    unsigned int p = widget->getPitch();

    _widgets[i]->resize(x+p, y+20+p, w-2*p, h-20-2*p);
  }
}


void LFl_Scroll::getMinSize(int *width, int *height) const {

  lay->getMinSize(width, height);

  switch(type()) {
    case Fl_Scroll::HORIZONTAL_ALWAYS: // - Horizontal scrollbar always on, vertical always off.
    case Fl_Scroll::HORIZONTAL:// - Only a horizontal scrollbar.
      *width = 30;
      *height += hscrollbar.h();
      break;
    case Fl_Scroll::VERTICAL_ALWAYS: // - Vertical scrollbar always on, horizontal always off.
    case Fl_Scroll::VERTICAL:// - Only a vertical scrollbar.
      *height = 30;
      *width += scrollbar.w();
      break;
    case Fl_Scroll::BOTH_ALWAYS: // - Both always on.
    case Fl_Scroll::BOTH: // - The default is both scrollbars.
      *width = *height = 30;
      break;
  }
}

void LFl_Scroll::resize(int x, int y, int w, int h) {

  int lw, lh;

  lay->getMinSize(&lw, &lh);

  lay->resize(lay->x(), lay->y(), lw, lh);

  Fl_Scroll::resize(x, y, w, h);
}

void LFl_Double_Window::show() {

  if (!shown()) {

    int wy, hy;
    lay->getMinSize(&wy, &hy);

    if (res)
      size_range(wy, hy, 0, 0);
    else
      size_range(wy, hy, wy, hy);

    if (!placed || wy > w() || hy > h()) {
      size(wy, hy);

      // center the window
      {
        int sw, sh, sx, sy;
        Fl::screen_xywh(sx, sy, sw, sh);

        position((sw-wy)/2+sx, (sh-hy)/2+sy);
      }
    }
  }

  Fl_Double_Window::show();
}


