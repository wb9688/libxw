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

#include "keystate.h"
#include "keystate-kde.h"
#include "keystate-x11.h"

G_DEFINE_INTERFACE(XwKeystate, xw_keystate, G_TYPE_OBJECT)

enum {
    STATES_CHANGED,
    N_SIGNALS
};

static guint keystate_signals[N_SIGNALS];

static void xw_keystate_default_init(XwKeystateInterface *iface) {
    keystate_signals[STATES_CHANGED] = g_signal_new("states-changed", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(XwKeystateInterface, states_changed), NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);
}

XwKeystateStates *xw_keystate_get_states(XwKeystate *self) {
    g_return_if_fail(XW_IS_KEYSTATE(self));

    XwKeystateInterface *iface = XW_KEYSTATE_GET_IFACE(self);
    g_return_if_fail(iface->get_states != NULL);
    return iface->get_states(self);
}

XwKeystate *xw_keystate_new() {
    if (xw_keystate_kde_is_supported()) {
        return XW_KEYSTATE(g_object_new(XW_TYPE_KEYSTATE_KDE, NULL));
    } else if (xw_keystate_x11_is_supported()) {
        return XW_KEYSTATE(g_object_new(XW_TYPE_KEYSTATE_X11, NULL));
    }

    return NULL;
}
