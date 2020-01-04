#include "Text.h"

extern "C" {
Module *getInstance() {
    return new Text();
}
}
