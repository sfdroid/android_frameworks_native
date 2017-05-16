#ifndef WAYLAND_WINDOW_MANAGER__
#define WAYLAND_WINDOW_MANAGER__

#include <wayland-client.h>

#include <string>
#include <vector>

#include <system/window.h>

class WaylandWindowManager {
private:
    static void seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t caps);
    static void registry_add_object(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    static void registry_remove_object(void *data, struct wl_registry *registry, uint32_t name);

protected:
    struct wl_display *dpy;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shell *shell;
    struct wl_seat *seat;
    const struct wl_registry_listener registry_listener {
        registry_add_object,
        registry_remove_object
    };
    const struct wl_seat_listener w_seat_listener = {
        seat_handle_capabilities,
    };
    struct wl_output *output;

    static void output_handle_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform);
    static void output_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh);
    static void output_handle_done(void *data, struct wl_output *wl_output);
    static void output_handle_scale(void *data, struct wl_output *wl_output, int32_t factor);
    const struct wl_output_listener output_listener = {&output_handle_geometry, &output_handle_mode, &output_handle_done, &output_handle_scale};
    struct android_wlegl *a_android_wlegl;

    const struct wl_message qt_extended_surface_requests[3] = {
        { "update_generic_property", "sa", qt_surface_extension_types + 0 },
        { "set_content_orientation", "i", qt_surface_extension_types + 0 },
        { "set_window_flags", "i", qt_surface_extension_types + 0 },
    };

    const struct wl_message qt_extended_surface_events[3] = {
        { "onscreen_visibility", "i", qt_surface_extension_types + 0 },
        { "set_generic_property", "sa", qt_surface_extension_types + 0 },
        { "close", "", qt_surface_extension_types + 0 },
    };

    WL_EXPORT const struct wl_interface qt_extended_surface_interface = {
        "qt_extended_surface", 1,
        3, qt_extended_surface_requests,
        3, qt_extended_surface_events,
    };
    const struct wl_interface *qt_surface_extension_types[4] = {
        NULL,
        NULL,
        &qt_extended_surface_interface,
        &wl_surface_interface,
    };

    const struct wl_message qt_surface_extension_requests[1] = {
        { "get_extended_surface", "no", qt_surface_extension_types + 2 },
    };

    WL_EXPORT const struct wl_interface qt_surface_extension_interface = {
        "qt_surface_extension", 1,
        1, qt_surface_extension_requests,
        0, NULL,
    };

    struct qt_surface_extension *q_surface_extension;

    int screen_width;
    int screen_height;

public:
    virtual int initialize();
    virtual void pushFrame(std::vector<std::string> layer_names, buffer_handle_t buffer, int width, int height, int stride, int pixel_format) = 0;
    virtual int finalize();

    WaylandWindowManager();
    virtual ~WaylandWindowManager();
};

#endif

