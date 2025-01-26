#ifndef _GL_CAMERA_HPP
#define _GL_CAMERA_HPP

struct Camera {
    SceneObject* object_binding;
    glm::vec3 up;

    float cam_speed;
    float cam_sense;
    float fov;
    float near_plane;
    float far_plane;

    GLFWwindow* window;

    // creates camera instance bound to world object
    Camera(GLFWwindow* window, SceneObject* object_binding);

    // destroys camera instance
    ~Camera();
    
    // binds camera to window
    void bind();

    // unbinds camera from window
    void unbind() const;

    // returns perspective matrix
    glm::mat4 get_perspective() const;

    // passes viewport to a shader
    void apply_viewport(const ShaderProgram* shader, const char* uniform) const;

    // passed position to a shader
    void apply_position(const ShaderProgram* shader, const char* uniform) const;

    // applies camera movement "WASD"
    void apply_inputs(float dt) const;

    // applies camera rotation via mouse movement
    void apply_mouse(double xpos, double ypos) const;
};

#endif