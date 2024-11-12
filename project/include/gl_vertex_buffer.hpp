#ifndef _GL_VERTEX_BUFFER_HPP
#define _GL_VERTEX_BUFFER_HPP

struct VertexBuffer {
    GLuint id;

    // create vertex buffer given an array of vertices
    static VertexBuffer create(std::vector<Vertex>& vertices);

    // destroy vertex buffer
    static void destroy(VertexBuffer* buffer);

    // bind as current vertex buffer
    static void bind(const VertexBuffer* buffer);

    // unbind as current vertex buffer
    static void unbind();
};

#endif