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
#ifndef __MILLABLE_H__
#define __MILLABLE_H__

class Voxel;

/**
 * Check, if a piece is notchable.
 * a piece is notchable, if it can be made purely by cutting
 *
 * This function only works for cube voxel spaces right now
 */
bool isNotchable(const Voxel *v);

/**
 * Check, if a shape is millable.
 * a piece is millable, if it contains no inside corners
 *
 * This function only works for cube voxel spaces right now
 */
bool isMillable(const Voxel *v);

#endif
