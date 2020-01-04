#ifndef YIGEBAR_SRC_MODULELOADER_H
#define YIGEBAR_SRC_MODULELOADER_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include <Module.h>

class ModuleLoader {
    using factory_function = Module *(*)();

public:
    ~ModuleLoader();

    void setDirection(std::filesystem::path direction);

    std::shared_ptr<Module> load(std::string name);

private:
    static constexpr char module_ext[] = ".so";

    std::filesystem::path direction_;

    std::unordered_set<void *> handles_;
    std::unordered_map<std::string, factory_function> loaded_;
};

#endif // !YIGEBAR_SRC_MODULELOADER_H
