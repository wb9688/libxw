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

#include "toplevels.h"
#include "toplevels-wlroots.h"

G_DEFINE_INTERFACE(XwToplevels, xw_toplevels, G_TYPE_OBJECT)

enum {
    NEW_TOPLEVEL,
    N_SIGNALS
};

static guint toplevels_signals[N_SIGNALS];

static void xw_toplevels_default_init(XwToplevelsInterface *iface) {
    toplevels_signals[NEW_TOPLEVEL] = g_signal_new("new-toplevel", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(XwToplevelsInterface, new_toplevel), NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

GList *xw_toplevels_get_toplevels(XwToplevels *self) {
    g_return_if_fail(XW_IS_TOPLEVELS(self));

    XwToplevelsInterface *iface = XW_TOPLEVELS_GET_IFACE(self);
    g_return_if_fail(iface->get_toplevels != NULL);
    return iface->get_toplevels(self);
}

XwToplevels *xw_toplevels_new() {
    if (xw_toplevels_wlroots_is_supported()) {
        return XW_TOPLEVELS(g_object_new(XW_TYPE_TOPLEVELS_WLROOTS, NULL));
    }

    return NULL;
}
