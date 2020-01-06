#ifndef YIGEBAR_MODULES_TEXT_TEXT_H
#define YIGEBAR_MODULES_TEXT_TEXT_H

#include <Module.h>

class Text : public Module {
public:
    virtual const std::string &getName() override;
    virtual std::string getContent() override;

    virtual void start(std::shared_ptr<Bridge> cb) override;
    virtual void clickEvent(uint16_t button) override;
private:
    std::shared_ptr<Bridge> cb_;

    std::string content_;
    std::string left_click_;
    std::string middle_click_;
    std::string right_click_;
};

#endif // !YIGEBAR_MODULES_TEXT_TEXT_H
