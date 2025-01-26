#include "main.hpp"

// creates a mesh
Mesh::Mesh(
    const std::vector<Vertex>& vertices, 
    const std::vector<GLuint>& indices
) {
    this->vertices = vertices;
    this->indices = indices;

    this->VAO.init();
    this->VBO.init(vertices);
    this->EBO.init(indices);

    this->VAO.add_attribute(VertexArray::vertex_position_attribute);
    this->VAO.add_attribute(VertexArray::vertex_normal_attribute);
    this->VAO.add_attribute(VertexArray::vertex_color_attribute);
    this->VAO.add_attribute(VertexArray::vertex_texUV_attribute);

    this->VAO.unbind();
    this->VBO.unbind();
    this->EBO.unbind();
}

// destroys a mesh
Mesh::~Mesh() {
    this->vertices.clear();
    this->indices.clear();
}

// draws mesh with shader
void Mesh::draw(const ShaderProgram* shader, const SceneObject* binding) {
    shader->set_uniform_matrix_4fv(
        "model", 
        (GLfloat*) glm::value_ptr(binding->get_model_matrix())
    );

    this->VAO.bind();

    // ??
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    this->VAO.unbind();

    Texture::unbind();
}