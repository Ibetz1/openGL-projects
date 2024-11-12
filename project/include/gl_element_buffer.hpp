#ifndef _GL_ELEMENT_BUFFER_HPP
#define _GL_ELEMENT_BUFFER_HPP

struct ElementBuffer {
    GLuint id;

    // create an element buffer from an array of indices
    static ElementBuffer create(std::vector<GLuint>& data);

    // destroy an element buffer
    static void destroy(ElementBuffer* buffer);

    // bind as current element buffer
    static void bind(const ElementBuffer* buffer);

    // unbind current element buffer
    static void unbind();
};

#endif