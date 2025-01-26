#ifndef _SCENE_LIGHT_HPP
#define _SCENE_LIGHT_HPP

struct Light {
    glm::vec4 color;
    float intensity;
    float brightness;

    Light(glm::vec4 color);

    ~Light();

    void set_color(glm::vec4 color);

    void set_intensity(float intensity);

    void set_brightness(float brightness);

    void render(const ShaderProgram* shader, const SceneObject* binding, int idx) const;
};

#endif