#include "main.hpp"

WorldObject WorldObject::create(glm::vec3 position, float pitch, float yaw, glm::vec3 scale) {
    WorldObject object = { };

    object.facing = glm::vec3(0);
    object.position = position;
    object.scale = scale;
    set_orientation(&object, pitch, yaw);

    return object;
}

void WorldObject::destroy(WorldObject* object) {
    if (!object) return;

    *object = { };   
}

void WorldObject::set_orientation(WorldObject* object, float pitch, float yaw) {
    if (!object) return;

    object->pitch = pitch;
    object->yaw = yaw;

    object->facing = glm::normalize(glm::vec3(
        cos(glm::radians(object->yaw)) * cos(glm::radians(object->pitch)),
        sin(glm::radians(object->pitch)),
        sin(glm::radians(object->yaw)) * cos(glm::radians(object->pitch))
    ));
}

void WorldObject::set_position(WorldObject* object, glm::vec3 position) {
    if (!object) return;

    object->position = position;
}

glm::mat4 WorldObject::get_model_matrix(WorldObject* object) {
    glm::mat4 model = glm::mat4(1);

    if (!object) return model;

    model = glm::translate(model, object->position);
    model = glm::rotate(model, glm::radians(object->yaw), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(object->pitch), glm::vec3(1, 0, 0));
    model = glm::scale(model, object->scale);

    return model;
}
