#ifndef YIGEBAR_SRC_YIGEBAR_H
#define YIGEBAR_SRC_YIGEBAR_H

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <xcb/xproto.h>

#include <yaml-cpp/yaml.h>

#include "Atoms.h"
#include "ModuleLoader.h"
#include "X11.h"
#include "Painter.h"

class Painter;

class YigeBar {
public:
    enum class MODULE_POSITION {LEFT, RIGHT};

    YigeBar();
    void init();
    void show();

    /**
     * 窗口顯示回調
     */
    void onShow();

    /**
     * 鼠標按下回調
     */
    void onButtonPress(uint16_t button, uint16_t x, uint16_t y);

    void setProperty(uint8_t mode, ATOM property, size_t len, const ATOM *values);
    void setProperty(uint8_t mode, ATOM property, ATOM value) {
        setProperty(mode, property, 1, &value);
    }

    void update(MODULE_POSITION pos, uint16_t module_id);

    xcb_window_t id() { return window_id_; }

private:
    static constexpr ATOM preload_atoms_[] = {
            ATOM::_NET_WM_WINDOW_TYPE,
            ATOM::_NET_WM_WINDOW_TYPE_DOCK,
            ATOM::_NET_WM_STATE,
            ATOM::_NET_WM_STATE_STICKY,
            ATOM::_NET_WM_STRUT,
    };

    int argc_;
    const char **argv_;

    struct ModuleInfo {
        bool updated;
        uint16_t start_offset;
        uint16_t end_offset;
        YAML::Node options;
        std::shared_ptr<Module> instance;
    };

    struct Options {
        uint16_t width;
        uint16_t height;
        ColorARGB background;
        std::string font_family;
        uint16_t font_size;
    };

    Options options_;

    ModuleLoader module_loader_;

    std::shared_ptr<X11> x11_;
    xcb_window_t window_id_;

    std::shared_ptr<Painter> painter_;

    std::vector<ModuleInfo> left_modules_;
    std::vector<ModuleInfo> right_modules_;

    std::mutex modules_mutex_;
    std::condition_variable modules_cv_;

    std::thread modules_update_loop_;

    void modulesLoop();

    void initOptions();

};

#endif // !YIGEBAR_SRC_YIGEBAR_H
