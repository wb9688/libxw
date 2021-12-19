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

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include <gdk/gdkwayland.h>

#include <wayland-client-protocol.h>
#include "wlr-screencopy-unstable-v1-client-protocol.h"

#include "screenshot-wlroots.h"

struct _XwScreenshotWlroots {
    GObject parent_instance;

    struct wl_shm *shm;
    struct zwlr_screencopy_manager_v1 *screencopy_manager;

    struct wl_buffer *wl_buffer;
    void *data;
    enum wl_shm_format format;
    int width, height, stride;
    gboolean y_invert;
    gboolean buffer_copy_done;
    GMutex mutex;
};

static void xw_screenshot_wlroots_screenshot_interface_init(XwScreenshotInterface *iface);
static void xw_screenshot_wlroots_finalize(GObject *gobject);
static GdkPixbuf *xw_screenshot_wlroots_take(XwScreenshot *self, GdkMonitor *monitor);

G_DEFINE_TYPE_WITH_CODE(XwScreenshotWlroots, xw_screenshot_wlroots, G_TYPE_OBJECT, G_IMPLEMENT_INTERFACE(XW_TYPE_SCREENSHOT, xw_screenshot_wlroots_screenshot_interface_init))

static void xw_screenshot_wlroots_class_init(XwScreenshotWlrootsClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = xw_screenshot_wlroots_finalize;
}

static void xw_screenshot_wlroots_screenshot_interface_init(XwScreenshotInterface *iface) {
    iface->take = xw_screenshot_wlroots_take;
}

static void handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    XwScreenshotWlroots *self = XW_SCREENSHOT_WLROOTS(data);

    if (strcmp(interface, wl_shm_interface.name) == 0) {
        self->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
        self->screencopy_manager = wl_registry_bind(registry, name, &zwlr_screencopy_manager_v1_interface, 1);
    }
}

static void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove
};

static void xw_screenshot_wlroots_init(XwScreenshotWlroots *self) {
    GdkDisplay *gdk_display = gdk_display_get_default();
    struct wl_display *display = gdk_wayland_display_get_wl_display(GDK_WAYLAND_DISPLAY(gdk_display));

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, self);
    wl_display_roundtrip(display);

    g_mutex_init(&self->mutex);
}

static void xw_screenshot_wlroots_finalize(GObject *gobject) {
    XwScreenshotWlroots *self = XW_SCREENSHOT_WLROOTS(gobject);

    wl_shm_destroy(self->shm);
    zwlr_screencopy_manager_v1_destroy(self->screencopy_manager);

    g_mutex_clear(&self->mutex);

    G_OBJECT_CLASS(xw_screenshot_wlroots_parent_class)->finalize(gobject);
}

static struct wl_buffer *create_shm_buffer(XwScreenshotWlroots *self, enum wl_shm_format fmt, int width, int height, int stride, void **data_out) {
    int size = stride * height;

    const char shm_name[] = "/wlroots-screencopy";
    int fd = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        g_critical("shm_open failed");
        return NULL;
    }
    shm_unlink(shm_name);

    int ret;
    while ((ret = ftruncate(fd, size)) == EINTR) {}
    if (ret < 0) {
        close(fd);
        g_critical("ftruncate failed");
        return NULL;
    }

    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        g_critical("mmap failed");
        close(fd);
        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(self->shm, fd, size);
    close(fd);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, fmt);
    wl_shm_pool_destroy(pool);

    *data_out = data;
    return buffer;
}

static void frame_handle_buffer(void *data, struct zwlr_screencopy_frame_v1 *frame, uint32_t format, uint32_t width, uint32_t height, uint32_t stride) {
    XwScreenshotWlroots *self = XW_SCREENSHOT_WLROOTS(data);

    self->format = format;
    self->width = width;
    self->height = height;
    self->stride = stride;

    assert(!self->wl_buffer);
    self->wl_buffer = create_shm_buffer(self, format, width, height, stride, &self->data);
    if (self->wl_buffer == NULL) {
        g_error("Failed to create buffer");
    }

    zwlr_screencopy_frame_v1_copy(frame, self->wl_buffer);
}

static void frame_handle_flags(void *data, struct zwlr_screencopy_frame_v1 *frame, uint32_t flags) {
    XwScreenshotWlroots *self = XW_SCREENSHOT_WLROOTS(data);

    self->y_invert = flags & ZWLR_SCREENCOPY_FRAME_V1_FLAGS_Y_INVERT;
}

