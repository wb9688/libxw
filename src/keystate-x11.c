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

#include <gdk/gdkx.h>

#include <X11/XKBlib.h>

#include "keystate-x11.h"

struct _XwKeystateX11 {
    GObject parent_instance;

    gint xkb_event_base;
    XwKeystateStates *states;
};

static void xw_keystate_x11_keystate_interface_init(XwKeystateInterface *iface);
static void xw_keystate_x11_finalize(GObject *gobject);
static XwKeystateStates *xw_keystate_x11_get_states(XwKeystate *self);

G_DEFINE_TYPE_WITH_CODE(XwKeystateX11, xw_keystate_x11, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_KEYSTATE, xw_keystate_x11_keystate_interface_init))

static void xw_keystate_x11_class_init(XwKeystateX11Class *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = xw_keystate_x11_finalize;
}

static void xw_keystate_x11_keystate_interface_init(XwKeystateInterface *iface) {
    iface->get_states = xw_keystate_x11_get_states;
}

static void xw_keystate_x11_update_state(XwKeystateX11 *self, guint state) {
    gint is_lit0 = state & (1 << 0);
    gint is_lit1 = state & (1 << 1);
    gint is_lit2 = state & (1 << 2);

    self->states->capslock = is_lit0 ? XW_KEYSTATE_STATE_LOCKED : XW_KEYSTATE_STATE_UNLOCKED;
    self->states->numlock = is_lit1 ? XW_KEYSTATE_STATE_LOCKED : XW_KEYSTATE_STATE_UNLOCKED;
    self->states->scrolllock = is_lit2 ? XW_KEYSTATE_STATE_LOCKED : XW_KEYSTATE_STATE_UNLOCKED;

    g_signal_emit_by_name(self, "states-changed", self->states);
}

static GdkFilterReturn xw_keystate_x11_event_filter(GdkXEvent *xevent, GdkEvent *event, XwKeystateX11 *self) {
    XEvent *xev = (XEvent *) xevent;
    if (xev->xany.type == self->xkb_event_base + XkbEventCode) {
        XkbEvent *xkbev = (XkbEvent *) xev;
        if (xkbev->any.xkb_type == XkbIndicatorStateNotify)
            xw_keystate_x11_update_state(self, xkbev->indicators.state);
    }

    return GDK_FILTER_CONTINUE;
}

static void xw_keystate_x11_init(XwKeystateX11 *self) {
    Display *xdisplay = GDK_DISPLAY_XDISPLAY(gdk_display_get_default());

    gint opcode;
    gint maj = XkbMajorVersion;
    gint min = XkbMinorVersion;
    if (!XkbLibraryVersion(&maj, &min))
        g_error("XkbLibraryVersion failed");

    self->xkb_event_base = 0;
    gint xkb_error_base = 0;
    if (!XkbQueryExtension(xdisplay, &opcode, &self->xkb_event_base, &xkb_error_base, &maj, &min))
        g_error("XkbQueryExtension failed");

    self->states = g_slice_new(XwKeystateStates);

    gdk_window_add_filter(NULL, (GdkFilterFunc) xw_keystate_x11_event_filter, self);
    if (!XkbSelectEvents(xdisplay, XkbUseCoreKbd, XkbIndicatorStateNotifyMask, XkbIndicatorStateNotifyMask))
        g_error("XkbSelectEvents failed");

    guint state;
    XkbGetIndicatorState(xdisplay, XkbUseCoreKbd, &state);
    xw_keystate_x11_update_state(self, state);
}

static void xw_keystate_x11_finalize(GObject *gobject) {
    XwKeystateX11 *self = XW_KEYSTATE_X11(gobject);

    g_slice_free(XwKeystateStates, self->states);

    G_OBJECT_CLASS(xw_keystate_x11_parent_class)->finalize(gobject);
}

static XwKeystateStates *xw_keystate_x11_get_states(XwKeystate *self) {
    return XW_KEYSTATE_X11(self)->states;
}

gboolean xw_keystate_x11_is_supported() {
    GdkDisplay *display = gdk_display_get_default();
    gint maj = XkbMajorVersion;
    gint min = XkbMinorVersion;
    return GDK_IS_X11_DISPLAY(display) && XkbLibraryVersion(&maj, &min);
}
