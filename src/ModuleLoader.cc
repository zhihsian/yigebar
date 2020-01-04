#include "ModuleLoader.h"

#include <dlfcn.h>

#include <glog/logging.h>

ModuleLoader::~ModuleLoader() {
    for (auto &i : handles_) {
        dlclose(i);
    }
}

void ModuleLoader::setDirection(std::filesystem::path direction) {
    direction_ = std::move(direction);
}

std::shared_ptr<Module> ModuleLoader::load(std::string name) {
    factory_function func;

    auto i = loaded_.find(name);
    if (i == loaded_.end()) {
        name.append(".so");

        auto path = direction_;
        path.append(name);
        auto s = path.string();

        auto *handle = dlopen(path.string().c_str(), RTLD_LAZY);
        if (!handle) {
            LOG(FATAL) << "failed to load module: " << path.string();
        }

        func = reinterpret_cast<Module *(*)()>(dlsym(handle, "getInstance"));
        if (func == nullptr) {
            LOG(FATAL) << "cannot initial module: " << name;
        }

        handles_.insert(handle);
        loaded_.insert(std::make_pair(name, func));
    } else {
        func = i->second;
    }

    return std::shared_ptr<Module>(func());
}
