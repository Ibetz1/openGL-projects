#ifndef _GL_TEXTURE_HPP
#define _GL_TEXTURE_HPP

struct Texture {
    typedef enum {
        TEXST_UNLOADED,
        TEXST_LOADED,
    } TextureState;

    typedef enum {
        TTYPE_TEXTURE,
        TTYPE_SPECULAR,
        TTYPE_DIFFUSE,
        TTYPE_NORMAL,
        TTYPE_MAX,
    } TextureType;

    static constexpr const char* type_str[TTYPE_MAX] = {
        [TTYPE_TEXTURE] = "texture",
        [TTYPE_SPECULAR] = "specular",
        [TTYPE_DIFFUSE] = "diffuse",
        [TTYPE_NORMAL] = "normal",
    };

    GLuint tex_unit;
    GLuint id;
    TextureState state;
    TextureType type;

    // creates texture
    Texture();

    // create and load
    Texture(const char* file, TextureType type, GLuint tex_idx);

    // destroys texture
    ~Texture();

    // load texture into class
    void load(const char* file, TextureType type);

    // binds texture
    void bind() const;

    // pass texture unit to shader
    void assign_unit(const ShaderProgram* shader, const char* uni) const;

    // unbinds all textures
    static void unbind();
};

#endif