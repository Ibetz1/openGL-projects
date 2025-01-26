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
    VertexArray();

    // destroy a vertex array
    ~VertexArray();

    // secondary intialization
    void init();

    // bind as current vertex array
    void bind() const;

    // unbind as current vertex array
    void unbind() const;

    // add attribute to vertex array
    void add_attribute(const Attribute attribute) const;
};

#endif