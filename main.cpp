#include <iostream>
#include "common.h"
#include "dlfcn.h"

namespace {
void* handle = nullptr;

void UnloadLibrary() {
    if (handle != nullptr) {
        dlclose(handle);
    }
}
}

int main(int argc, char* argv[]) {
    std::cout << "Hello world!" << std::endl;
    AddNumbers(20);

    handle = dlopen("libplugin.dll", RTLD_LAZY|RTLD_LOCAL);

    std::cout << "Hello again!" << std::endl;
    AddNumbers(50);

    if (handle == nullptr) {
        std::cerr << "Err: " << dlerror() << std::endl;
    }

    atexit(UnloadLibrary);

    return 0;
}