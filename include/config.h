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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <glib-2.0/glib.h>

#include <wayland-client-protocol.h>

void xw_config_override_has_wl_registry_global(void *override_function);
gboolean xw_config_has_wl_registry_global(const char* global);

void xw_config_override_get_wl_display(void *override_function);
struct wl_display *xw_config_get_wl_display();

#endif
