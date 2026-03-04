//
//  Cpp_engine.cpp
//  Airplane
//
//  Created by sunt on 2026/3/4.
//

#include "Cpp_engine.h"

class Engine {
public:
    void start() {}
};

void* engine_create() {
    return new Engine();
}

void engine_start(void* engine) {
    static_cast<Engine*>(engine)->start();
}

void engine_destroy(void* engine) {
    delete static_cast<Engine*>(engine);
}
