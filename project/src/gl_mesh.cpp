#include "main.hpp"

// creates a mesh
Mesh Mesh::create(
    const std::vector<Vertex>& vertices, 
    const std::vector<GLuint>& indices
) {
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

// destroys a mesh
void Mesh::destroy(Mesh* mesh) {
    if (!mesh) return;

    VertexArray::destroy(&mesh->VAO);
    VertexBuffer::destroy(&mesh->VBO);
    ElementBuffer::destroy(&mesh->EBO);

    mesh->vertices.clear();
    mesh->indices.clear();
}

// draws mesh with shader
void Mesh::draw(const Mesh* mesh, const WorldObject* binding, const ShaderProgram* shader) {
    if (!mesh) return;

    ShaderProgram::set_uniform_matrix_4fv(
        shader, 
        "model", 
        (GLfloat*) glm::value_ptr(WorldObject::get_model_matrix(binding)
    ));

    VertexArray::bind(&mesh->VAO);

    // ??
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

    VertexArray::unbind();

    Texture::unbind();
}