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

    GLuint id;
    TextureState state;
    TextureType type;

    // creates texture
    static Texture create();

    // create and load
    static Texture create(const char* file, TextureType type);

    // destroys texture
    static void destroy(Texture* tex);

    // load texture into class
    static void load(Texture* tex, const char* file, TextureType type);

    // binds texture
    static void bind(const Texture* tex);

    // unbinds all textures
    static void unbind();
};

#endif