static void frame_handle_ready(void *data, struct zwlr_screencopy_frame_v1 *frame, uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec) {
    XwScreenshotWlroots *self = XW_SCREENSHOT_WLROOTS(data);

    self->buffer_copy_done = TRUE;
}

static void frame_handle_failed(void *data, struct zwlr_screencopy_frame_v1 *frame) {
    g_error("Failed to copy frame");
}

static const struct zwlr_screencopy_frame_v1_listener frame_listener = {
    .buffer = frame_handle_buffer,
    .flags = frame_handle_flags,
    .ready = frame_handle_ready,
    .failed = frame_handle_failed
};

struct format {
    enum wl_shm_format wl_format;
    gboolean is_bgr;
};

// wl_shm_format describes little-endian formats, GDK-PixBuf uses big-endian formats (so Wayland's ABGR is GDK-PixBuf's RGBA).
static const struct format formats[] = {
    { WL_SHM_FORMAT_XRGB8888, TRUE },
    { WL_SHM_FORMAT_ARGB8888, TRUE },
    { WL_SHM_FORMAT_XBGR8888, FALSE },
    { WL_SHM_FORMAT_ABGR8888, FALSE }
};

static GdkPixbuf *xw_screenshot_wlroots_take(XwScreenshot *self, GdkMonitor *monitor) {
    XwScreenshotWlroots *self_wlroots = XW_SCREENSHOT_WLROOTS(self);

    self_wlroots->buffer_copy_done = FALSE;
    g_mutex_lock(&self_wlroots->mutex);

    struct zwlr_screencopy_frame_v1 *frame = zwlr_screencopy_manager_v1_capture_output(self_wlroots->screencopy_manager, 0, gdk_wayland_monitor_get_wl_output(monitor));
    zwlr_screencopy_frame_v1_add_listener(frame, &frame_listener, self);

    GdkDisplay *gdk_display = gdk_monitor_get_display(monitor);
    struct wl_display *display = gdk_wayland_display_get_wl_display(GDK_WAYLAND_DISPLAY(gdk_display));

    while (!self_wlroots->buffer_copy_done && wl_display_dispatch(display) != -1) {}

    const struct format *fmt = NULL;
    for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); ++i) {
        if (formats[i].wl_format == self_wlroots->format) {
            fmt = &formats[i];
            break;
        }
    }
    if (fmt == NULL)
        g_error("Unsupported format %"PRIu32, self_wlroots->format);

    guchar *data;
    if (fmt->is_bgr) {
        data = g_malloc(self_wlroots->height * self_wlroots->stride);
        for (int i = 0; i < self_wlroots->height * self_wlroots->width; i++) {
            data[i * 4 + 2] = ((guchar *) self_wlroots->data)[i * 4 + 0];
            data[i * 4 + 1] = ((guchar *) self_wlroots->data)[i * 4 + 1];
            data[i * 4 + 0] = ((guchar *) self_wlroots->data)[i * 4 + 2];
            data[i * 4 + 3] = ((guchar *) self_wlroots->data)[i * 4 + 3];
        }
    } else
        data = g_memdup2(self_wlroots->data, self_wlroots->height * self_wlroots->stride);

    if (self_wlroots->y_invert) {
        guchar *original_data = data;
        data = g_malloc(self_wlroots->height * self_wlroots->stride);
        for (gint i = 0; i < self_wlroots->height; i++)
            for (gint j = 0; j < self_wlroots->stride; j++)
                data[i * self_wlroots->stride + j] = original_data[(self_wlroots->height - i) * self_wlroots->stride + j];
        g_free(original_data);
    }

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB, TRUE, 8, self_wlroots->width, self_wlroots->height, self_wlroots->stride, (GdkPixbufDestroyNotify) g_free, NULL);

    wl_buffer_destroy(self_wlroots->wl_buffer);
    self_wlroots->wl_buffer = NULL;
    munmap(self_wlroots->data, self_wlroots->stride * self_wlroots->height);
    g_mutex_unlock(&self_wlroots->mutex);

    return pixbuf;
}

gboolean xw_screenshot_wlroots_is_supported() {
    GdkDisplay *display = gdk_display_get_default();

    return GDK_IS_WAYLAND_DISPLAY(display) && gdk_wayland_display_query_registry(GDK_WAYLAND_DISPLAY(display), wl_shm_interface.name) && gdk_wayland_display_query_registry(GDK_WAYLAND_DISPLAY(display), zwlr_screencopy_manager_v1_interface.name);
}
