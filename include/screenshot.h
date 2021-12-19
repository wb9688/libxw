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

#ifndef __SCREENSHOT_H__
#define __SCREENSHOT_H__

#include <glib-2.0/glib.h>
#include <glib-object.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#define XW_TYPE_SCREENSHOT xw_screenshot_get_type()
G_DECLARE_INTERFACE(XwScreenshot, xw_screenshot, XW, SCREENSHOT, GObject)

struct _XwScreenshotInterface {
    GTypeInterface parent;

    GdkPixbuf *(*take)(XwScreenshot *self, GdkMonitor *monitor);
};

GdkPixbuf *xw_screenshot_take(XwScreenshot *self, GdkMonitor *monitor);

XwScreenshot *xw_screenshot_new();

G_END_DECLS

#endif
