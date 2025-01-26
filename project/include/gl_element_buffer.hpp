#ifndef _GL_ELEMENT_BUFFER_HPP
#define _GL_ELEMENT_BUFFER_HPP

struct ElementBuffer {
    GLuint id;

    // create an element buffer from an array of indices
    ElementBuffer();

    // destroy an element buffer
    ~ElementBuffer();

    // init the element buffer object within openGL
    void init(const std::vector<GLuint>& data);

    // bind as current element buffer
    void bind();

    // unbind current element buffer
    void unbind();
};

#endif