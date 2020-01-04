#ifndef YIGEBAR_SRC_ATOMS_H
#define YIGEBAR_SRC_ATOMS_H

#include <cstddef>

#include <xcb/xproto.h>

enum class ATOM {
    _NET_WM_WINDOW_TYPE,
    _NET_WM_WINDOW_TYPE_DOCK, // 窗口類型爲 dock
    _NET_WM_STATE,
    _NET_WM_STATE_STICKY, // 固定位置，即使切換虛擬桌面
    _NET_WM_STRUT,
};

const xcb_atom_t atoms_type[] = {32, 32, 8, 8, 32};

struct AtomName {
    const char *name;
    size_t len;
};

// clang-format off
const AtomName atoms_name[] = {
    {"_NET_WM_WINDOW_TYPE", sizeof("_NET_WM_WINDOW_TYPE") - 1},
    {"_NET_WM_WINDOW_TYPE_DOCK", sizeof("_NET_WM_WINDOW_TYPE_DOCK") - 1}, // 窗口類型爲 dock
    {"_NET_WM_STATE", sizeof("_NET_WM_STATE") - 1},
    {"_NET_WM_STATE_STICKY", sizeof("_NET_WM_STATE_STICKY") - 1}, // 固定位置，即使切換虛擬桌面
    {"_NET_WM_STRUT", sizeof("_NET_WM_STRUT") - 1},
};
// clang-format on

#endif // !YIGEBAR_SRC_ATOMS_H
