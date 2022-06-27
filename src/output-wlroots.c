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

#include <gdk/gdkwayland.h>

#include "output-wlroots.h"

struct _XwOutputWlroots {
    GObject parent_instance;

    struct zwlr_output_head_v1 *head;

    GList *modes;

    gchar *name;
};

typedef enum {
    PROP_HEAD = 1,
    PROP_NAME,
    N_PROPERTIES
} XwOutputWlrootsProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void xw_output_wlroots_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec) {
    XwOutputWlroots *self = XW_OUTPUT_WLROOTS(object);

    switch ((XwOutputWlrootsProperty) property_id) {
        case PROP_HEAD:
            self->head = g_value_get_pointer(value);
            break;
        case PROP_NAME:
            g_free(self->name);
            self->name = g_value_dup_string(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void xw_output_wlroots_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
    XwOutputWlroots *self = XW_OUTPUT_WLROOTS(object);

    switch ((XwOutputWlrootsProperty) property_id) {
        case PROP_HEAD:
            g_value_set_pointer(value, self->head);
            break;
        case PROP_NAME:
            g_value_set_static_string(value, self->name);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void xw_output_wlroots_output_interface_init(XwOutputInterface *iface);
static void xw_output_wlroots_constructed(GObject *gobject);
static void xw_output_wlroots_finalize(GObject *gobject);

G_DEFINE_TYPE_WITH_CODE(XwOutputWlroots, xw_output_wlroots, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_OUTPUT, xw_output_wlroots_output_interface_init))

static void xw_output_wlroots_class_init(XwOutputWlrootsClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->set_property = xw_output_wlroots_set_property;
    object_class->get_property = xw_output_wlroots_get_property;

    obj_properties[PROP_HEAD] = g_param_spec_pointer("head", "Head", "Head", G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES - 1, obj_properties); // TODO: Do this properly instead of - 1

    g_object_class_override_property(object_class, PROP_NAME, "name");

    object_class->constructed = xw_output_wlroots_constructed;
    object_class->finalize = xw_output_wlroots_finalize;
}

static void xw_output_wlroots_output_interface_init(XwOutputInterface *iface) {
}

static void output_mode_handle_size(void *data, struct zwlr_output_mode_v1 *output_mode, int32_t width, int32_t height) {}

static void output_mode_handle_refresh(void *data, struct zwlr_output_mode_v1 *output_mode, int32_t refresh) {}

static void output_mode_handle_preferred(void *data, struct zwlr_output_mode_v1 *output_mode) {}

static void output_mode_handle_finished(void *data, struct zwlr_output_mode_v1 *output_mode) {
    XwOutputWlroots *self = XW_OUTPUT_WLROOTS(data);

    self->modes = g_list_remove(self->modes, output_mode);

    zwlr_output_mode_v1_destroy(output_mode);
}

static const struct zwlr_output_mode_v1_listener output_mode_listener = {
    .size = output_mode_handle_size,
    .refresh = output_mode_handle_refresh,
    .preferred = output_mode_handle_preferred,
    .finished = output_mode_handle_finished,
};

static void output_head_handle_name(void *data, struct zwlr_output_head_v1 *output_head, const char *name) {
    GValue val = G_VALUE_INIT;
    g_value_init(&val, G_TYPE_STRING);
    g_value_set_static_string(&val, name);

    g_object_set_property(G_OBJECT(data), "name", &val);

    //g_critical("name: %s", name);
}

static void output_head_handle_description(void *data, struct zwlr_output_head_v1 *output_head, const char *description) {}

static void output_head_handle_physical_size(void *data, struct zwlr_output_head_v1 *output_head, int32_t width, int32_t height) {}

static void output_head_handle_mode(void *data, struct zwlr_output_head_v1 *output_head, struct zwlr_output_mode_v1 *mode) {
    XwOutputWlroots *self = XW_OUTPUT_WLROOTS(data);

    zwlr_output_mode_v1_add_listener(mode, &output_mode_listener, self);

    self->modes = g_list_append(self->modes, mode);
}

static void output_head_handle_enabled(void *data, struct zwlr_output_head_v1 *output_head, int32_t enabled) {}

static void output_head_handle_current_mode(void *data, struct zwlr_output_head_v1 *output_head, struct zwlr_output_mode_v1 *mode) {}

static void output_head_handle_position(void *data, struct zwlr_output_head_v1 *output_head, int32_t x, gint32 y) {}

static void output_head_handle_transform(void *data, struct zwlr_output_head_v1 *output_head, int32_t transform) {}

static void output_head_handle_scale(void *data, struct zwlr_output_head_v1 *output_head, wl_fixed_t scale) {}

static void output_head_handle_finished(void *data, struct zwlr_output_head_v1 *output_head) {
    g_signal_emit_by_name(data, "destroy");
}

static void output_head_handle_make(void *data, struct zwlr_output_head_v1 *output_head, const char *make) {}

static void output_head_handle_model(void *data, struct zwlr_output_head_v1 *output_head, const char *model) {}

static void output_head_handle_serial_number(void *data, struct zwlr_output_head_v1 *output_head, const char *serial_number) {}

static const struct zwlr_output_head_v1_listener output_head_listener = {
    .name = output_head_handle_name,
    .description = output_head_handle_description,
    .physical_size = output_head_handle_physical_size,
    .mode = output_head_handle_mode,
    .enabled = output_head_handle_enabled,
    .current_mode = output_head_handle_current_mode,
    .position = output_head_handle_position,
    .transform = output_head_handle_transform,
    .scale = output_head_handle_scale,
    .finished = output_head_handle_finished,
    .make = output_head_handle_make,
    .model = output_head_handle_model,
    .serial_number = output_head_handle_serial_number
};

static void xw_output_wlroots_init(XwOutputWlroots *self) {
}

static void xw_output_wlroots_constructed(GObject *gobject) {
    XwOutputWlroots *self = XW_OUTPUT_WLROOTS(gobject);

    zwlr_output_head_v1_add_listener(self->head, &output_head_listener, self);

    G_OBJECT_CLASS(xw_output_wlroots_parent_class)->constructed(gobject);
}

static void xw_output_wlroots_finalize(GObject *gobject) {
    XwOutputWlroots *self = XW_OUTPUT_WLROOTS(gobject);

    g_free(self->name);

    g_list_free_full(self->modes, (GDestroyNotify) zwlr_output_mode_v1_destroy);
    zwlr_output_head_v1_destroy(self->head);

    //g_critical("xw_output_wlroots_finalize");

    G_OBJECT_CLASS(xw_output_wlroots_parent_class)->finalize(gobject);
}

gboolean xw_output_wlroots_is_supported() {
    GdkDisplay *display = gdk_display_get_default();

    return GDK_IS_WAYLAND_DISPLAY(display) && gdk_wayland_display_query_registry(GDK_WAYLAND_DISPLAY(display), zwlr_output_manager_v1_interface.name);
}

XwOutputWlroots *xw_output_wlroots_new(struct zwlr_output_head_v1 *head) {
    if (xw_output_wlroots_is_supported()) {
        XwOutputWlroots *self = XW_OUTPUT_WLROOTS(g_object_new(XW_TYPE_OUTPUT_WLROOTS, "head", head, NULL));
        return self;
    }

    return NULL;
}
