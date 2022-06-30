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

#ifndef __KEYSTATE_H__
#define __KEYSTATE_H__

#include <glib-2.0/glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef enum {
    XW_KEYSTATE_STATE_UNLOCKED,
    XW_KEYSTATE_STATE_LATCHED,
    XW_KEYSTATE_STATE_LOCKED
} xw_keystate_state;

typedef struct {
    xw_keystate_state capslock;
    xw_keystate_state numlock;
    xw_keystate_state scrolllock;
} XwKeystateStates;

#define XW_TYPE_KEYSTATE xw_keystate_get_type()
G_DECLARE_INTERFACE(XwKeystate, xw_keystate, XW, KEYSTATE, GObject)

struct _XwKeystateInterface {
    GTypeInterface parent;

    void (*states_changed)(XwKeystate *self, XwKeystateStates *states);

    XwKeystateStates *(*get_states)(XwKeystate *self);
};

XwKeystateStates *xw_keystate_get_states(XwKeystate *self);

XwKeystate *xw_keystate_new();

G_END_DECLS

#endif
