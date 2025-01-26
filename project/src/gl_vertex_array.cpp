#include "main.hpp"

VertexArray::VertexArray() {}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &this->id);
}

void VertexArray::init() {
    glGenVertexArrays(1, &this->id);
    glBindVertexArray(this->id);
}

void VertexArray::bind() const {
    glBindVertexArray(this->id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::add_attribute(const Attribute attribute) const {
    glVertexAttribPointer(
        attribute.index, 
        attribute.size, 
        attribute.type, 
        attribute.normalized ? GL_TRUE : GL_FALSE, 
        attribute.stride, 
        attribute.offset
    );
    glEnableVertexAttribArray(attribute.index);
}

const VertexArray::Attribute VertexArray::vertex_position_attribute = { 
    0, 3, GL_FLOAT, false, sizeof(Vertex), (void*) (0 * sizeof(float))
};

const VertexArray::Attribute VertexArray::vertex_normal_attribute = { 
    1, 3, GL_FLOAT, true, sizeof(Vertex), (void*) (3 * sizeof(float))
};

const VertexArray::Attribute VertexArray::vertex_color_attribute = { 
    2, 4, GL_FLOAT, true, sizeof(Vertex), (void*) (6 * sizeof(float))
};

const VertexArray::Attribute VertexArray::vertex_texUV_attribute = {
    3, 2, GL_FLOAT, true, sizeof(Vertex), (void*) (10 * sizeof(float))
};