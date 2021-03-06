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

#ifndef __OUTPUTS_WLROOTS_H__
#define __OUTPUTS_WLROOTS_H__

#include "wlr-output-management-unstable-v1-client-protocol.h"

#include "outputs.h"

G_BEGIN_DECLS

#define XW_TYPE_OUTPUTS_WLROOTS xw_outputs_wlroots_get_type()
G_DECLARE_FINAL_TYPE(XwOutputsWlroots, xw_outputs_wlroots, XW, OUTPUTS_WLROOTS, GObject)

void xw_outputs_wlroots_apply_changes(XwOutputsWlroots *self, struct zwlr_output_head_v1 *head, gboolean enabled);

gboolean xw_outputs_wlroots_is_supported();

G_END_DECLS

#endif
