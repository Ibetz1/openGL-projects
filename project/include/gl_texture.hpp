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
    static Texture create();

    // create and load
    static Texture create(const char* file, TextureType type, GLuint tex_idx);

    // destroys texture
    static void destroy(Texture* tex);

    // load texture into class
    static void load(Texture* tex, const char* file, TextureType type);

    // binds texture
    static void bind(const Texture* tex);

    // pass texture unit to shader
    static void assign_unit(const Texture* tex, const ShaderProgram* shader, const char* uni);

    // unbinds all textures
    static void unbind();
};

#endif