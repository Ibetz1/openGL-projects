#ifndef _SHADER_HPP
#define _SHADER_HPP

struct ShaderProgram {
    static constexpr int max_shaders = 5;

    typedef enum {
        PRGM_UNLINKED,
        PRGM_LINKED,
    } ProgramStatus;

    GLuint id;
    GLuint shaders[max_shaders];
    int num_shaders;
    ProgramStatus status;

    // create a shader program
    ShaderProgram();

    // free up a shader program
    ~ShaderProgram();

    // add and compile a shader to a shader program via the file path
    void add(const char* path);

    // link a shader program for after shaders have been added
    void link();

    // use a compiled and linked shader program
    void activate() const;

    /*
        for setting uniforms
    */

    // set 1int unfirom
    void set_uniform_1i(const char* name, const GLint uni) const;

    // set a float uniform
    void set_uniform_1f(const char* name, const GLfloat data) const;

    // set 4fv vector uniform
    void set_uniform_4fv(const char* name, const GLfloat* uni) const;

    // set 3fv vector unfiform
    void set_uniform_3fv(const char* name, const GLfloat* uni) const;

    // set 4fv matrix uniform
    void set_uniform_matrix_4fv(const char* name, const GLfloat* uni) const;


};

#endif