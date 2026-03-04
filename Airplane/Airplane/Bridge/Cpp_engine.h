//
//  Cpp_engine.hpp
//  Airplane
//
//  Created by sunt on 2026/3/4.
//

#ifdef __cplusplus
extern "C" {
#endif

void* engine_create();
void engine_start(void* engine);
void engine_destroy(void* engine);

#ifdef __cplusplus
}
#endif
