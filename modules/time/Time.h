#ifndef YIGEBAR_MODULES_TIME_H
#define YIGEBAR_MODULES_TIME_H

#include <Module.h>

#include <thread>

class Time : public Module {
public:
    Time();

    virtual const std::string &getName() override;
    virtual std::string getContent() override;

    virtual void start(std::shared_ptr<Callback> cb) override;
    virtual void clickEvent(uint16_t button) override;

private:
    constexpr const static char *const default_time_ = "%H:%M";

    std::shared_ptr<Callback> cb_;

    bool is_alt_;

    std::string time_;
    std::string time_alt_;

    std::thread tic_thread_;

    void tic();
};

#endif // !YIGEBAR_MODULES_TIME_H
