#include "main.hpp"

/*
    create a shader program
*/
ShaderProgram ShaderProgram::create() {
    ShaderProgram program = { 0 };

    program.num_shaders = 0;
    program.id = glCreateProgram();
    program.status = PRGM_UNLINKED;

    return program;
}

/*
    free up a shader program
*/
void ShaderProgram::destroy(ShaderProgram* program) {
    if (!program) return;

    for (int i = 0; i < program->num_shaders; i++) {
        glDeleteShader(program->shaders[i]);
    }

    if (program->id) {
        glDeleteProgram(program->id);
    }

}

/*
    add and compile a shader to a shader program via the file path
*/
void ShaderProgram::add(ShaderProgram* program, const char* path) {

    // check program state
    if (program->status == PRGM_LINKED) {
        LOGE("add shader failed: bad state");
        return;
    }

    // assert program can take a shader
    if (program->num_shaders >= ShaderProgram::max_shaders) {
        LOGE("program reached max shaders");
        return;
    }

    // get shader type
    GLint shader_type;
    if (strstr(path, ".vert")) {
        shader_type = GL_VERTEX_SHADER;
    } else if (strstr(path, ".frag")) {
        shader_type = GL_FRAGMENT_SHADER;
    } else {
        LOGE("failed to find shader type for %s", path);
        return;
    }

    // get shader source
    FILE* file = fopen(path, "rb+");
    if (!file) {
        LOGE("failed to open file %s", path);
        return;
    }

    // get file content
    struct stat file_stats;
    
    if (fstat(fileno(file), &file_stats) != 0) {
        LOGE("failed to get file stats: %s", path);
        fclose(file);
        return;
    }

    long fsize = file_stats.st_size;
    char* buf = (char*) malloc(fsize);

    if (!check_alloc(buf)) {
        fclose(file);
        return;
    }

    fread(buf, 1, fsize, file);
    buf[fsize] = '\0';
    fclose(file);

    // add file content to shader
    GLint* shader_id = &program->shaders[program->num_shaders];
    *shader_id = glCreateShader(shader_type);
    glShaderSource(*shader_id, 1, &buf, nullptr);

    free(buf);

    // compile shader
    glCompileShader(*shader_id);
    GLint ret;
    glGetShaderiv(*shader_id, GL_COMPILE_STATUS,&ret);
    if (!ret) {
        char buf[512];
        glGetShaderInfoLog(*shader_id, sizeof(buf), nullptr, buf);
        LOGE("failed to compile shader %s", buf);
        glDeleteShader(*shader_id);
        *shader_id = 0;
        return;
    }

    LOGI("added shader %s", path);
    glAttachShader(program->id, *shader_id);        
    program->num_shaders++;
}

/*
    link a shader program for after shaders have been added
*/
void ShaderProgram::link(ShaderProgram* program) {
    // check program state
    if (program->status == PRGM_LINKED) {
        LOGE("link program failed: bad state");
        return;
    }

    if (program->num_shaders == 0) {
        LOGE("link program failed: no shaders");
        return;
    }

    glLinkProgram(program->id);
    GLint ret;
    glGetProgramiv(program->id, GL_LINK_STATUS, &ret);
    if (!ret) {
        char buf[512];
        glGetProgramInfoLog(program->id, sizeof(buf), nullptr, buf);
        LOGE("failed to link shader program %s", buf);
        return;
    }

    program->status = PRGM_LINKED;
}

/*
    use a compiled and linked shader program
*/
void ShaderProgram::run(const ShaderProgram* program) {
    if (program->status == PRGM_UNLINKED) {
        LOGE("run program failed: bad state");
        return;
    }

    glUseProgram(program->id);
}