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
#ifndef __VECTOR_EXPORT_WINDOW_H__
#define __VECTOR_EXPORT_WINDOW_H__

#include "Layouter.h"

#include "voxelframe.h"

class vectorExportWindow_c : public LFl_Double_Window {

    LFl_Input * inp;
    Layouter * radGroup;

  public:

    vectorExportWindow_c(void);

    const char * getFileName(void);
    voxelFrame_c::VectorFiletype getVectorType(void);

    /* callback functions */
    void cb_FileChoose(void);

    bool cancelled;

};

#endif
