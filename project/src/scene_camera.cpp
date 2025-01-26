#include "main.hpp"

Camera::Camera(GLFWwindow* window, SceneObject* object_binding) {
    if (!window) {
        THROW("camera missing window");
    }

    this->object_binding = object_binding;
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);

    this->cam_speed = 10.f;
    this->cam_sense = 0.1f;
    this->fov = 45.f;
    this->near_plane = 0.1f;
    this->far_plane = 100.f;

    this->window = window;
}

Camera::~Camera() {}

void Camera::bind() {
    LOGI("bound camera");

    glfwSetWindowUserPointer(this->window, this);
}

void Camera::unbind() const {
    LOGI("bound camera");

    glfwSetWindowUserPointer(this->window, nullptr);
}

glm::mat4 Camera::get_perspective() const {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    view = glm::lookAt(
        this->object_binding->position, 
        this->object_binding->position + this->object_binding->facing, 
        this->up
    );

    projection = glm::perspective(
        glm::radians(this->fov), 
        (float) width / (float) height, 
        this->near_plane, 
        this->far_plane
    );

    return projection * view;
}

void Camera::apply_viewport(const ShaderProgram* shader, const char* uniform) const {
    // apply camera perspective
    shader->set_uniform_matrix_4fv(
        uniform, 
        (GLfloat*) glm::value_ptr(get_perspective())
    );
}

void Camera::apply_position(const ShaderProgram* shader, const char* uniform) const {
    // apply camera perspective
    shader->set_uniform_3fv(
        uniform, 
        (GLfloat*) glm::value_ptr(this->object_binding->position)
    );
}

void Camera::apply_inputs(const float dt) const {
    SceneObject* object = this->object_binding;

    if (glfwGetKey(this->window, GLFW_KEY_W)) {
        object->position += this->cam_speed * dt * object->facing;
    }

    if (glfwGetKey(this->window, GLFW_KEY_A)) {
        object->position -= this->cam_speed * dt * glm::normalize(glm::cross(object->facing, this->up));
    }

    if (glfwGetKey(this->window, GLFW_KEY_S)) {
        object->position -= this->cam_speed * dt * object->facing;
    }

    if (glfwGetKey(this->window, GLFW_KEY_D)) {
        object->position += this->cam_speed * dt * glm::normalize(glm::cross(object->facing, this->up));
    }

    if (glfwGetKey(this->window, GLFW_KEY_SPACE)) {
        object->position.y += this->cam_speed * dt;
    }

    if (glfwGetKey(this->window, GLFW_KEY_C)) {
        object->position.y -= this->cam_speed * dt;
    }
}

void Camera::apply_mouse(const double xpos, double ypos) const {
    static bool firstMouse = true;
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= this->cam_sense;
    yoffset *= this->cam_sense;

    SceneObject* object = this->object_binding;

    float yaw = object->yaw + xoffset;
    float pitch = fminf(fmaxf(object->pitch + yoffset, -89.f), 89.f);

    object->set_orientation(pitch, yaw);
}