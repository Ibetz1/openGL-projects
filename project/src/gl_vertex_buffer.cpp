#include "main.hpp"

VertexBuffer VertexBuffer::create(std::vector<Vertex>& vertices) {
    VertexBuffer buffer = { 0 };
    glGenBuffers(1, &buffer.id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    return buffer;
}

void VertexBuffer::destroy(VertexBuffer* buffer) {
    if (!buffer) return;

    glDeleteBuffers(1, &buffer->id);
    *buffer = { 0 };
}

void VertexBuffer::bind(const VertexBuffer* buffer) {
    if (!buffer) return;

    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}