#ifndef _GL_VERTEX_ARRAY_HPP
#define _GL_VERTEX_ARRAY_HPP

struct VertexArray {
    GLuint id;

    // vertex array attribute
    typedef struct {
        const int index;
        const int size;
        const GLenum type;
        const bool normalized;
        const int stride;
        const void* offset;
    } Attribute;

    // predefined vertex attributes
    static const Attribute vertex_position_attribute;
    static const Attribute vertex_normal_attribute;
    static const Attribute vertex_color_attribute;
    static const Attribute vertex_texUV_attribute;

    // create a vertex array
    static VertexArray create();

    // destroy a vertex array
    static void destroy(VertexArray* array);

    // bind as current vertex array
    static void bind(const VertexArray* array);

    // unbind as current vertex array
    static void unbind();

    // add attribute to vertex array
    static void add_attribute(const VertexArray* array, const Attribute attribute);
};

#endif