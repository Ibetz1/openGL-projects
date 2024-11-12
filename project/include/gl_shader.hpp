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
    static ShaderProgram create();

    // free up a shader program
    static void destroy(ShaderProgram* program);

    // add and compile a shader to a shader program via the file path
    static void add(ShaderProgram* program, const char* path);

    // link a shader program for after shaders have been added
    static void link(ShaderProgram* program);

    // use a compiled and linked shader program
    static void activate(const ShaderProgram* program);

    /*
        for setting uniforms
    */

    // set 1int unfirom
    static void set_uniform_1i(const ShaderProgram* program, const char* name, GLint uni);

    // set 4fv uniform
    static void set_uniform_maxtrix_4fv(const ShaderProgram* progam, const char* name, GLfloat* uni);
};

#endif