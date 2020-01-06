#include "Text.h"

#include <cstdlib>

#include <Bridge.h>

const std::string &Text::getName() {
    static const std::string name("text");
    return name;
}

std::string Text::getContent() {
    return content_;
}

void Text::start(std::shared_ptr<Bridge> cb) {
    cb_ = cb;

    auto options = cb_->getOptions();
    auto i = options["text"];
    if (i) {
        content_ = i.as<std::string>();
    }

    i = options["left-click"];
    if (i) {
        left_click_ = i.as<std::string>();
    }

    i = options["middle-click"];
    if (i) {
        middle_click_ = i.as<std::string>();
    }

    i = options["right-click"];
    if (i) {
        right_click_ = i.as<std::string>();
    }
}

void Text::clickEvent(uint16_t button) {
    switch (button) {
    case XCB_BUTTON_INDEX_1: {
        // 左键
        if (!left_click_.empty()) {
            system(left_click_.c_str());
        }
    } break;
    case XCB_BUTTON_INDEX_2: {
        // 中键
        if (!middle_click_.empty()) {
            system(middle_click_.c_str());
        }
    } break;
    case XCB_BUTTON_INDEX_3: {
        // 右键
        if (!right_click_.empty()) {
            system(right_click_.c_str());
        }
    } break;
    }
}
