#include <cutils/log.h>

#include <cstring>

#include "WaylandWindowManager.h"

#include "wayland-android-client-protocol.h"

WaylandWindowManager::WaylandWindowManager()
{
    dpy = wl_display_connect(NULL);
    if(!dpy) goto exit_error;

    registry = wl_display_get_registry(dpy);
    if(!registry) goto exit_error;

    wl_registry_add_listener(registry, &registry_listener, this);

    wl_display_roundtrip(dpy);

    wl_output_add_listener(output, &output_listener, this);
    wl_seat_add_listener(seat, &w_seat_listener, this);

    wl_display_dispatch(dpy);
    wl_display_roundtrip(dpy);

    return;

exit_error:
    if(dpy != NULL)
    {
        wl_display_disconnect(dpy);
    }
    ALOGE("failed to initialize WaylandWindowManager");
}

int WaylandWindowManager::initialize()
{
    return 0;
}

void WaylandWindowManager::seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
    WaylandWindowManager *self = reinterpret_cast<WaylandWindowManager*>(data);

}

void WaylandWindowManager::registry_add_object(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    WaylandWindowManager *self = reinterpret_cast<WaylandWindowManager*>(data);

    if(strcmp(interface, "wl_compositor") == 0)
    {
        self->compositor = (struct wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, 0);
    }
    else if(strcmp(interface, "wl_shell") == 0)
    {
        self->shell = (struct wl_shell*)wl_registry_bind(registry, name, &wl_shell_interface, 0);
    }
    else if(strcmp(interface, "wl_output") == 0)
    {
        self->output = (struct wl_output*)wl_registry_bind(registry, name, &wl_output_interface, 0);
    }
    else if(strcmp(interface, "wl_seat") == 0)
    {
        self->seat = (struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, 0);
    }
    else if(strcmp(interface, "qt_surface_extension") == 0)
    {
        self->q_surface_extension = (struct qt_surface_extension*)wl_registry_bind(registry, name, &self->qt_surface_extension_interface, 0);
    }
    else if(strcmp(interface, "android_wlegl") == 0)
    {
        self->a_android_wlegl = static_cast<struct android_wlegl*>(wl_registry_bind(registry, name, &android_wlegl_interface, 1));
    }
    else
    {
        ALOGE("ignored interface: %s", interface);
    }
}

void WaylandWindowManager::registry_remove_object(void *data, struct wl_registry *registry, uint32_t name)
{
    ALOGW("registry remove object: %d", name);
}

void WaylandWindowManager::output_handle_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform)
{
    ALOGW("output handle geometry");
}

void WaylandWindowManager::output_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
    ALOGW("output handle mode");
    WaylandWindowManager *self = reinterpret_cast<WaylandWindowManager*>(data);

    self->screen_width = width;
    self->screen_height = height;
}

void WaylandWindowManager::output_handle_done(void *data, struct wl_output *wl_output)
{
    ALOGW("output handle done");
}

void WaylandWindowManager::output_handle_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
    ALOGW("output handle scale");
}

int WaylandWindowManager::finalize()
{
    return 0;
}

WaylandWindowManager::~WaylandWindowManager()
{
    wl_display_disconnect(dpy);
}

