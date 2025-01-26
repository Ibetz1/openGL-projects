#include "main.hpp"

// creates texture
Texture::Texture() {
    this->state = TEXST_UNLOADED;
}

Texture::Texture(const char* file, TextureType type, GLuint tex_idx) {
    this->state = TEXST_UNLOADED;

    this->tex_unit = tex_idx;
    load(file, type);
}

// destroys texture
Texture::~Texture() {
    unbind();
    glDeleteTextures(1, &this->id);
}

// load texture into class
void Texture::load(const char* file, TextureType type) {
    if (this->state == TEXST_LOADED) {
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
    glActiveTexture(this->tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, colmod, iw, ih, 0, colmod, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

    this->id = texture;
    this->state = TEXST_LOADED;
    this->type = type;

    LOGI("texture loaded %s", file);
}

// gets tex unit relatived to unit 0
void Texture::assign_unit(const ShaderProgram* shader, const char* uni) const {
    shader->set_uniform_1i(uni, this->tex_unit - GL_TEXTURE0);
}

// binds texture
void Texture::bind() const {
    if (this->state == TEXST_UNLOADED) {
        LOGE("cannot bind unloaded texture");
        return;
    }

    glActiveTexture(this->tex_unit);
    glBindTexture(GL_TEXTURE_2D, this->id);
}

// unbinds all textures
void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}