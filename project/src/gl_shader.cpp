#include "main.hpp"

ShaderProgram::ShaderProgram() {
    this->num_shaders = 0;
    this->id = glCreateProgram();
    this->status = PRGM_UNLINKED;
}

ShaderProgram::~ShaderProgram() {
    for (int i = 0; i < this->num_shaders; i++) {
        glDeleteShader(this->shaders[i]);
    }

    if (this->id) {
        glDeleteProgram(this->id);
    }
}

void ShaderProgram::add(const char* path) {
    // check program state
    if (this->status == PRGM_LINKED) {
        LOGE("add shader failed: bad state");
        return;
    }

    // assert program can take a shader
    if (this->num_shaders >= ShaderProgram::max_shaders) {
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
    GLuint* shader_id = &this->shaders[this->num_shaders];
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
    glAttachShader(this->id, *shader_id);        
    this->num_shaders++;
}

void ShaderProgram::link() {
    // check program state
    if (this->status == PRGM_LINKED) {
        LOGE("link program failed: bad state");
        return;
    }

    if (this->num_shaders == 0) {
        LOGE("link program failed: no shaders");
        return;
    }

    glLinkProgram(this->id);
    GLint ret;
    glGetProgramiv(this->id, GL_LINK_STATUS, &ret);
    if (!ret) {
        char buf[512];
        glGetProgramInfoLog(this->id, sizeof(buf), nullptr, buf);
        LOGE("failed to link shader program %s", buf);
        return;
    }

    this->status = PRGM_LINKED;
}

void ShaderProgram::activate() const {
    if (this->status == PRGM_UNLINKED) {
        LOGE("run program failed: bad state");
        return;
    }

    glUseProgram(this->id);
}

/*
    setting uniforms
*/

void ShaderProgram::set_uniform_1i(const char* name, const GLint data) const {
    GLuint uniID = glGetUniformLocation(this->id, name);
    glUniform1i(uniID, data);
}

void ShaderProgram::set_uniform_1f(const char* name, const GLfloat data) const {
    GLuint uniID = glGetUniformLocation(this->id, name);
    glUniform1f(uniID, data);
}

void ShaderProgram::set_uniform_3fv(const char* name, const GLfloat* data) const {
    GLuint uniID = glGetUniformLocation(this->id, name);
    glUniform3fv(uniID, 1, data);
}

void ShaderProgram::set_uniform_4fv(const char* name, const GLfloat* data) const {
    GLuint uniID = glGetUniformLocation(this->id, name);
    glUniform4fv(uniID, 1, data);
}

void ShaderProgram::set_uniform_matrix_4fv(const char* name, const GLfloat* data) const {
    GLuint uniID = glGetUniformLocation(this->id, name);
    glUniformMatrix4fv(uniID, 1, GL_FALSE, data);
}

