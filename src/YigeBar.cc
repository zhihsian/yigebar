#include "YigeBar.h"

#include <filesystem>

#include <glog/logging.h>

#include <Bridge.h>
#include <Painter.h>

using std::swap;

YigeBar::YigeBar()
    : options_({0})
    , x11_(std::make_shared<X11>())
    , window_id_(0) {
    std::filesystem::path path(INSTALL_PREFIX);
    path = path.append("lib/yigebar/modules");

    module_loader_.setDirection(path);
}

void YigeBar::init() {
    initOptions();

    x11_->preloadAtoms(sizeof(preload_atoms_) / sizeof(preload_atoms_[0]),
                       preload_atoms_);

    window_id_ = x11_->createWindow(this, options_.width, options_.height);
    setProperty(XCB_PROP_MODE_REPLACE,
                ATOM::_NET_WM_WINDOW_TYPE,
                ATOM::_NET_WM_WINDOW_TYPE_DOCK);
    setProperty(XCB_PROP_MODE_APPEND,
                ATOM::_NET_WM_STATE,
                ATOM::_NET_WM_STATE_STICKY);
}

void YigeBar::setProperty(uint8_t mode,
                          ATOM property,
                          size_t len,
                          const ATOM *values) {
    x11_->changeProperty(window_id_, mode, property, len, values);
}

void YigeBar::show() {
    x11_->mapWindow(window_id_);

    painter_ = std::make_shared<Painter>(x11_->conn(),
                                         window_id_,
                                         x11_->getRootVisualType(),
                                         options_.width,
                                         options_.height);

    painter_->setFontFace(options_.font_family);
    painter_->setFontSize(options_.font_size);

    modules_update_loop_ = std::thread(&YigeBar::modulesLoop, this);

    x11_->loop();
}

void YigeBar::onShow() {
}

void YigeBar::onButtonPress(uint16_t button, uint16_t x, uint16_t y) {
    uint16_t half_width = options_.width / 2;
    const std::vector<ModuleInfo> *search = &right_modules_,
                                  *next_search = &left_modules_;
    if (x > half_width) {
        swap(search, next_search);
    }

    for (int j = 0; j < 2; ++j) {
        for (const auto &i : *search) {
            if (i.start_offset <= x && x <= i.end_offset) {
                i.instance->clickEvent(button);

                return;
            }
        }

        swap(search, next_search);
    }
}

void YigeBar::initOptions() {
    options_.width = x11_->screen()->width_in_pixels;

    std::filesystem::path config_file;
    const char *config_home = getenv("XDG_CONFIG_HOME");
    if (config_home) {
        config_file = config_home;
    } else {
        const char *home = getenv("HOME");
        if (!home) {
            LOG(FATAL) << "cannot get environment HOME";
        }

        config_file = home;
        config_file.append(".config");
    }

    config_file.append("yigebar/config.yml");

    if (!std::filesystem::exists(config_file)) {
        LOG(FATAL) << "config file not exists: " << config_file.string();
    }

    YAML::Node config = YAML::LoadFile(config_file.string());
    options_.height = config["height"].as<uint16_t>();
    options_.background =
            ColorARGB::fromul(config["background"].as<uint32_t>());
    options_.font_family = config["font-family"].as<std::string>();
    options_.font_size = config["font-size"].as<uint16_t>();

    auto left_modules = config["left-modules"];
    if (left_modules) {
        left_modules_.reserve(left_modules.size());
        for (const YAML::Node &i : left_modules) {
            auto name = i["name"].as<std::string>();

            // 加載模塊
            std::shared_ptr<Module> module = module_loader_.load(name);
            size_t id = left_modules_.size();

            // 配置回調信息並啓動模塊
            auto cb = std::make_shared<Bridge>(this,
                                                 MODULE_POSITION::LEFT,
                                                 id,
                                                 i);
            module->start(cb);

            left_modules_.emplace_back(ModuleInfo({false, 0, 0, i, module}));
        }
    }

    auto right_modules = config["right-modules"];
    if (right_modules) {
        right_modules_.reserve(right_modules.size());
        for (const YAML::Node &i : right_modules) {
            auto name = i["name"].as<std::string>();

            // 加載模塊
            std::shared_ptr<Module> module = module_loader_.load(name);
            size_t id = right_modules_.size();

            // 配置回調信息並啓動模塊
            auto cb = std::make_shared<Bridge>(this,
                                                 MODULE_POSITION::RIGHT,
                                                 id,
                                                 i);
            module->start(cb);

            right_modules_.emplace_back(ModuleInfo({false, 0, 0, i, module}));
        }
    }
}

void YigeBar::update(MODULE_POSITION pos, uint16_t module_id) {
    {
        std::lock_guard lock(modules_mutex_);
        if (pos == MODULE_POSITION::LEFT) {
            left_modules_[module_id].updated = true;
        } else {
            right_modules_[module_id].updated = true;
        }
    }

    modules_cv_.notify_one();
}

void YigeBar::modulesLoop() {
    std::unique_lock lock(modules_mutex_);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for (;;) {
        modules_cv_.wait(lock);

        // 繪製背景
        painter_->drawRectangle(0,
                                0,
                                options_.width,
                                options_.height,
                                options_.background);

        uint16_t x, y, w, h;
        uint16_t content_padding;

        uint16_t offset = 0;
        for (auto &i : left_modules_) {
            i.start_offset = offset;
            content_padding =
                    i.options["content-padding"]
                            ? i.options["content-padding"].as<uint16_t>()
                            : 0;

            std::string text = i.instance->getContent();
            painter_->getTextSize(text, &w, &h);
            offset += content_padding;
            x = offset;
            y = (options_.height + h) / 2; // 垂直居中
            offset += w + content_padding;
            i.end_offset = offset;

            if (i.options["background"]) {
                auto background = i.options["background"].as<uint32_t>();
                painter_->drawRectangle(i.start_offset,
                                        0,
                                        content_padding + w + content_padding,
                                        options_.height,
                                        ColorARGB::fromul(background));
            }
            painter_->showText(x, y, text);
        }

        offset = options_.width;
        for (auto &i : right_modules_) {
            i.end_offset = offset;
            content_padding =
                    i.options["content-padding"]
                            ? i.options["content-padding"].as<uint16_t>()
                            : 0;

            std::string text = i.instance->getContent();
            painter_->getTextSize(text, &w, &h);
            offset -= content_padding + w;
            x = offset;
            y = (options_.height + h) / 2; // 垂直居中

            offset -= content_padding;
            i.start_offset = offset;

            if (i.options["background"]) {
                auto background = i.options["background"].as<uint32_t>();
                painter_->drawRectangle(i.start_offset,
                                        0,
                                        content_padding + w + content_padding,
                                        options_.height,
                                        ColorARGB::fromul(background));
            }
            painter_->showText(x, y, text);
        }

        x11_->flush();
    }
#pragma clang diagnostic pop
}
