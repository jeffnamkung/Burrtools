//
// "$Id: Fl_Help_Dialog.cxx 4721 2005-12-19 16:52:11Z matt $"
//
// Fl_Help_Dialog dialog for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2005 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

// generated by Fast Light User Interface Designer (fluid) version 1.0107

#include "Fl_Help_Dialog.h"
#include "flstring.h"
#include <FL/fl_ask.H>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>

#include "../flu/Flu_File_Chooser.h"
#include "../help/helpdata.h"

#include <png.h>
#include <string.h>
#include <stdio.h>

filestruct* filelist = new filestruct[100];
imagestruct* imagelist = new imagestruct[100];

void cb_view_(Fl_Help_View* /*o*/, void* v) { ((Fl_Help_Dialog*)v)->cb_view__i(); }
void Fl_Help_Dialog::cb_view__i() {
  if (view_->filename()) {
    if (view_->changed()) {
      index_ ++;

      if (index_ >= 100) {
        memmove(line_, line_ + 10, sizeof(line_[0]) * 90);
        memmove(file_, file_ + 10, sizeof(file_[0]) * 90);
        index_ -= 10;
      }

      max_ = index_;

      strlcpy(file_[index_], view_->filename(),sizeof(file_[0]));
      line_[index_] = view_->topline();

      if (index_ > 0)
        back_->activate();
      else
        back_->deactivate();

      forward_->deactivate();
      label(view_->title());
    }
    else // if ! view_->changed()
    {
      strlcpy(file_[index_], view_->filename(), sizeof(file_[0]));
      line_[index_] = view_->topline();
    }
  } else { // if ! view_->filename()

    index_ = 0; // hitting an internal page will disable the back/fwd buffer
    file_[index_][0] = 0; // unnamed internal page
    line_[index_] = view_->topline();
    back_->deactivate();
    forward_->deactivate();
  }
}

void cb_Save(Fl_Button* /*o*/, void* v) { ((Fl_Help_Dialog*)(v))->cb_save__i(); }
void Fl_Help_Dialog::cb_save__i() {

#ifndef BT_EXTERNAL
  const char * path = flu_dir_chooser("Select directory to save files to", "./");

  int i = 0;

  while (filelist[i].name) {

    char n[10000];
    snprintf(n, 10000, "%s/%s", path, filelist[i].name);

    FILE * f = fopen(n, "wb");

    fwrite(filelist[i].data, filelist[i].size, 1, f);

    fclose(f);

    i++;
  }

  // now the images

  i = 0;

  while (imagelist[i].name) {

    char n[10000];
    snprintf(n, 10000, "%s/%s", path, imagelist[i].name);

    int sx = imagelist[i].w;
    int sy = imagelist[i].h;
    const unsigned char * buffer = imagelist[i].data;

    png_structp png_ptr;
    png_infop info_ptr;
    unsigned char ** png_rows = 0;
    int x, y;

    FILE *fi = fopen(n, "wb");
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL)
    {
      fclose(fi);
      png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
      fprintf(stderr, "\nError: Couldn't save the image!\n%s\n\n", n);
      return;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
      fclose(fi);
      png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
      fprintf(stderr, "\nError: Couldn't save the image!\n%s\n\n", n);
      return;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {

      /* if we have already instantiated the png_rows, we need to free them */
      if (png_rows) {
        for (y = 0; y < sy; y++)
          delete [] png_rows[y];

        delete [] png_rows;
      }

      fclose(fi);
      png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
      fprintf(stderr, "\nError: Couldn't save the image!\n%s\n\n", n);
      return;
    }

    png_init_io(png_ptr, fi);
    png_set_IHDR(png_ptr, info_ptr, sx, sy, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    /* Save the picture: */

    png_rows = new unsigned char*[sy];
    for (y = 0; y < sy; y++)
    {
      png_rows[y] = new unsigned char[4*sx];

      for (x = 0; x < sx; x++)
      {
        png_rows[y][x * 3 + 0] = buffer[(y*sx+x)*3+0];
        png_rows[y][x * 3 + 1] = buffer[(y*sx+x)*3+1];
        png_rows[y][x * 3 + 2] = buffer[(y*sx+x)*3+2];
      }
    }

    png_write_image(png_ptr, png_rows);

    for (y = 0; y < sy; y++)
      delete [] png_rows[y];

    delete [] png_rows;

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fi);

    i++;
  }

#else
  fl_message("This button only works in the internal BurrTools help");
#endif
}

void cb_back_(Fl_Button* /*o*/, void* v) { ((Fl_Help_Dialog*)v)->cb_back__i(); }
void Fl_Help_Dialog::cb_back__i() {
  if (index_ > 0)
    index_ --;

  if (index_ == 0)
    back_->deactivate();

  forward_->activate();

  int l = line_[index_];

  if (strcmp(view_->filename(), file_[index_]) != 0)
    view_->load(file_[index_]);

  view_->topline(l);
}

