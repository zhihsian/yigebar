#ifndef YIGEBAR_SRC_X11_H
#define YIGEBAR_SRC_X11_H

#include <list>
#include <vector>
#include <unordered_map>

#include <xcb/xcb.h>

#include "Atoms.h"

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

class YigeBar;

class X11 {
public:
    X11();
    ~X11();

    xcb_connection_t *conn() { return conn_; }
    const xcb_screen_t *screen() { return screen_; }
    xcb_atom_t getAtom(ATOM atom) { return loaded_atoms_.find(atom)->second; }

    void mapWindow(xcb_window_t id) {
        xcb_map_window(conn_, id);
        flush();
    }

    void preloadAtoms(size_t len, const ATOM *atoms);
    void preloadAtoms(const std::vector<ATOM> &atoms);

    xcb_window_t createWindow(YigeBar *window, uint16_t width, uint16_t height);

    void changeProperty(xcb_window_t window_id,
                        uint8_t mode,
                        ATOM property,
                        size_t len,
                        const ATOM *values);

    void flush() { xcb_flush(conn_); }

    void loop();

    xcb_visualtype_t *getRootVisualType();

private:
    std::unordered_map<ATOM, xcb_atom_t> loaded_atoms_;

    xcb_connection_t *conn_;
    xcb_screen_t *screen_;

    YigeBar *window_;

    cairo_surface_t *cairo_surface_;
    cairo_t *cairo_;
};

#endif // !YIGEBAR_SRC_X11_H
