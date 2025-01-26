#include "main.hpp"

Light::Light(glm::vec4 color) {
    this->color = color;
    this->intensity = 1.0f;
    this->brightness = 1.0f;
}

Light::~Light() {
    // TODO: remove from scene
}

void Light::set_color(glm::vec4 color) {
    this->color = color;
}

void Light::set_intensity(float intensity) {
    this->intensity = intensity;
}

void Light::set_brightness(float brightness) {
    this->brightness = brightness;
}

void Light::render(const ShaderProgram* shader, const SceneObject* binding, int idx) const {
    std::string shaderReference = "lights[" + std::to_string(idx) + "]";

    shader->set_uniform_4fv((shaderReference + ".Color").c_str(), glm::value_ptr(this->color));
    shader->set_uniform_3fv((shaderReference + ".Position").c_str(), glm::value_ptr(binding->position));
    shader->set_uniform_1f((shaderReference + ".Intensity").c_str(), this->intensity);
    shader->set_uniform_1f((shaderReference + ".Brightness").c_str(), this->brightness);
}