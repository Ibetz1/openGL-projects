#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <iostream>
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "sys/stat.h"

#include <stdio.h>
#include "glad/glad.h"
#include "GL/gl.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#define type_alloc(T) (T*) malloc(sizeof(T));
#define check_alloc(ptr) ({ if (!ptr) LOGE("allocation failed, non mem"); ptr; })
#define LOGI(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)

#include "shader.hpp"

#endif