void cb_forward_(Fl_Button* /*o*/, void* v) { ((Fl_Help_Dialog*)v)->cb_forward__i(); }
void Fl_Help_Dialog::cb_forward__i() {
  if (index_ < max_)
    index_ ++;

  if (index_ >= max_)
    forward_->deactivate();

  back_->activate();

  int l = view_->topline();

  if (strcmp(view_->filename(), file_[index_]) != 0)
    view_->load(file_[index_]);

  view_->topline(l);
}

void cb_smaller_(Fl_Button* /*o*/, void* v) { ((Fl_Help_Dialog*)v)->cb_textsize__i(-2); }
void cb_larger_(Fl_Button* /*o*/, void* v) { ((Fl_Help_Dialog*)v)->cb_textsize__i(2); }
void Fl_Help_Dialog::cb_textsize__i(int inc) {
  textsize(textsize()+inc);
}

void cb_find_(Fl_Input* /*o*/, void* v) { ((Fl_Help_Dialog*)v)->cb_find__i(); }
void Fl_Help_Dialog::cb_find__i() { find_pos_ = view_->find(find_->value(), find_pos_); }

Fl_Help_Dialog::Fl_Help_Dialog() : Fl_Double_Window(530, 385, "Help Dialog") {
  {
    user_data((void*)(this));
    { Fl_Help_View* o = view_ = new Fl_Help_View(10, 10, 510, 330);
      o->box(FL_DOWN_BOX);
      o->color(FL_BACKGROUND_COLOR);
      o->selection_color(FL_SELECTION_COLOR);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(14);
      o->labelcolor(FL_FOREGROUND_COLOR);
      o->callback((Fl_Callback*)cb_view_, this);
      o->align(FL_ALIGN_TOP);
      o->when(FL_WHEN_RELEASE);
      o->end();
      Fl_Group::current()->resizable(o);
    }
    { Fl_Group* o = new Fl_Group(10, 348, 510, 27);
      { Fl_Button* o = save_ = new Fl_Button(456, 350, 64, 25, "Save");
        o->callback((Fl_Callback*)cb_Save, this);
      }
      { Fl_Button* o = back_ = new Fl_Button(386, 350, 25, 25, "@<-");
        o->tooltip("Show the previous help page.");
        o->shortcut(0xff51);
        o->labelcolor((Fl_Color)2);
        o->callback((Fl_Callback*)cb_back_, this);
      }
      { Fl_Button* o = forward_ = new Fl_Button(421, 350, 25, 25, "@->");
        o->tooltip("Show the next help page.");
        o->shortcut(0xff53);
        o->labelcolor((Fl_Color)2);
        o->callback((Fl_Callback*)cb_forward_, this);
      }
      { Fl_Button* o = smaller_ = new Fl_Button(316, 350, 25, 25, "F");
        o->tooltip("Make the help text smaller.");
        o->labelfont(1);
        o->labelsize(10);
        o->callback((Fl_Callback*)cb_smaller_, this);
      }
      { Fl_Button* o = larger_ = new Fl_Button(351, 350, 25, 25, "F");
        o->tooltip("Make the help text larger.");
        o->labelfont(1);
        o->labelsize(16);
        o->callback((Fl_Callback*)cb_larger_, this);
      }
      { Fl_Group* o = new Fl_Group(10, 350, 296, 25);
        o->box(FL_DOWN_BOX);
        o->color(FL_BACKGROUND2_COLOR);
        { Fl_Input* o = find_ = new Fl_Input(35, 352, 268, 21, "@search");
          o->tooltip("find text in document");
          o->box(FL_FLAT_BOX);
          o->labelsize(13);
          o->callback((Fl_Callback*)cb_find_, this);
          o->when(FL_WHEN_ENTER_KEY_ALWAYS);
          Fl_Group::current()->resizable(o);
        }
        o->end();
        Fl_Group::current()->resizable(o);
      }
      o->end();
    }
    size_range(260, 150);
    end();
  }
  back_->deactivate();
  forward_->deactivate();

  index_    = -1;
  max_      = 0;
  find_pos_ = 0;

  // AR fl_register_images();
}

void Fl_Help_Dialog::load(const char *f) {
  view_->set_changed();
  view_->load(f);
  label(view_->title());
}

void Fl_Help_Dialog::textsize(uchar s) {

  if (s <= 8) {
    smaller_->deactivate();
    s = 8;
  } else
    smaller_->activate();

  if (s >= 18) {
    s = 18;
    larger_->deactivate();
  } else
    larger_->activate();

  view_->textsize(s);
}

uchar Fl_Help_Dialog::textsize() {
  return (view_->textsize());
}

void Fl_Help_Dialog::topline(const char *n) {
  view_->topline(n);
}

