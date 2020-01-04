#include <glog/logging.h>

#include "YigeBar.h"

int main(int argc, const char *argv[]) {
    google::InitGoogleLogging(argv[0]);

    YigeBar app;
    app.init();
    app.show();

    google::ShutdownGoogleLogging();

    return 0;
}
