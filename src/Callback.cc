#include "Callback.h"

Callback::Callback(YigeBar *bar, YigeBar::MODULE_POSITION pos, uint16_t id, const YAML::Node &options)
    : bar_(bar)
    , pos_(pos)
    , id_(id), options_(options) {
}

void Callback::postUpdate() {
    bar_->update(pos_, id_);
}

const YAML::Node &Callback::getOptions() {
    return options_;
}
