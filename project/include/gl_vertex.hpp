#ifndef _GL_VERTEX_HPP
#define _GL_VERTEX_HPP

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texUV;
};

#endif