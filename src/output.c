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

#include "output.h"
#include "output-wlroots.h"

G_DEFINE_INTERFACE(XwOutput, xw_output, G_TYPE_OBJECT)

enum {
    DESTROY,
    N_SIGNALS
};

static guint output_signals[N_SIGNALS];

static void xw_output_default_init(XwOutputInterface *iface) {
    g_object_interface_install_property(iface, g_param_spec_string("name", "Name", "Name", "", G_PARAM_READWRITE));

    output_signals[DESTROY] = g_signal_new("destroy", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET(XwOutputInterface, destroy), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
