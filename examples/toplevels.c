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

#include <gtk/gtk.h>

#include "toplevels.h"
#include "toplevel.h"

XwToplevels *xw_toplevels;
GtkWidget *box;

static void notify_title_event(XwToplevel *toplevel, GParamSpec *pspec, GtkLabel *label) {
    gtk_label_set_text(label, xw_toplevel_get_title(toplevel));
}

static void destroy_event(XwToplevel *toplevel, GtkWidget *widget) {
    gtk_widget_destroy(widget);
}

static void new_toplevel_event(XwToplevels *xw_toplevels, XwToplevel *toplevel) {
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    GtkWidget *label = gtk_label_new(xw_toplevel_get_title(toplevel));
    gtk_container_add(GTK_CONTAINER(hbox), label);

    gtk_container_add(GTK_CONTAINER(box), hbox);

    gtk_widget_show_all(hbox);

    g_signal_connect(toplevel, "notify::title", G_CALLBACK(notify_title_event), label);

    g_signal_connect(toplevel, "destroy", G_CALLBACK(destroy_event), hbox);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "libxw toplevels example");

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), box);

    if (!xw_toplevels) {
        GtkWidget *label = gtk_label_new("toplevels is not supported in your environment");
        gtk_container_add(GTK_CONTAINER(box), label);
    } else {
        GList *toplevels = xw_toplevels_get_toplevels(xw_toplevels);
        for (GList *toplevel = toplevels; toplevel; toplevel = toplevel->next) {
            XwToplevel *data = toplevel->data;

            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

            GtkWidget *label = gtk_label_new(xw_toplevel_get_title(data));
            gtk_container_add(GTK_CONTAINER(hbox), label);

            gtk_container_add(GTK_CONTAINER(box), hbox);

            g_signal_connect(data, "notify::title", G_CALLBACK(notify_title_event), label);

            g_signal_connect(data, "destroy", G_CALLBACK(destroy_event), hbox);
        }
    }

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    xw_toplevels = xw_toplevels_new();
    if (xw_toplevels)
        g_signal_connect(xw_toplevels, "new-toplevel", G_CALLBACK(new_toplevel_event), NULL);

    GtkApplication *app = gtk_application_new("org.libxw.examples.toplevels", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (xw_toplevels)
        g_object_unref(xw_toplevels);

    return status;
}
