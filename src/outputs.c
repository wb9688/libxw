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

#include "outputs.h"
#include "outputs-wlroots.h"

G_DEFINE_INTERFACE(XwOutputs, xw_outputs, G_TYPE_OBJECT)

enum {
    NEW_OUTPUT,
    N_SIGNALS
};

static guint outputs_signals[N_SIGNALS];

static void xw_outputs_default_init(XwOutputsInterface *iface) {
    outputs_signals[NEW_OUTPUT] = g_signal_new("new-output", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(XwOutputsInterface, new_output), NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

GList *xw_outputs_get_outputs(XwOutputs *self) {
    g_return_if_fail(XW_IS_OUTPUTS(self));

    XwOutputsInterface *iface = XW_OUTPUTS_GET_IFACE(self);
    g_return_if_fail(iface->get_outputs != NULL);
    iface->get_outputs(self);
}

XwOutputs *xw_outputs_new() {
    if (xw_outputs_wlroots_is_supported()) {
        return XW_OUTPUTS(g_object_new(XW_TYPE_OUTPUTS_WLROOTS, NULL));
    }

    return NULL;
}
