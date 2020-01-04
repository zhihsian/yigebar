#include "X11.h"

#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>

#include <glog/logging.h>

#include "YigeBar.h"

X11::X11()
    : window_(nullptr)
    , cairo_surface_(nullptr)
    , cairo_(nullptr) {
    int screen_index;

    // 連接 X server
    conn_ = xcb_connect(nullptr, &screen_index);

    // 根據屏幕下標獲取屏幕
    const xcb_setup_t *setup = xcb_get_setup(conn_);
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(setup);

    for (int i = 0; i < screen_index; i++) {
        xcb_screen_next(&screen_iter);
    }

    screen_ = screen_iter.data;
}

X11::~X11() {
    // if (cairo_surface_) cairo_surface_finish(cairo_surface_);
    if (conn_) xcb_disconnect(conn_);
}

void X11::preloadAtoms(size_t len, const ATOM *atoms) {
    xcb_intern_atom_cookie_t cookies[len];
    const AtomName *atom_info;
    for (size_t i = 0; i < len; ++i) {
        atom_info = &atoms_name[static_cast<uint16_t>(atoms[i])];
        cookies[i] = xcb_intern_atom(conn_, 0, atom_info->len, atom_info->name);
    }

    xcb_intern_atom_reply_t *reply;
    for (size_t i = 0; i < len; ++i) {
        reply = xcb_intern_atom_reply(conn_, cookies[i], nullptr);
        if (!reply) {
            continue;
        }
        loaded_atoms_.insert(std::make_pair(atoms[i], reply->atom));

        free(reply);
    }
}

void X11::preloadAtoms(const std::vector<ATOM> &atoms) {
    preloadAtoms(atoms.size(), atoms.data());
}

xcb_window_t
X11::createWindow(YigeBar *window, uint16_t width, uint16_t height) {
    xcb_window_t window_id = xcb_generate_id(conn_);

    uint32_t mask = XCB_CW_EVENT_MASK;
    uint32_t values[] = {
            XCB_EVENT_MASK_EXPOSURE |             // 接收 exposure 事件
                    XCB_EVENT_MASK_BUTTON_PRESS | // 接收鼠标按钮按下事件
                    XCB_EVENT_MASK_BUTTON_RELEASE | // 接收鼠标按钮释放事件
                    XCB_EVENT_MASK_KEY_PRESS |      // 接收键盘按下事件
                    XCB_EVENT_MASK_KEY_RELEASE, // 接收键盘释放事件
    };

    xcb_create_window(conn_,
                      XCB_COPY_FROM_PARENT,
                      window_id,
                      screen_->root,
                      0,      // x
                      0,      // y
                      width,  // 窗口寬度
                      height, // 窗口高度
                      0,      // 邊框寬度
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen_->root_visual,
                      mask,
                      values);

    window_ = window;

    return window_id;
}

void X11::loop() {
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(conn_))) {
        switch (event->response_type & ~0x80) {
        case 0: {
            auto *error = reinterpret_cast<xcb_generic_error_t *>(event);
            LOG(FATAL) << "xcb error: " << error->error_code
                       << ", sequence: " << error->sequence
                       << ", full_sequence: " << error->full_sequence
                       << ", resource id: " << error->resource_id
                       << ", major code: " << error->major_code
                       << ", minor code: " << error->minor_code;
        } break;
        case XCB_EXPOSE: {
            auto *ev = reinterpret_cast<xcb_expose_event_t *>(event);
            window_->onShow();
        } break;
        case XCB_BUTTON_PRESS: {
            auto *ev = reinterpret_cast<xcb_button_press_event_t *>(event);
            window_->onButtonPress(ev->detail, ev->event_x, ev->event_y);
        }
        default:
            break;
        }

        free(event);
    }
}

void X11::changeProperty(xcb_window_t window_id,
                         uint8_t mode,
                         ATOM property,
                         size_t len,
                         const ATOM *values) {
    xcb_atom_t p = getAtom(property);
    xcb_atom_t v[len];
    for (size_t i = 0; i < len; ++i) {
        v[i] = getAtom(values[i]);
    }
    xcb_atom_t type = atoms_type[static_cast<uint32_t>(property)];

    xcb_change_property(conn_, mode, window_id, p, XCB_ATOM_ATOM, type, len, v);

    flush();
}

xcb_visualtype_t *X11::getRootVisualType() {
    xcb_depth_iterator_t depth_iter =
            xcb_screen_allowed_depths_iterator(screen_);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t visual_iter =
                xcb_depth_visuals_iterator(depth_iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (screen_->root_visual == visual_iter.data->visual_id) {
                return visual_iter.data;
            }
        }
    }

    return nullptr;
}
