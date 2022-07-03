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

#include <gtk/gtk.h>

#include "outputs.h"
#include "output.h"

XwOutputs *xw_outputs;
GtkWidget *box;

static void notify_description_event(XwOutput *output, GParamSpec *pspec, GtkLabel *label) {
    GValue val = G_VALUE_INIT;
    g_object_get_property(G_OBJECT(output), "description", &val);

    gtk_label_set_text(label, g_value_get_string(&val));
}

static void destroy_event(XwOutput *output, GtkWidget *widget) {
    gtk_widget_destroy(widget);
}

static void toggled_event(GtkToggleButton *check_button, XwOutput *output) {
    xw_output_set_enabled(output, gtk_toggle_button_get_active(check_button));
}

static void new_output_event(XwOutputs *xw_outputs, XwOutput *output) {
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    GtkWidget *check_button = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), xw_output_get_enabled(output));
    g_signal_connect(check_button, "toggled", G_CALLBACK(toggled_event), output);
    gtk_container_add(GTK_CONTAINER(hbox), check_button);

    GtkWidget *label = gtk_label_new(xw_output_get_description(output));
    gtk_container_add(GTK_CONTAINER(hbox), label);

    gtk_container_add(GTK_CONTAINER(box), hbox);

    gtk_widget_show_all(hbox);

    g_signal_connect(output, "notify::description", G_CALLBACK(notify_description_event), label);

    g_signal_connect(output, "destroy", G_CALLBACK(destroy_event), hbox);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "libxw outputs example");

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), box);

    if (!xw_outputs) {
        GtkWidget *label = gtk_label_new("outputs is not supported in your environment");
        gtk_container_add(GTK_CONTAINER(box), label);
    } else {
        GList *outputs = xw_outputs_get_outputs(xw_outputs);
        for (GList *output = outputs; output; output = output->next) {
            XwOutput *data = output->data;

            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

            GtkWidget *check_button = gtk_check_button_new();
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), xw_output_get_enabled(data));
            g_signal_connect(check_button, "toggled", G_CALLBACK(toggled_event), data);
            gtk_container_add(GTK_CONTAINER(hbox), check_button);

            GtkWidget *label = gtk_label_new(xw_output_get_description(data));
            gtk_container_add(GTK_CONTAINER(hbox), label);

            gtk_container_add(GTK_CONTAINER(box), hbox);

            g_signal_connect(data, "notify::description", G_CALLBACK(notify_description_event), label);

            g_signal_connect(data, "destroy", G_CALLBACK(destroy_event), hbox);
        }
    }

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    xw_outputs = xw_outputs_new();
    if (xw_outputs)
        g_signal_connect(xw_outputs, "new-output", G_CALLBACK(new_output_event), NULL);

    GtkApplication *app = gtk_application_new("org.libxw.examples.outputs", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (xw_outputs)
        g_object_unref(xw_outputs);

    return status;
}
