#include "main.hpp"

// creates a mesh
Mesh Mesh::create(
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

    mesh.texture_tile_count = 1.f;

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
void Mesh::draw(Mesh* mesh, ShaderProgram* shader, WorldObject* object) {
    if (!mesh) return;

    ShaderProgram::set_uniform_matrix_4fv(
        shader, 
        "model", 
        (GLfloat*) glm::value_ptr(WorldObject::get_model_matrix(object)
    ));

    ShaderProgram::set_uniform_1f(
        shader,
        "texTileCount",
        mesh->texture_tile_count
    );

    int tex_counts[Texture::TTYPE_MAX] = {
        [Texture::TTYPE_TEXTURE] = 0,
        [Texture::TTYPE_SPECULAR] = 0,
        [Texture::TTYPE_DIFFUSE] = 0,
        [Texture::TTYPE_NORMAL] = 0,
    };

    // bind mesh textures
    for (size_t i = 0; i < mesh->textures.size(); i++) {
        const Texture& tex = mesh->textures[i];

        const char* type_str = std::string(
            Texture::type_str[tex.type] + 
            std::to_string(tex_counts[tex.type])
        ).c_str();

        tex_counts[tex.type]++;

        Texture::assign_unit(&tex, shader, type_str);

        Texture::bind(&tex);
    }

    VertexArray::bind(&mesh->VAO);

    // ??
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

    VertexArray::unbind();

    Texture::unbind();
}