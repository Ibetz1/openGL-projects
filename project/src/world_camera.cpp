#include "main.hpp"

Camera Camera::create(GLFWwindow* window, WorldObject* object_binding) {
    Camera camera = {  };

    if (!window) return camera;

    camera.object_binding = object_binding;
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

    camera.cam_speed = 10.f;
    camera.cam_sense = 0.1f;
    camera.fov = 45.f;
    camera.near_plane = 0.1f;
    camera.far_plane = 100.f;

    camera.window = window;

    return camera;
}

void Camera::destroy(Camera* camera) {
    if (!camera) return;

    *camera = {  };
}

void Camera::bind(Camera* camera) {
    if (!camera) return;

    LOGI("bound camera");

    glfwSetWindowUserPointer(camera->window, camera);
}

void Camera::unbind(Camera* camera) {
    if (!camera) return;

    LOGI("bound camera");

    glfwSetWindowUserPointer(camera->window, nullptr);
}

glm::mat4 Camera::get_perspective(Camera* camera) {
    if (!camera) return {};

    int width, height;
    glfwGetFramebufferSize(camera->window, &width, &height);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    view = glm::lookAt(
        camera->object_binding->position, 
        camera->object_binding->position + camera->object_binding->facing, 
        camera->up
    );

    projection = glm::perspective(
        glm::radians(camera->fov), 
        (float) width / (float) height, 
        camera->near_plane, 
        camera->far_plane
    );

    return projection * view;
}

void Camera::apply_viewport(Camera* camera, ShaderProgram* shader, const char* unfiorm) {
    // apply camera perspective
    ShaderProgram::set_uniform_maxtrix_4fv(
        shader, 
        unfiorm, 
        (GLfloat*) glm::value_ptr(get_perspective(camera)
    ));
}

void Camera::apply_inputs(Camera* camera, float dt) {
    if (!camera) return;

    WorldObject* object = camera->object_binding;

    if (glfwGetKey(camera->window, GLFW_KEY_W)) {
        object->position += camera->cam_speed * dt * object->facing;
    }

    if (glfwGetKey(camera->window, GLFW_KEY_A)) {
        object->position -= camera->cam_speed * dt * glm::normalize(glm::cross(object->facing, camera->up));
    }

    if (glfwGetKey(camera->window, GLFW_KEY_S)) {
        object->position -= camera->cam_speed * dt * object->facing;
    }

    if (glfwGetKey(camera->window, GLFW_KEY_D)) {
        object->position += camera->cam_speed * dt * glm::normalize(glm::cross(object->facing, camera->up));
    }

    if (glfwGetKey(camera->window, GLFW_KEY_SPACE)) {
        object->position.y += camera->cam_speed * dt;
    }

    if (glfwGetKey(camera->window, GLFW_KEY_C)) {
        object->position.y -= camera->cam_speed * dt;
    }
}

void Camera::apply_mouse(Camera* camera, double xpos, double ypos) {
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

    xoffset *= camera->cam_sense;
    yoffset *= camera->cam_sense;

    WorldObject* object = camera->object_binding;

    float yaw = object->yaw + xoffset;
    float pitch = fminf(fmaxf(object->pitch + yoffset, -89.f), 89.f);

    WorldObject::set_orientation(object, pitch, yaw);
}