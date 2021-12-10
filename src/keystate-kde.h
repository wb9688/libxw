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

#ifndef __KEYSTATE_KDE_H__
#define __KEYSTATE_KDE_H__

#include <glib-2.0/glib.h>

#include "keystate.h"

G_BEGIN_DECLS

#define XW_TYPE_KEYSTATE_KDE xw_keystate_kde_get_type()
G_DECLARE_FINAL_TYPE(XwKeystateKde, xw_keystate_kde, XW, KEYSTATE_KDE, GObject)

XwKeystateState *xw_keystate_kde_get_state(XwKeystate *self);

gboolean xw_keystate_kde_is_supported();

G_END_DECLS

#endif
