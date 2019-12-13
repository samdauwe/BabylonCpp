#ifndef BABYLONCPP_GLAD_CALLBACKS_H
#define BABYLONCPP_GLAD_CALLBACKS_H

#include <glad/glad.h>

void glad_post_call_callback(const char* name, void* /*funcptr*/,
                             int /*len_args*/, ...);

void glad_pre_call_callback(const char* name, void* /*funcptr*/,
                            int /*len_args*/, ...);

#endif // BABYLONCPP_GLAD_CALLBACKS_H
