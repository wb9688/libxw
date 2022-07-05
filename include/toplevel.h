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

#ifndef __TOPLEVEL_H__
#define __TOPLEVEL_H__

#include <glib-2.0/glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define XW_TYPE_TOPLEVEL xw_toplevel_get_type()
G_DECLARE_INTERFACE(XwToplevel, xw_toplevel, XW, TOPLEVEL, GObject)

struct _XwToplevelInterface {
    GTypeInterface parent;

    void (*destroy)(XwToplevel *self);

    gchar *(*get_title)(XwToplevel *self);
    gchar *(*get_app_id)(XwToplevel *self);
};

gchar *xw_toplevel_get_title(XwToplevel *self);
gchar *xw_toplevel_get_app_id(XwToplevel *self);

G_END_DECLS

#endif
