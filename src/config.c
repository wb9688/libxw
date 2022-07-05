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

#include "config.h"

static gboolean (*xw_config_has_wl_registry_global_override)(const char *global) = NULL;
static struct wl_display *(*xw_config_get_wl_display_override)() = NULL;

void xw_config_override_has_wl_registry_global(void *override_function) {
    xw_config_has_wl_registry_global_override = override_function;
}

gboolean xw_config_has_wl_registry_global(const char* global) {
    if (xw_config_has_wl_registry_global_override)
        return xw_config_has_wl_registry_global_override(global);

    GdkDisplay *display = gdk_display_get_default();
    return GDK_IS_WAYLAND_DISPLAY(display) && gdk_wayland_display_query_registry(GDK_WAYLAND_DISPLAY(display), global);
}

void xw_config_override_get_wl_display(void *override_function) {
    xw_config_get_wl_display_override = override_function;
}

struct wl_display *xw_config_get_wl_display() {
    if (xw_config_get_wl_display_override)
        return xw_config_get_wl_display_override();

    GdkDisplay *gdk_display = gdk_display_get_default();
    return gdk_wayland_display_get_wl_display(GDK_WAYLAND_DISPLAY(gdk_display));
}
