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

#ifndef __OUTPUTS_H__
#define __OUTPUTS_H__

#include <glib-2.0/glib.h>
#include <glib-object.h>

#include "output.h"

G_BEGIN_DECLS

#define XW_TYPE_OUTPUTS xw_outputs_get_type()
G_DECLARE_INTERFACE(XwOutputs, xw_outputs, XW, OUTPUTS, GObject)

struct _XwOutputsInterface {
    GTypeInterface parent;

    void (*new_output)(XwOutputs *self, XwOutput *output);

    GList *(*get_outputs)(XwOutputs *self);
};

GList *xw_outputs_get_outputs(XwOutputs *self);

XwOutputs *xw_outputs_new();

G_END_DECLS

#endif
