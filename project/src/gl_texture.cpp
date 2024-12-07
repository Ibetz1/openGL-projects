#include "main.hpp"

// creates texture
Texture Texture::create() {
    Texture tex = { 0 };
    
    tex.state = TEXST_UNLOADED;

    return tex;
}

Texture Texture::create(const char* file, TextureType type, GLuint tex_idx) {
    Texture tex = create();
    tex.tex_unit = tex_idx;

    load(&tex, file, type);
    return tex;
}

// destroys texture
void Texture::destroy(Texture* tex) {
    if (!tex) return;

    // delete texture
    unbind();
    glDeleteTextures(1, &tex->id);

    *tex = { 0 };
}

// load texture into class
void Texture::load(Texture* tex, const char* file, TextureType type) {
    if (!tex) return;

    if (tex->state == TEXST_LOADED) {
        LOGE("texture object already loaded");
        return;
    }

    // get color mode
    GLenum colmod;
    if (strstr(file, ".png")) {
        colmod = GL_RGBA;
    } else if (strstr(file, ".jpg")) {
        colmod = GL_RGB;
    } else {
        LOGE("invalid file type");
        return;
    }

    int iw, ih, colCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(file, &iw, &ih, &colCh, 0);

    if (!bytes) {
        LOGE("failed to load image file");
        return;
    }

    // load the texture
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(tex->tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, colmod, iw, ih, 0, colmod, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

    tex->id = texture;
    tex->state = TEXST_LOADED;
    tex->type = type;

    LOGI("texture loaded %s", file);
}

// gets tex unit relatived to unit 0
void Texture::assign_unit(const Texture* tex, const ShaderProgram* shader, const char* uni) {
    ShaderProgram::set_uniform_1i(shader, uni, tex->tex_unit - GL_TEXTURE0);
}

// binds texture
void Texture::bind(const Texture* tex) {
    if (!tex) return;

    if (tex->state == TEXST_UNLOADED) {
        LOGE("cannot bind unloaded texture");
        return;
    }

    glActiveTexture(tex->tex_unit);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

// unbinds all textures
void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}