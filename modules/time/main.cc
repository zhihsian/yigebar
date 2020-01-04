#include "Time.h"

extern "C" {
Module *getInstance() {
    return new Time();
}
}
