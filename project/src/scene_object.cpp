#include "main.hpp"

SceneObject::SceneObject(glm::vec3 position, float pitch, float yaw, glm::vec3 scale) {
    this->facing = glm::vec3(0);
    this->position = position;
    this->scale = scale;
    set_orientation(pitch, yaw);
}

SceneObject::~SceneObject() {}

void SceneObject::set_orientation(float pitch, float yaw) {
    this->pitch = pitch;
    this->yaw = yaw;

    this->facing = glm::normalize(glm::vec3(
        cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
        sin(glm::radians(this->pitch)),
        sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
    ));
}

void SceneObject::set_position(glm::vec3 position) {
    this->position = position;
}

glm::mat4 SceneObject::get_model_matrix() const {
    glm::mat4 model = glm::mat4(1);

    model = glm::translate(model, this->position);
    model = glm::rotate(model, glm::radians(this->yaw), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(this->pitch), glm::vec3(1, 0, 0));
    model = glm::scale(model, this->scale);

    return model;
}
