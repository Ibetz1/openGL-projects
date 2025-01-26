#ifndef _WORLD_OBJECT_HPP
#define _WORLD_OBJECT_HPP

struct SceneObject {
    float pitch;
    float yaw;
    glm::vec3 facing;
    glm::vec3 position;
    glm::vec3 scale;

    SceneObject(
        glm::vec3 position = glm::vec3(0), 
        float pitch = 0.f, 
        float yaw = 0.f, 
        glm::vec3 scale = glm::vec3(1)
    );

    ~SceneObject();

    void set_orientation(float pitch, float yaw);

    void set_position(glm::vec3 position);

    glm::mat4 get_model_matrix() const;
};

#endif