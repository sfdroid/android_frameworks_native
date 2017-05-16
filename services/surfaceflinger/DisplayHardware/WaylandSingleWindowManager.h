#ifndef WAYLAND_SINGLE_WINDOW_MANAGER__
#define WAYLAND_SINGLE_WINDOW_MANAGER__

#include <vector>
#include <string>
#include <map>

#include "WaylandWindowManager.h"

#include "wayland-android-client-protocol.h"

struct qt_extended_surface_listener
{
    void (*onscreen_visibility)(void *data, struct qt_extended_surface *qt_extended_surface, int32_t visible);
    void (*set_generic_property)(void *data, struct qt_extended_surface *qt_extended_surface, const char *name, struct wl_array *value);
    void (*close)(void *data, struct qt_extended_surface *qt_extended_surface);
};

class WaylandSingleWindowManager : public WaylandWindowManager
{
private:
    struct wl_shell_surface *w_shell_surface;
    struct wl_surface *w_surface;

    static void shell_surface_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial);
    static void shell_surface_configure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
    static void shell_surface_popup_done(void *data, struct wl_shell_surface *shell_surface);

    struct wl_shell_surface_listener shell_surface_listener = {&shell_surface_ping, &shell_surface_configure, &shell_surface_popup_done};

    static void handle_onscreen_visibility(void *data, struct qt_extended_surface *qt_extended_surface, int32_t visible);
    static void handle_set_generic_property(void *data, struct qt_extended_surface *qt_extended_surface, const char *name, struct wl_array *value);
    static void handle_close(void *data, struct qt_extended_surface *qt_extended_surface);

    static void buffer_release(void *data, struct wl_buffer *buffer);

    static void frame_callback(void *data, struct wl_callback *callback, uint32_t time);

    const struct qt_extended_surface_listener extended_surface_listener = { 
        &handle_onscreen_visibility,
        &handle_set_generic_property,
        &handle_close,
    };

    const struct wl_buffer_listener w_buffer_listener = {
        buffer_release
    };

    const struct wl_callback_listener w_frame_listener = {
        frame_callback
    };

    struct wl_callback *frame_callback_ptr;

    struct qt_extended_surface *q_extended_surface;

public:
    int initialize() override;
    void pushFrame(std::vector<std::string> layer_names, buffer_handle_t buffer, int width, int height, int stride, int pixel_format) override;
    int finalize() override;
    std::map<buffer_handle_t, struct wl_buffer*> buffer_map;

    WaylandSingleWindowManager() : WaylandWindowManager()
    {
    }

    ~WaylandSingleWindowManager()
    {
    }
};

#endif

