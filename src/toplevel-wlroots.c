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

#include "toplevel-wlroots.h"

struct _XwToplevelWlroots {
    GObject parent_instance;

    struct zwlr_foreign_toplevel_handle_v1 *toplevel;

    gchar *title;
    gchar *app_id;
};

typedef enum {
    PROP_TOPLEVEL = 1,
    N_PROPERTIES,
    PROP_TITLE,
    PROP_APP_ID
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

G_DEFINE_TYPE_WITH_CODE(XwToplevelWlroots, xw_toplevel_wlroots, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_TOPLEVEL, xw_toplevel_wlroots_toplevel_interface_init))

static void xw_toplevel_wlroots_class_init(XwToplevelWlrootsClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->set_property = xw_toplevel_wlroots_set_property;
    object_class->get_property = xw_toplevel_wlroots_get_property;

    obj_properties[PROP_TOPLEVEL] = g_param_spec_pointer("toplevel", "Toplevel", "Toplevel", G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);

    g_object_class_override_property(object_class, PROP_TITLE, "title");
    g_object_class_override_property(object_class, PROP_APP_ID, "app-id");

    object_class->constructed = xw_toplevel_wlroots_constructed;
    object_class->finalize = xw_toplevel_wlroots_finalize;
}

static void xw_toplevel_wlroots_toplevel_interface_init(XwToplevelInterface *iface) {
    iface->get_title = xw_toplevel_wlroots_get_title;
    iface->get_app_id = xw_toplevel_wlroots_get_app_id;
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

static void foreign_toplevel_handle_handle_state(void *data, struct zwlr_foreign_toplevel_handle_v1 *foreign_toplevel_handle, struct wl_array *state) {}

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

gboolean xw_toplevel_wlroots_is_supported() {
    return xw_config_has_wl_registry_global(zwlr_foreign_toplevel_manager_v1_interface.name);
}

XwToplevelWlroots *xw_toplevel_wlroots_new(struct zwlr_foreign_toplevel_handle_v1 *toplevel) {
    if (xw_toplevel_wlroots_is_supported()) {
        return XW_TOPLEVEL_WLROOTS(g_object_new(XW_TYPE_TOPLEVEL_WLROOTS, "toplevel", toplevel, NULL));
    }

    return NULL;
}
