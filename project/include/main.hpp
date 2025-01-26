#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include <map>
#include <algorithm>
#include <unordered_map>

#include "glad/glad.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#define check_alloc(ptr) ({ if (!ptr) LOGE("allocation failed, non mem"); ptr; })
#define LOGI(fmt, ...) do { printf(fmt "\n", ##__VA_ARGS__); } while (0)
#define LOGE(fmt, ...) do { fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while (0)
#define THROW(fmt, ...) do { LOGE(__FILE__ " " fmt, ##__VA_ARGS__); exit(1); } while (0)
#define MIN(T, a, b) ({ T _a = a; T _b = b; (_a < _b) ? _a : _b; })
#define MAX(T, a, b) ({ T _a = a; T _b = b; (_a < _b) ? _b : _a; })

#include "gl_shader.hpp"

// game objects
#include "scene_object.hpp"
#include "scene_camera.hpp"
#include "scene_light.hpp"

// mesh
#include "gl_vertex.hpp"
#include "gl_vertex_buffer.hpp"
#include "gl_element_buffer.hpp"
#include "gl_vertex_array.hpp"
#include "gl_texture.hpp"
#include "gl_mesh.hpp"

#endif
