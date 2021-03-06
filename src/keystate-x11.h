/*
 *    libxw
 *    Copyright (C) 2021  wb9688
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __KEYSTATE_X11_H__
#define __KEYSTATE_X11_H__

#include "keystate.h"

G_BEGIN_DECLS

#define XW_TYPE_KEYSTATE_X11 xw_keystate_x11_get_type()
G_DECLARE_FINAL_TYPE(XwKeystateX11, xw_keystate_x11, XW, KEYSTATE_X11, GObject)

gboolean xw_keystate_x11_is_supported();

G_END_DECLS

#endif
