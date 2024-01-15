#include <iostream>
#include "libplugin.h"
#include "common.h"

class plugin {
public:
    plugin() {
        std::cout << "Hello plugin!" << std::endl;
        AddNumbers(100);
    }
};

plugin t;