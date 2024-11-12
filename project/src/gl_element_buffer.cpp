#include "main.hpp"

ElementBuffer ElementBuffer::create(std::vector<GLuint>& data) {
    ElementBuffer buffer = { 0 };
    glGenBuffers(1, &buffer.id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(GLuint), data.data(), GL_STATIC_DRAW);
    return buffer;
}

void ElementBuffer::destroy(ElementBuffer* buffer) {
    if (!buffer) return;

    glDeleteBuffers(1, &buffer->id);
    *buffer = { 0 };
}

void ElementBuffer::bind(const ElementBuffer* buffer) {
    if (!buffer) return;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

void ElementBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
} 