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

#include "screenshot.h"

XwScreenshot *xw_screenshot;

GtkWidget *image;

static void clicked_event(GtkWidget *widget, gpointer user_data) {
    GdkPixbuf *pixbuf = xw_screenshot_take(xw_screenshot, gdk_display_get_monitor_at_window(gtk_widget_get_display(widget), gtk_widget_get_window(widget)));
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, gdk_pixbuf_get_width(pixbuf) / 2, gdk_pixbuf_get_height(pixbuf) / 2, GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);
    g_object_unref(scaled_pixbuf);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "libxw screenshot example");

    image = gtk_image_new();

    GtkWidget *button = gtk_button_new_with_label("Screenshot");
    g_signal_connect(button, "clicked", G_CALLBACK(clicked_event), NULL);

    if (!xw_screenshot)
        gtk_widget_set_sensitive(button, FALSE);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(box), image);
    gtk_container_add(GTK_CONTAINER(box), button);
    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    xw_screenshot = xw_screenshot_new();

    GtkApplication *app = gtk_application_new("org.libxw.examples.screenshot", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (xw_screenshot)
        g_object_unref(xw_screenshot);

    return status;
}
