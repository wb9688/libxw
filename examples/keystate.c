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

#include <gtk/gtk.h>

#include "keystate.h"

XwKeystate *xw_keystate;

GtkWidget *label;

static void states_changed_event(XwKeystate *xw_keystate, XwKeystateStates *states) {
    gchar label_string[42];
    sprintf(label_string, "Caps Lock: %d, Num Lock: %d, Scroll Lock: %d", states->capslock, states->numlock, states->scrolllock);
    gtk_label_set_label(GTK_LABEL(label), label_string);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "libxw keystate example");

    label = gtk_label_new("Waiting for keystate…");
    gtk_container_add(GTK_CONTAINER(window), label);

    if (xw_keystate) {
        XwKeystateStates *states = xw_keystate_get_states(xw_keystate);
        states_changed_event(xw_keystate, states);
    } else
        gtk_label_set_label(GTK_LABEL(label), "keystate is not supported in your environment");

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    xw_keystate = xw_keystate_new();
    if (xw_keystate)
        g_signal_connect(xw_keystate, "states-changed", G_CALLBACK(states_changed_event), NULL);

    GtkApplication *app = gtk_application_new("org.libxw.examples.keystate", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (xw_keystate)
        g_object_unref(xw_keystate);

    return status;
}
