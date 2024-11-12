#ifndef _GL_MESH_HPP
#define _GL_MESH_HPP

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
    );

    // destroys a mesh
    static void destroy(Mesh* mesh);

    // draws mesh with shader
    static void draw(Mesh* mesh, ShaderProgram* shader);
};

#endif