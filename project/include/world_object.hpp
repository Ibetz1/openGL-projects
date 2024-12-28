#ifndef _WORLD_OBJECT_HPP
#define _WORLD_OBJECT_HPP

struct WorldObject {
    float pitch;
    float yaw;
    glm::vec3 facing;
    glm::vec3 position;
    glm::vec3 scale;

    static WorldObject create(glm::vec3 position = glm::vec3(0), float pitch = 0.f, float yaw = 0.f, glm::vec3 scale = glm::vec3(1));

    static void destroy(WorldObject* object);

    static void set_orientation(WorldObject* object, float pitch, float yaw);

    static void set_position(WorldObject* object, glm::vec3 position);

    static glm::mat4 get_model_matrix(const WorldObject* object);
};


#endif