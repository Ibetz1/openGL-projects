#ifndef _GL_MESH_HPP
#define _GL_MESH_HPP

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    VertexArray VAO;
    VertexBuffer VBO;
    ElementBuffer EBO;

    // creates a mesh
    Mesh(
        const std::vector<Vertex>& vertices, 
        const std::vector<GLuint>& indices
    );

    ~Mesh();

    // draws mesh with shader
    void draw(const ShaderProgram* shader, const SceneObject* binding);
};

#endif