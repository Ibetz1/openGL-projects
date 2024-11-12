#include "main.hpp"

VertexArray VertexArray::create() {
    VertexArray array = { 0 };

    glGenVertexArrays(1, &array.id);
    glBindVertexArray(array.id);

    return array;
}

void VertexArray::destroy(VertexArray* array) {
    if (!array) return;

    glDeleteVertexArrays(1, &array->id);
    *array = { 0 };
}

void VertexArray::bind(const VertexArray* array) {
    if (!array) return;

    glBindVertexArray(array->id);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::add_attribute(const VertexArray* array, const Attribute attribute) {
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
    2, 3, GL_FLOAT, true, sizeof(Vertex), (void*) (6 * sizeof(float))
};

const VertexArray::Attribute VertexArray::vertex_texUV_attribute = {
    3, 2, GL_FLOAT, true, sizeof(Vertex), (void*) (9 * sizeof(float))
};