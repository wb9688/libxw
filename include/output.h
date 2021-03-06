/*
 *    libxw
 *    Copyright (C) 2021-2022  wb9688
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

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <glib-2.0/glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define XW_TYPE_OUTPUT xw_output_get_type()
G_DECLARE_INTERFACE(XwOutput, xw_output, XW, OUTPUT, GObject)

struct _XwOutputInterface {
    GTypeInterface parent;

    void (*destroy)(XwOutput *self);

    gchar *(*get_name)(XwOutput *self);
    gchar *(*get_description)(XwOutput *self);

    gboolean (*get_enabled)(XwOutput *self);
    void (*set_enabled)(XwOutput *self, gboolean enabled);
};

gchar *xw_output_get_name(XwOutput *self);
gchar *xw_output_get_description(XwOutput *self);

gboolean xw_output_get_enabled(XwOutput *self);
void xw_output_set_enabled(XwOutput *self, gboolean enabled);

G_END_DECLS

#endif
