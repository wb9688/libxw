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

#include <gdk/gdkwayland.h>

#include <wayland-client-protocol.h>

#include "toplevels-wlroots.h"
#include "toplevel-wlroots.h"

struct _XwToplevelsWlroots {
    GObject parent_instance;

    struct zwlr_foreign_toplevel_manager_v1 *foreign_toplevel_manager;

    GList *toplevels;
};

static void xw_toplevels_wlroots_toplevels_interface_init(XwToplevelsInterface *iface);
static void xw_toplevels_wlroots_finalize(GObject *gobject);
static GList *xw_toplevels_wlroots_get_toplevels(XwToplevels *self);

G_DEFINE_TYPE_WITH_CODE(XwToplevelsWlroots, xw_toplevels_wlroots, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_TOPLEVELS, xw_toplevels_wlroots_toplevels_interface_init))

static void xw_toplevels_wlroots_class_init(XwToplevelsWlrootsClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = xw_toplevels_wlroots_finalize;
}

static void xw_toplevels_wlroots_toplevels_interface_init(XwToplevelsInterface *iface) {
    iface->get_toplevels = xw_toplevels_wlroots_get_toplevels;
}

static void registry_handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    XwToplevelsWlroots *self = XW_TOPLEVELS_WLROOTS(data);

    if (strcmp(interface, zwlr_foreign_toplevel_manager_v1_interface.name) == 0)
        self->foreign_toplevel_manager = wl_registry_bind(registry, name, &zwlr_foreign_toplevel_manager_v1_interface, 3);
}

static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove
};

static void destroy_event(XwToplevel *toplevel, XwToplevelsWlroots *self) {
    self->toplevels = g_list_remove(self->toplevels, toplevel);
    g_object_unref(toplevel);
}

static void foreign_toplevel_manager_handle_toplevel(void *data, struct zwlr_foreign_toplevel_manager_v1 *foreign_toplevel_manager, struct zwlr_foreign_toplevel_handle_v1 *toplevel) {
    XwToplevelsWlroots *self = XW_TOPLEVELS_WLROOTS(data);

    XwToplevelWlroots *xw_toplevel = xw_toplevel_wlroots_new(toplevel);
    self->toplevels = g_list_append(self->toplevels, xw_toplevel);

    g_signal_emit_by_name(self, "new-toplevel", XW_TOPLEVEL(xw_toplevel));

    g_signal_connect(xw_toplevel, "destroy", G_CALLBACK(destroy_event), self);
}

static void foreign_toplevel_manager_handle_finished(void *data, struct zwlr_foreign_toplevel_manager_v1 *foreign_toplevel_manager) {}

static const struct zwlr_foreign_toplevel_manager_v1_listener foreign_toplevel_manager_listener = {
    .toplevel = foreign_toplevel_manager_handle_toplevel,
    .finished = foreign_toplevel_manager_handle_finished
};

static void xw_toplevels_wlroots_init(XwToplevelsWlroots *self) {
    GdkDisplay *gdk_display = gdk_display_get_default();
    struct wl_display *display = gdk_wayland_display_get_wl_display(GDK_WAYLAND_DISPLAY(gdk_display));

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, self);
    wl_display_roundtrip(display);

    zwlr_foreign_toplevel_manager_v1_add_listener(self->foreign_toplevel_manager, &foreign_toplevel_manager_listener, self);
    wl_display_roundtrip(display);
}

static void xw_toplevels_wlroots_finalize(GObject *gobject) {
    XwToplevelsWlroots *self = XW_TOPLEVELS_WLROOTS(gobject);

    g_list_free_full(self->toplevels, g_object_unref);

    zwlr_foreign_toplevel_manager_v1_destroy(self->foreign_toplevel_manager);

    G_OBJECT_CLASS(xw_toplevels_wlroots_parent_class)->finalize(gobject);
}

static GList *xw_toplevels_wlroots_get_toplevels(XwToplevels *self) {
    return XW_TOPLEVELS_WLROOTS(self)->toplevels;
}

gboolean xw_toplevels_wlroots_is_supported() {
    GdkDisplay *display = gdk_display_get_default();

    return GDK_IS_WAYLAND_DISPLAY(display) && gdk_wayland_display_query_registry(GDK_WAYLAND_DISPLAY(display), zwlr_foreign_toplevel_manager_v1_interface.name);
}
