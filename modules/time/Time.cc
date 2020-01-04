#include "Time.h"

#include <ctime>

#include <chrono>
#include <functional>
#include <sstream>
#include <thread>

#include <Callback.h>
#include <YigeBar.h>

Time::Time()
    : is_alt_(false) {
}

const std::string &Time::getName() {
    static const std::string name("time");
    return name;
}

std::string Time::getContent() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(localtime(&time),
                        is_alt_ ? time_alt_.c_str() : time_.c_str());

    return ss.str();
}

void Time::start(std::shared_ptr<Callback> cb) {
    cb_ = cb;

    auto options = cb_->getOptions();
    auto i = options["time"];
    if (!i) {
        time_ = default_time_;
    } else {
        time_ = i.as<std::string>();
    }

    i = options["time-alt"];
    if (!i) {
        time_alt_ = time_;
    } else {
        time_alt_ = i.as<std::string>();
    }

    tic_thread_ = std::thread(std::bind(&Time::tic, this));
}

void Time::tic() {
    for (;;) {
        cb_->postUpdate();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Time::clickEvent(uint16_t button) {
    is_alt_ = !is_alt_;
    cb_->postUpdate();
}
