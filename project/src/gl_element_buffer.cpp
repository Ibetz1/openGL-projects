#include "main.hpp"

ElementBuffer::ElementBuffer() {}

ElementBuffer::~ElementBuffer() {
    glDeleteBuffers(1, &this->id);
}

void ElementBuffer::init(const std::vector<GLuint>& data) {
    glGenBuffers(1, &this->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(GLuint), data.data(), GL_STATIC_DRAW);
}

void ElementBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void ElementBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
} 