#ifndef YIGEBAR_SRC_MODULE_H
#define YIGEBAR_SRC_MODULE_H

#include <string>
#include <memory>

class Bridge;

class Module {
public:
    virtual ~Module() = default;

    virtual const std::string &getName() = 0;
    virtual std::string getContent() = 0;

    virtual void start(std::shared_ptr<Bridge> cb) = 0;

    virtual void clickEvent(uint16_t button) = 0;
    // virtual void doubleClickEvent() = 0;
    // virtual void scrollEvent() = 0;
};

extern "C" {
Module *getInstance();
}

#endif // !YIGEBAR_SRC_MODULE_H
