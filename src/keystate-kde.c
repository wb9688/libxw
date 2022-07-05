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

#include <wayland-client-protocol.h>
#include "keystate-client-protocol.h"

#include "config.h"
#include "keystate-kde.h"

struct _XwKeystateKde {
    GObject parent_instance;

    struct org_kde_kwin_keystate *keystate_interface;
    XwKeystateStates *states;
};

static void xw_keystate_kde_keystate_interface_init(XwKeystateInterface *iface);
static void xw_keystate_kde_finalize(GObject *gobject);
static XwKeystateStates *xw_keystate_kde_get_states(XwKeystate *self);

G_DEFINE_TYPE_WITH_CODE(XwKeystateKde, xw_keystate_kde, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_KEYSTATE, xw_keystate_kde_keystate_interface_init))

static void xw_keystate_kde_class_init(XwKeystateKdeClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = xw_keystate_kde_finalize;
}

static void xw_keystate_kde_keystate_interface_init(XwKeystateInterface *iface) {
    iface->get_states = xw_keystate_kde_get_states;
}

static void handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    XwKeystateKde *self = XW_KEYSTATE_KDE(data);

    if (strcmp(interface, org_kde_kwin_keystate_interface.name) == 0)
        self->keystate_interface = wl_registry_bind(registry, name, &org_kde_kwin_keystate_interface, 3);
}

static void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove
};

static void state_changed(void *data, struct org_kde_kwin_keystate *org_kde_kwin_keystate, uint32_t key, uint32_t state) {
    XwKeystateKde *self = XW_KEYSTATE_KDE(data);

    if (key == 0)
        self->states->capslock = state;
    else if (key == 1)
        self->states->numlock = state;
    else if (key == 2)
        self->states->scrolllock = state;

    g_signal_emit_by_name(self, "states-changed", self->states);
}

static const struct org_kde_kwin_keystate_listener state_changed_listener = {
    .stateChanged = state_changed
};

static void xw_keystate_kde_init(XwKeystateKde *self) {
    struct wl_display *display = xw_config_get_wl_display();

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, self);
    wl_display_roundtrip(display);

    self->states = g_slice_new(XwKeystateStates);
    self->states->capslock = XW_KEYSTATE_STATE_UNLOCKED;
    self->states->numlock = XW_KEYSTATE_STATE_UNLOCKED;
    self->states->scrolllock = XW_KEYSTATE_STATE_UNLOCKED;

    org_kde_kwin_keystate_add_listener(self->keystate_interface, &state_changed_listener, self);
    org_kde_kwin_keystate_fetchStates(self->keystate_interface);
}

static void xw_keystate_kde_finalize(GObject *gobject) {
    XwKeystateKde *self = XW_KEYSTATE_KDE(gobject);

    org_kde_kwin_keystate_destroy(self->keystate_interface);

    g_slice_free(XwKeystateStates, self->states);

    G_OBJECT_CLASS(xw_keystate_kde_parent_class)->finalize(gobject);
}

static XwKeystateStates *xw_keystate_kde_get_states(XwKeystate *self) {
    return XW_KEYSTATE_KDE(self)->states;
}

gboolean xw_keystate_kde_is_supported() {
    return xw_config_has_wl_registry_global(org_kde_kwin_keystate_interface.name);
}
