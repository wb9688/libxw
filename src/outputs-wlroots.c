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

#include <wayland-client-protocol.h>

#include "config.h"
#include "outputs-wlroots.h"
#include "output-wlroots.h"

struct _XwOutputsWlroots {
    GObject parent_instance;

    struct zwlr_output_manager_v1 *output_manager;

    guint32 serial;

    GList *outputs;
};

static void xw_outputs_wlroots_outputs_interface_init(XwOutputsInterface *iface);
static void xw_outputs_wlroots_finalize(GObject *gobject);
static GList *xw_outputs_wlroots_get_outputs(XwOutputs *self);

G_DEFINE_TYPE_WITH_CODE(XwOutputsWlroots, xw_outputs_wlroots, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_OUTPUTS, xw_outputs_wlroots_outputs_interface_init))

static void xw_outputs_wlroots_class_init(XwOutputsWlrootsClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = xw_outputs_wlroots_finalize;
}

static void xw_outputs_wlroots_outputs_interface_init(XwOutputsInterface *iface) {
    iface->get_outputs = xw_outputs_wlroots_get_outputs;
}

static void registry_handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    XwOutputsWlroots *self = XW_OUTPUTS_WLROOTS(data);

    if (strcmp(interface, zwlr_output_manager_v1_interface.name) == 0)
        self->output_manager = wl_registry_bind(registry, name, &zwlr_output_manager_v1_interface, 2);
}

static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove
};

static void destroy_event(XwOutput *output, XwOutputsWlroots *self) {
    self->outputs = g_list_remove(self->outputs, output);
    g_object_unref(output);
}

static void output_manager_handle_head(void *data, struct zwlr_output_manager_v1 *output_manager, struct zwlr_output_head_v1 *head) {
    XwOutputsWlroots *self = XW_OUTPUTS_WLROOTS(data);

    XwOutputWlroots *xw_output = xw_output_wlroots_new(self, head);
    self->outputs = g_list_append(self->outputs, xw_output);

    g_signal_emit_by_name(self, "new-output", XW_OUTPUT(xw_output));

    g_signal_connect(xw_output, "destroy", G_CALLBACK(destroy_event), self);
}

static void output_manager_handle_done(void *data, struct zwlr_output_manager_v1 *output_manager, uint32_t serial) {
    XwOutputsWlroots *self = XW_OUTPUTS_WLROOTS(data);

    self->serial = serial;
}

static void output_manager_handle_finished(void *data, struct zwlr_output_manager_v1 *output_manager) {}

static const struct zwlr_output_manager_v1_listener output_manager_listener = {
    .head = output_manager_handle_head,
    .done = output_manager_handle_done,
    .finished = output_manager_handle_finished
};

static void xw_outputs_wlroots_init(XwOutputsWlroots *self) {
    struct wl_display *display = xw_config_get_wl_display();

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, self);
    wl_display_roundtrip(display);

    zwlr_output_manager_v1_add_listener(self->output_manager, &output_manager_listener, self);
    wl_display_roundtrip(display);
}

static void xw_outputs_wlroots_finalize(GObject *gobject) {
    XwOutputsWlroots *self = XW_OUTPUTS_WLROOTS(gobject);

    g_list_free_full(self->outputs, g_object_unref);

    zwlr_output_manager_v1_destroy(self->output_manager);

    G_OBJECT_CLASS(xw_outputs_wlroots_parent_class)->finalize(gobject);
}

static GList *xw_outputs_wlroots_get_outputs(XwOutputs *self) {
    return XW_OUTPUTS_WLROOTS(self)->outputs;
}

static void output_configuration_handle(void *data, struct zwlr_output_configuration_v1 *output_configuration) {
    zwlr_output_configuration_v1_destroy(output_configuration);
}

static const struct zwlr_output_configuration_v1_listener output_configuration_listener = {
    .succeeded = output_configuration_handle,
    .failed = output_configuration_handle,
    .cancelled = output_configuration_handle
};

void xw_outputs_wlroots_apply_changes(XwOutputsWlroots *self, struct zwlr_output_head_v1 *head, gboolean enabled) {
    struct zwlr_output_configuration_v1 *output_configuration = zwlr_output_manager_v1_create_configuration(self->output_manager, self->serial);

    zwlr_output_configuration_v1_add_listener(output_configuration, &output_configuration_listener, NULL);

    for (GList *output = self->outputs; output; output = output->next) {
        GValue value = G_VALUE_INIT;
        g_object_get_property(G_OBJECT(output->data), "head", &value);
        struct zwlr_output_head_v1 *output_head = g_value_get_pointer(&value);

        if (head == output_head ? enabled : xw_output_get_enabled(XW_OUTPUT(output->data)))
            zwlr_output_configuration_v1_enable_head(output_configuration, output_head);
        else
            zwlr_output_configuration_v1_disable_head(output_configuration, output_head);
    }

    zwlr_output_configuration_v1_apply(output_configuration);
}

gboolean xw_outputs_wlroots_is_supported() {
    return xw_config_has_wl_registry_global(zwlr_output_manager_v1_interface.name);
}
