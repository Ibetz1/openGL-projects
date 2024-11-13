#ifndef _GL_CAMERA_HPP
#define _GL_CAMERA_HPP

struct Camera {
    WorldObject* object_binding;
    glm::vec3 up;

    float cam_speed;
    float cam_sense;
    float fov;
    float near_plane;
    float far_plane;

    GLFWwindow* window;

    // creates camera instance bound to world object
    static Camera create(GLFWwindow* window, WorldObject* object_binding);

    // destroys camera instance
    static void destroy(Camera* camera);
    
    // binds camera to window
    static void bind(Camera* camera);

    // unbinds camera from window
    static void unbind(Camera* camera);

    // returns perspective matrix
    static glm::mat4 get_perspective(Camera* camera);

    // passes viewport to a shader
    static void apply_viewport(Camera* camera, ShaderProgram* shader, const char* unfiorm);

    // applies camera movement "WASD"
    static void apply_inputs(Camera* camera, float dt);

    // applies camera rotation via mouse movement
    static void apply_mouse(Camera* camera, double xpos, double ypos);
};

#endif