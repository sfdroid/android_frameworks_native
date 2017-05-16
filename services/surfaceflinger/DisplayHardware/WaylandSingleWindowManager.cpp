#include "WaylandSingleWindowManager.h"

#include <hardware/gralloc.h>
#include <cutils/log.h>

#define QT_SURFACE_EXTENSION_GET_EXTENDED_SURFACE 0

int WaylandSingleWindowManager::initialize()
{
    ALOGW("initializing WaylandSingleWindowManager");

    ALOGW("creating surface");
    w_surface = wl_compositor_create_surface(WaylandWindowManager::compositor);

    ALOGW("getting shell surface");
    w_shell_surface = wl_shell_get_shell_surface(shell, w_surface);

    ALOGW("getting qt_extended surface");
    q_extended_surface = (struct qt_extended_surface*)wl_proxy_create((struct wl_proxy *)q_surface_extension, &qt_extended_surface_interface);

    wl_proxy_marshal((struct wl_proxy*)q_surface_extension, QT_SURFACE_EXTENSION_GET_EXTENDED_SURFACE, q_extended_surface, w_surface);

    ALOGW("adding listeners");
    wl_proxy_add_listener((struct wl_proxy*)q_extended_surface, (void (**)(void))&extended_surface_listener, this);
    wl_display_roundtrip(dpy);

    wl_shell_surface_add_listener(w_shell_surface, &shell_surface_listener, NULL);

    wl_shell_surface_set_toplevel(w_shell_surface);

    ALOGW("creating wl region");
    struct wl_region *region;
    region = wl_compositor_create_region(WaylandWindowManager::compositor);
    wl_region_add(region, 0, 0,
                  screen_width,
                  screen_height);
    wl_surface_set_opaque_region(w_surface, region);
    wl_region_destroy(region);

    frame_callback_ptr = 0;

    return 0;
}

void WaylandSingleWindowManager::pushFrame(std::vector<std::string> layer_names, buffer_handle_t buffer, int width, int height, int stride, int pixel_format)
{
    ALOGW("pushing frame");
    if(buffer_map.find(buffer) == buffer_map.end())
    {
        struct wl_buffer *w_buffer;
        struct wl_array ints;
        int *the_ints;
        struct android_wlegl_handle *wlegl_handle;

        wl_array_init(&ints);
        the_ints = (int*)wl_array_add(&ints, buffer->numInts * sizeof(int));
        memcpy(the_ints, buffer->data + buffer->numFds, buffer->numInts * sizeof(int));
        wlegl_handle = android_wlegl_create_handle(a_android_wlegl, buffer->numFds, &ints);
        wl_array_release(&ints);

        for(int i = 0; i < buffer->numFds; i++)
        {
            android_wlegl_handle_add_fd(wlegl_handle, buffer->data[i]);
        }

        w_buffer = android_wlegl_create_buffer(a_android_wlegl, width, height, stride, pixel_format, GRALLOC_USAGE_HW_RENDER, wlegl_handle);
        android_wlegl_handle_destroy(wlegl_handle);

        wl_buffer_add_listener(w_buffer, &w_buffer_listener, this);

        buffer_map[buffer] = w_buffer;
    }

    int ret = 0;
    while(frame_callback_ptr && ret != -1)
    {
        ret = wl_display_dispatch(dpy);
    }

    frame_callback_ptr = wl_surface_frame(w_surface);
    wl_callback_add_listener(frame_callback_ptr, &w_frame_listener, this);

    ALOGW("final commit");
    wl_surface_attach(w_surface, buffer_map[buffer], 0, 0);
    wl_surface_damage(w_surface, 0, 0, width, height);
    wl_surface_commit(w_surface);
}

void WaylandSingleWindowManager::buffer_release(void *data, struct wl_buffer *buffer)
{
    ALOGW("buffer release");

    // we're cleaning in finalize()
}

void WaylandSingleWindowManager::shell_surface_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
    ALOGW("shell surface ping");
    wl_shell_surface_pong(shell_surface, serial);
}

void WaylandSingleWindowManager::shell_surface_configure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
{
    ALOGW("shell surface configure");
}

void WaylandSingleWindowManager::shell_surface_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
    ALOGW("shell surface popup done");
}

void WaylandSingleWindowManager::handle_onscreen_visibility(void *data, struct qt_extended_surface *qt_extended_surface, int32_t visible)
{
    WaylandSingleWindowManager *self = (WaylandSingleWindowManager*)data;
    ALOGW("qt_extended_surface handle onscreen visibility");
}

void WaylandSingleWindowManager::handle_set_generic_property(void *data, struct qt_extended_surface *qt_extended_surface, const char *name, struct wl_array *value)
{
    ALOGW("qt_extended_surface handle set generic property");
}

void WaylandSingleWindowManager::handle_close(void *data, struct qt_extended_surface *qt_extended_surface)
{
    ALOGW("qt_extended_surface handle close");

    //WaylandSingleWindowManager *self = (WaylandSingleWindowManager*)data;
    //self->handle_close_impl(self->w_surface);
}

void WaylandSingleWindowManager::frame_callback(void *data, struct wl_callback *callback, uint32_t time)
{
    WaylandSingleWindowManager *self = (WaylandSingleWindowManager*)data;
    self->frame_callback_ptr = 0;
    wl_callback_destroy(callback);
}

int WaylandSingleWindowManager::finalize()
{
    for(std::map<buffer_handle_t, struct wl_buffer*>::iterator it = buffer_map.begin();it != buffer_map.end();it++)
    {
        wl_buffer_destroy(it->second);
    }
    buffer_map.clear();

    wl_proxy_destroy((struct wl_proxy *)q_extended_surface);
    wl_shell_surface_destroy(w_shell_surface);
    wl_surface_destroy(w_surface);
    return 0;
}

