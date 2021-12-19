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

#ifndef __SCREENSHOT_WLROOTS_H__
#define __SCREENSHOT_WLROOTS_H__

#include "screenshot.h"

G_BEGIN_DECLS

#define XW_TYPE_SCREENSHOT_WLROOTS xw_screenshot_wlroots_get_type()
G_DECLARE_FINAL_TYPE(XwScreenshotWlroots, xw_screenshot_wlroots, XW, SCREENSHOT_WLROOTS, GObject)

gboolean xw_screenshot_wlroots_is_supported();

G_END_DECLS

#endif
