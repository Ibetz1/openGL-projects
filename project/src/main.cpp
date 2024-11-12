#include "main.hpp"

//! TODO: textures
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    VertexArray VAO;
    VertexBuffer VBO;
    ElementBuffer EBO;

    static Mesh create(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
        Mesh mesh;

        mesh.vertices = vertices;
        mesh.indices = indices;

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

    static void destroy(Mesh* mesh) {
        if (!mesh) return;

        VertexArray::destroy(&mesh->VAO);
        VertexBuffer::destroy(&mesh->VBO);
        ElementBuffer::destroy(&mesh->EBO);

        mesh->vertices.clear();
        mesh->indices.clear();
    }

    //! TODO: camera
    static void draw(Mesh* mesh, ShaderProgram* shader) {
        if (!mesh) return;

        ShaderProgram::run(shader);
        VertexArray::bind(&mesh->VAO);
        
        // ??
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        VertexArray::unbind();
    }

};

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

    std::vector<Vertex> vertices = {
        { { 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
    };

    std::vector<GLuint> indices = {
        0, 1, 2
    };

    Mesh mesh = Mesh::create(vertices, indices);

    while (!glfwWindowShouldClose(window))
    {
        // Render background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        Mesh::draw(&mesh, &shader);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ShaderProgram::destroy(&shader);
    glfwTerminate();
    return 0;
}