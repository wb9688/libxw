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

#include "toplevel.h"

G_DEFINE_INTERFACE(XwToplevel, xw_toplevel, G_TYPE_OBJECT)

enum {
    DESTROY,
    N_SIGNALS
};

static guint toplevel_signals[N_SIGNALS];

static void xw_toplevel_default_init(XwToplevelInterface *iface) {
    g_object_interface_install_property(iface, g_param_spec_string("title", "Title", "Title", "", G_PARAM_READWRITE));
    g_object_interface_install_property(iface, g_param_spec_string("app-id", "App ID", "App ID", "", G_PARAM_READWRITE));
    g_object_interface_install_property(iface, g_param_spec_boolean("minimized", "Minimized", "Minimized", FALSE, G_PARAM_READWRITE));
    g_object_interface_install_property(iface, g_param_spec_boolean("maximized", "Maximized", "Maximized", FALSE, G_PARAM_READWRITE));
    g_object_interface_install_property(iface, g_param_spec_boolean("fullscreen", "Fullscreen", "Fullscreen", FALSE, G_PARAM_READWRITE));

    toplevel_signals[DESTROY] = g_signal_new("destroy", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(XwToplevelInterface, destroy), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

gchar *xw_toplevel_get_title(XwToplevel *self) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->get_title != NULL);
    return iface->get_title(self);
}

gchar *xw_toplevel_get_app_id(XwToplevel *self) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->get_app_id != NULL);
    return iface->get_app_id(self);
}

gboolean xw_toplevel_get_minimized(XwToplevel *self) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->get_minimized != NULL);
    return iface->get_minimized(self);
}

gboolean xw_toplevel_get_maximized(XwToplevel *self) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->get_maximized != NULL);
    return iface->get_maximized(self);
}

gboolean xw_toplevel_get_fullscreen(XwToplevel *self) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->get_fullscreen != NULL);
    return iface->get_fullscreen(self);
}

void xw_toplevel_set_minimized(XwToplevel *self, gboolean minimized) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->set_minimized != NULL);
    iface->set_minimized(self, minimized);
}

void xw_toplevel_set_maximized(XwToplevel *self, gboolean maximized) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->set_maximized != NULL);
    iface->set_maximized(self, maximized);
}

void xw_toplevel_set_fullscreen(XwToplevel *self, gboolean fullscreen) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->set_fullscreen != NULL);
    iface->set_fullscreen(self, fullscreen);
}

void xw_toplevel_close(XwToplevel *self) {
    g_return_if_fail(XW_IS_TOPLEVEL(self));

    XwToplevelInterface *iface = XW_TOPLEVEL_GET_IFACE(self);
    g_return_if_fail(iface->close != NULL);
    iface->close(self);
}
