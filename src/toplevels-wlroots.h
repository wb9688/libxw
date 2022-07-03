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

#ifndef __TOPLEVELS_WLROOTS_H__
#define __TOPLEVELS_WLROOTS_H__

#include "wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

#include "toplevels.h"

G_BEGIN_DECLS

#define XW_TYPE_TOPLEVELS_WLROOTS xw_toplevels_wlroots_get_type()
G_DECLARE_FINAL_TYPE(XwToplevelsWlroots, xw_toplevels_wlroots, XW, TOPLEVELS_WLROOTS, GObject)

gboolean xw_toplevels_wlroots_is_supported();

G_END_DECLS

#endif
