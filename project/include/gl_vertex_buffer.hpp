#ifndef _GL_VERTEX_BUFFER_HPP
#define _GL_VERTEX_BUFFER_HPP

struct VertexBuffer {
    GLuint id;

    // create vertex buffer given an array of vertices
    VertexBuffer();

    // destroy vertex buffer
    ~VertexBuffer();

    // generate vertex buffer
    void init(const std::vector<Vertex>& vertice);

    // bind as current vertex buffer
    void bind() const;

    // unbind as current vertex buffer
    void unbind() const;
};

#endif