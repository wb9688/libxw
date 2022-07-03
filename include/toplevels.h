/*
 *    libxw
 *    Copyright (C) 2022  wb9688
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

#ifndef __TOPLEVELS_H__
#define __TOPLEVELS_H__

#include <glib-2.0/glib.h>
#include <glib-object.h>

#include "toplevel.h"

G_BEGIN_DECLS

#define XW_TYPE_TOPLEVELS xw_toplevels_get_type()
G_DECLARE_INTERFACE(XwToplevels, xw_toplevels, XW, TOPLEVELS, GObject)

struct _XwToplevelsInterface {
    GTypeInterface parent;

    void (*new_toplevel)(XwToplevels *self, XwToplevel *toplevel);

    GList *(*get_toplevels)(XwToplevels *self);
};

GList *xw_toplevels_get_toplevels(XwToplevels *self);

XwToplevels *xw_toplevels_new();

G_END_DECLS

#endif
