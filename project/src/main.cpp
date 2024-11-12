#include "main.hpp"

float sw = 800.f;
float sh = 800.f;

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
    static Texture create() {
        Texture tex = { 0 };
        
        tex.state = TEXST_UNLOADED;

        return tex;
    }

    static Texture create(const char* file, TextureType type) {
        Texture tex = create();
        load(&tex, file, type);
        return tex;
    }

    // destroys texture
    static void destroy(Texture* tex) {
        if (!tex) return;

        // delete texture
        glDeleteTextures(1, &tex->id);

        *tex = { 0 };
    }

    // load texture into class
    static void load(Texture* tex, const char* file, TextureType type) {
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
        unsigned char* bytes = stbi_load("assets/tex.png", &iw, &ih, &colCh, 0);

        if (!bytes) {
            LOGE("failed to load image file");
            return;
        }

        // load the texture
        GLuint texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
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

    // binds texture
    static void bind(const Texture* tex) {
        if (!tex) return;

        if (tex->state == TEXST_UNLOADED) {
            LOGE("cannot bind unloaded texture");
            return;
        }

        glBindTexture(GL_TEXTURE_2D, tex->id);
    }

    // unbinds all textures
    static void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    VertexArray VAO;
    VertexBuffer VBO;
    ElementBuffer EBO;

    // creates a mesh
    static Mesh create(
        const std::vector<Vertex>& vertices, 
        const std::vector<GLuint>& indices,
        const std::vector<Texture>& textures
    ) {
        Mesh mesh;

        mesh.vertices = vertices;
        mesh.indices = indices;
        mesh.textures = textures;

        mesh.VAO = VertexArray::create();
        mesh.VBO = VertexBuffer::create(mesh.vertices);
        mesh.EBO = ElementBuffer::create(mesh.indices);

        VertexArray::add_attribute(&mesh.VAO, VertexArray::vertex_position_attribute);
        VertexArray::add_attribute(&mesh.VAO, VertexArray::vertex_normal_attribute);
        VertexArray::add_attribute(&mesh.VAO, VertexArray::vertex_color_attribute);
        VertexArray::add_attribute(&mesh.VAO, VertexArray::vertex_texUV_attribute);

        VertexArray::unbind();
        VertexBuffer::unbind();
        ElementBuffer::unbind();

        return mesh;
    }

    // destroys a mesh
    static void destroy(Mesh* mesh) {
        if (!mesh) return;

        VertexArray::destroy(&mesh->VAO);
        VertexBuffer::destroy(&mesh->VBO);
        ElementBuffer::destroy(&mesh->EBO);

        mesh->vertices.clear();
        mesh->indices.clear();
    }

    // draws mesh with shader
    static void draw(Mesh* mesh, ShaderProgram* shader) {
        if (!mesh) return;

        ShaderProgram::activate(shader);

        int tex_counts[Texture::TTYPE_MAX] = {
            [Texture::TTYPE_TEXTURE] = 0,
            [Texture::TTYPE_SPECULAR] = 0,
            [Texture::TTYPE_DIFFUSE] = 0,
            [Texture::TTYPE_NORMAL] = 0,
        };

        // bind mesh textures
        //! TODO: offload to build time
        for (size_t i = 0; i < mesh->textures.size(); i++) {
            const Texture& tex = mesh->textures[i];

            const char* type_str = std::string(
                Texture::type_str[tex.type] + 
                tex_counts[tex.type]
            ).c_str();

            tex_counts[tex.type]++;

            ShaderProgram::set_uniform_1i(shader, type_str, i);
            Texture::bind(&tex);
        }

        VertexArray::bind(&mesh->VAO);
        
        // ??
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

        VertexArray::unbind();
    }
};

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(sw, sh, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOGE("glad init failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }


    ShaderProgram shader = ShaderProgram::create();
    ShaderProgram::add(&shader, "shaders/triangle.frag");
    ShaderProgram::add(&shader, "shaders/triangle.vert");
    ShaderProgram::link(&shader);

    Texture texture = Texture::create("assets/tex.png", Texture::TTYPE_TEXTURE);

    std::vector<Vertex> vertices = {
        // Front face
        {{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top right
        {{-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top left
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
        {{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom right

        // Back face
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top right
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top left
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom right

        // Left face
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top front
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top back
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom back
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom front

        // Right face
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top front
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top back
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom back
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom front

        // Top face
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // front right
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // front left
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // back left
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // back right

        // Bottom face
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // front right
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // front left
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // back left
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}  // back right
    };

    std::vector<GLuint> indices = {
        // Front face
        0, 1, 2, 0, 2, 3,
        // Back face
        4, 5, 6, 4, 6, 7,
        // Left face
        8, 9, 10, 8, 10, 11,
        // Right face
        12, 13, 14, 12, 14, 15,
        // Top face
        16, 17, 18, 16, 18, 19,
        // Bottom face
        20, 21, 22, 20, 22, 23
    };

    Mesh mesh = Mesh::create(vertices, indices, {
        texture
    });

    float rotation = 0.0f;
    float rotspeed = 100.f;
    double prevtime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        // Render background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double curtime = glfwGetTime();
        double dt = curtime - prevtime;
        prevtime = curtime;

        rotation += rotspeed * dt;
        rotation = fmodf(rotation, 360.0f);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.5f, 0.5f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0, -0.5, -5.0f));
        projection = glm::perspective(glm::radians(45.f), sw / sh, 0.1f, 100.0f);

        ShaderProgram::set_uniform_maxtrix_4fv(&shader, "model", glm::value_ptr(model));
        ShaderProgram::set_uniform_maxtrix_4fv(&shader, "view", glm::value_ptr(view));
        ShaderProgram::set_uniform_maxtrix_4fv(&shader, "projection", glm::value_ptr(projection));

        // Draw the triangle
        Mesh::draw(&mesh, &shader);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ShaderProgram::destroy(&shader);
    Mesh::destroy(&mesh);
    Texture::destroy(&texture);

    glfwTerminate();
    return 0;
}