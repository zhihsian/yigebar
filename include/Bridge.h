#ifndef YIGEBAR_INCLUDE_BRIDGE_H
#define YIGEBAR_INCLUDE_BRIDGE_H

#include <cstdint>

#include <yaml-cpp/yaml.h>

#include "YigeBar.h"

class Bridge {
public:
    Bridge(YigeBar *bar,
           YigeBar::MODULE_POSITION pos,
           uint16_t id,
           const YAML::Node &options);

    virtual const YAML::Node &getOptions();

    /**
     * 當模塊內容有更新，通知 bar 更新內容
     */
    virtual void postUpdate();

private:
    YigeBar *bar_;
    YigeBar::MODULE_POSITION pos_;
    uint16_t id_;
    YAML::Node options_;
};

#endif // !YIGEBAR_INCLUDE_BRIDGE_H
