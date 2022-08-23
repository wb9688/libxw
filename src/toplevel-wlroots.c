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

#include "toplevel-wlroots.h"

struct _XwToplevelWlroots {
    GObject parent_instance;

    struct zwlr_foreign_toplevel_handle_v1 *toplevel;

    gchar *title;
    gchar *app_id;
    gboolean minimized;
    gboolean maximized;
    gboolean fullscreen;
};

typedef enum {
    PROP_TOPLEVEL = 1,
    N_PROPERTIES,
    PROP_TITLE,
    PROP_APP_ID,
    PROP_MINIMIZED,
    PROP_MAXIMIZED,
    PROP_FULLSCREEN
} XwToplevelWlrootsProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void xw_toplevel_wlroots_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec) {
    XwToplevelWlroots *self = XW_TOPLEVEL_WLROOTS(object);

    switch ((XwToplevelWlrootsProperty) property_id) {
        case PROP_TOPLEVEL:
            self->toplevel = g_value_get_pointer(value);
            break;
        case PROP_TITLE:
            g_free(self->title);
            self->title = g_value_dup_string(value);
            break;
        case PROP_APP_ID:
            g_free(self->app_id);
            self->app_id = g_value_dup_string(value);
            break;
        case PROP_MINIMIZED:
            self->minimized = g_value_get_boolean(value);
            break;
        case PROP_MAXIMIZED:
            self->maximized = g_value_get_boolean(value);
            break;
        case PROP_FULLSCREEN:
            self->fullscreen = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void xw_toplevel_wlroots_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
    XwToplevelWlroots *self = XW_TOPLEVEL_WLROOTS(object);

    switch ((XwToplevelWlrootsProperty) property_id) {
        case PROP_TOPLEVEL:
            g_value_set_pointer(value, self->toplevel);
            break;
        case PROP_TITLE:
            g_value_set_static_string(value, self->title);
            break;
        case PROP_APP_ID:
            g_value_set_static_string(value, self->app_id);
            break;
        case PROP_MINIMIZED:
            g_value_set_boolean(value, self->minimized);
            break;
        case PROP_MAXIMIZED:
            g_value_set_boolean(value, self->maximized);
            break;
        case PROP_FULLSCREEN:
            g_value_set_boolean(value, self->fullscreen);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void xw_toplevel_wlroots_toplevel_interface_init(XwToplevelInterface *iface);
static void xw_toplevel_wlroots_constructed(GObject *gobject);
static void xw_toplevel_wlroots_finalize(GObject *gobject);

static gchar *xw_toplevel_wlroots_get_title(XwToplevel *self);
static gchar *xw_toplevel_wlroots_get_app_id(XwToplevel *self);
static gboolean xw_toplevel_wlroots_get_minimized(XwToplevel *self);
static gboolean xw_toplevel_wlroots_get_maximized(XwToplevel *self);
static gboolean xw_toplevel_wlroots_get_fullscreen(XwToplevel *self);

static void xw_toplevel_wlroots_set_minimized(XwToplevel *self, gboolean minimized);
static void xw_toplevel_wlroots_set_maximized(XwToplevel *self, gboolean maximized);
static void xw_toplevel_wlroots_set_fullscreen(XwToplevel *self, gboolean fullscreen);
static void xw_toplevel_wlroots_close(XwToplevel *self);

G_DEFINE_TYPE_WITH_CODE(XwToplevelWlroots, xw_toplevel_wlroots, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_TOPLEVEL, xw_toplevel_wlroots_toplevel_interface_init))

static void xw_toplevel_wlroots_class_init(XwToplevelWlrootsClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->set_property = xw_toplevel_wlroots_set_property;
    object_class->get_property = xw_toplevel_wlroots_get_property;

    obj_properties[PROP_TOPLEVEL] = g_param_spec_pointer("toplevel", "Toplevel", "Toplevel", G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);

    g_object_class_override_property(object_class, PROP_TITLE, "title");
    g_object_class_override_property(object_class, PROP_APP_ID, "app-id");
    g_object_class_override_property(object_class, PROP_MINIMIZED, "minimized");
    g_object_class_override_property(object_class, PROP_MAXIMIZED, "maximized");
    g_object_class_override_property(object_class, PROP_FULLSCREEN, "fullscreen");

    object_class->constructed = xw_toplevel_wlroots_constructed;
    object_class->finalize = xw_toplevel_wlroots_finalize;
}

static void xw_toplevel_wlroots_toplevel_interface_init(XwToplevelInterface *iface) {
    iface->get_title = xw_toplevel_wlroots_get_title;
    iface->get_app_id = xw_toplevel_wlroots_get_app_id;
    iface->get_minimized = xw_toplevel_wlroots_get_minimized;
    iface->get_maximized = xw_toplevel_wlroots_get_maximized;
    iface->get_fullscreen = xw_toplevel_wlroots_get_fullscreen;

    iface->set_minimized = xw_toplevel_wlroots_set_minimized;
    iface->set_maximized = xw_toplevel_wlroots_set_maximized;
    iface->set_fullscreen = xw_toplevel_wlroots_set_fullscreen;
    iface->close = xw_toplevel_wlroots_close;
}

static void xw_toplevel_wlroots_init(XwToplevelWlroots *self) {}

static void foreign_toplevel_handle_handle_title(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle, const char *title) {
    GValue val = G_VALUE_INIT;
    g_value_init(&val, G_TYPE_STRING);
    g_value_set_static_string(&val, title);

    g_object_set_property(G_OBJECT(data), "title", &val);
}

static void foreign_toplevel_handle_handle_app_id(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle, const char *app_id) {
    GValue val = G_VALUE_INIT;
    g_value_init(&val, G_TYPE_STRING);
    g_value_set_static_string(&val, app_id);

    g_object_set_property(G_OBJECT(data), "app-id", &val);
}

static void foreign_toplevel_handle_handle_output_enter(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle, struct wl_output *output) {}

static void foreign_toplevel_handle_handle_output_leave(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle, struct wl_output *output) {}

static void foreign_toplevel_handle_handle_state(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle, struct wl_array *state) {
    uint32_t flags = 0;
    uint32_t *entry;
    wl_array_for_each(entry, state) {
        flags |= (1 << *entry);
    }

    GValue minimized_val = G_VALUE_INIT;
    g_value_init(&minimized_val, G_TYPE_BOOLEAN);
    g_value_set_boolean(&minimized_val, flags & (1 << ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED));

    g_object_set_property(G_OBJECT(data), "minimized", &minimized_val);

    GValue maximized_val = G_VALUE_INIT;
    g_value_init(&maximized_val, G_TYPE_BOOLEAN);
    g_value_set_boolean(&maximized_val, flags & (1 << ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED));

    g_object_set_property(G_OBJECT(data), "maximized", &maximized_val);

    GValue fullscreen_val = G_VALUE_INIT;
    g_value_init(&fullscreen_val, G_TYPE_BOOLEAN);
    g_value_set_boolean(&fullscreen_val, flags & (1 << ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN));

    g_object_set_property(G_OBJECT(data), "fullscreen", &fullscreen_val);
}

static void foreign_toplevel_handle_handle_done(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle) {}

static void foreign_toplevel_handle_handle_closed(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle) {
    g_signal_emit_by_name(data, "destroy");
}

static void foreign_toplevel_handle_handle_parent(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle, struct zwlr_foreign_toplevel_handle_v1 *parent) {}

static const struct zwlr_foreign_toplevel_handle_v1_listener foreign_toplevel_handle_listener = {
    .title = foreign_toplevel_handle_handle_title,
    .app_id = foreign_toplevel_handle_handle_app_id,
    .output_enter = foreign_toplevel_handle_handle_output_enter,
    .output_leave = foreign_toplevel_handle_handle_output_leave,
    .state = foreign_toplevel_handle_handle_state,
    .done = foreign_toplevel_handle_handle_done,
    .closed = foreign_toplevel_handle_handle_closed,
    .parent = foreign_toplevel_handle_handle_parent
};

static void xw_toplevel_wlroots_constructed(GObject *gobject) {
    XwToplevelWlroots *self = XW_TOPLEVEL_WLROOTS(gobject);

    zwlr_foreign_toplevel_handle_v1_add_listener(self->toplevel, &foreign_toplevel_handle_listener, self);

    G_OBJECT_CLASS(xw_toplevel_wlroots_parent_class)->constructed(gobject);
}

static void xw_toplevel_wlroots_finalize(GObject *gobject) {
    XwToplevelWlroots *self = XW_TOPLEVEL_WLROOTS(gobject);

    g_free(self->title);
    g_free(self->app_id);

    zwlr_foreign_toplevel_handle_v1_destroy(self->toplevel);

    G_OBJECT_CLASS(xw_toplevel_wlroots_parent_class)->finalize(gobject);
}

static gchar *xw_toplevel_wlroots_get_title(XwToplevel *self) {
    return XW_TOPLEVEL_WLROOTS(self)->title;
}

static gchar *xw_toplevel_wlroots_get_app_id(XwToplevel *self) {
    return XW_TOPLEVEL_WLROOTS(self)->app_id;
}

static gboolean xw_toplevel_wlroots_get_minimized(XwToplevel *self) {
    return XW_TOPLEVEL_WLROOTS(self)->minimized;
}

static gboolean xw_toplevel_wlroots_get_maximized(XwToplevel *self) {
    return XW_TOPLEVEL_WLROOTS(self)->maximized;
}

static gboolean xw_toplevel_wlroots_get_fullscreen(XwToplevel *self) {
    return XW_TOPLEVEL_WLROOTS(self)->fullscreen;
}

static void xw_toplevel_wlroots_set_minimized(XwToplevel *self, gboolean minimized) {
    if (minimized)
        zwlr_foreign_toplevel_handle_v1_set_minimized(XW_TOPLEVEL_WLROOTS(self)->toplevel);
    else
        zwlr_foreign_toplevel_handle_v1_unset_minimized(XW_TOPLEVEL_WLROOTS(self)->toplevel);
}

static void xw_toplevel_wlroots_set_maximized(XwToplevel *self, gboolean maximized) {
    if (maximized)
        zwlr_foreign_toplevel_handle_v1_set_maximized(XW_TOPLEVEL_WLROOTS(self)->toplevel);
    else
        zwlr_foreign_toplevel_handle_v1_unset_maximized(XW_TOPLEVEL_WLROOTS(self)->toplevel);
}

static void xw_toplevel_wlroots_set_fullscreen(XwToplevel *self, gboolean fullscreen) {
    if (fullscreen)
        zwlr_foreign_toplevel_handle_v1_set_fullscreen(XW_TOPLEVEL_WLROOTS(self)->toplevel, NULL);
    else
        zwlr_foreign_toplevel_handle_v1_unset_fullscreen(XW_TOPLEVEL_WLROOTS(self)->toplevel);
}

static void xw_toplevel_wlroots_close(XwToplevel *self) {
    zwlr_foreign_toplevel_handle_v1_close(XW_TOPLEVEL_WLROOTS(self)->toplevel);
}

gboolean xw_toplevel_wlroots_is_supported() {
    GdkDisplay *display = gdk_display_get_default();

    return GDK_IS_WAYLAND_DISPLAY(display) && gdk_wayland_display_query_registry(GDK_WAYLAND_DISPLAY(display), zwlr_foreign_toplevel_manager_v1_interface.name);
}

XwToplevelWlroots *xw_toplevel_wlroots_new(struct zwlr_foreign_toplevel_handle_v1 *toplevel) {
    if (xw_toplevel_wlroots_is_supported()) {
        return XW_TOPLEVEL_WLROOTS(g_object_new(XW_TYPE_TOPLEVEL_WLROOTS, "toplevel", toplevel, NULL));
    }

    return NULL;
}
