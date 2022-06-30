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

#include "screenshot.h"
#include "screenshot-wlroots.h"

G_DEFINE_INTERFACE(XwScreenshot, xw_screenshot, G_TYPE_OBJECT)

static void xw_screenshot_default_init(XwScreenshotInterface *iface) {}

GdkPixbuf *xw_screenshot_take(XwScreenshot *self, GdkMonitor *monitor) {
    g_return_if_fail(XW_IS_SCREENSHOT(self));

    XwScreenshotInterface *iface = XW_SCREENSHOT_GET_IFACE(self);
    g_return_if_fail(iface->take != NULL);
    return iface->take(self, monitor);
}

XwScreenshot *xw_screenshot_new() {
    if (xw_screenshot_wlroots_is_supported()) {
        return XW_SCREENSHOT(g_object_new(XW_TYPE_SCREENSHOT_WLROOTS, NULL));
    }

    return NULL;
}
