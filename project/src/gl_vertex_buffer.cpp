#include "main.hpp"

VertexBuffer::VertexBuffer() {}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &this->id);
}

void VertexBuffer::init(const std::vector<Vertex>& vertices) {
    glGenBuffers(1, &this->id);
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}