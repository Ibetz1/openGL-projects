#include "main.hpp"

float sw = 800.f;
float sh = 800.f;

struct Camera {
    glm::vec3 position;
    glm::vec3 orientation;
    glm::vec3 up;

    float pitch = 0.0f;
    float yaw = 0.0f;

    float cam_speed;
    float cam_sense;
    float fov;
    float near_plane;
    float far_plane;

    static Camera create() {
        Camera camera = {  };
        camera.position = glm::vec3(0);
        camera.orientation = glm::vec3(0.f, 0.f, -1.0f);
        camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
        
        camera.cam_speed = 10.f;
        camera.cam_sense = 0.1f;
        camera.fov = 45.f;
        camera.near_plane = 0.1f;
        camera.far_plane = 100.f;

        return camera;
    }

    static void destroy(Camera* camera) {
        if (!camera) return;

        *camera = {  };
    }

    static void pass_matrix(const Camera* camera, GLFWwindow* window, const ShaderProgram* shader, const char* vpuni) {
        if (!camera) return;
        if (!shader) return;
        if (!window) return;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        
        view = glm::lookAt(camera->position, camera->position + camera->orientation, camera->up);

        projection = glm::perspective(
            glm::radians(camera->fov), 
            (float) width / (float) height, 
            camera->near_plane, 
            camera->far_plane
        );

        ShaderProgram::set_uniform_maxtrix_4fv(shader, vpuni, (GLfloat*) glm::value_ptr(projection * view));
    }

    static void apply_inputs(Camera* camera, GLFWwindow* window, float dt) {
        if (!camera) return;
        if (!window) return;

        if (glfwGetKey(window, GLFW_KEY_W)) {
            camera->position += camera->cam_speed * dt * camera->orientation;
        }

        if (glfwGetKey(window, GLFW_KEY_A)) {
            camera->position -= camera->cam_speed * dt * glm::normalize(glm::cross(camera->orientation, camera->up));
        }

        if (glfwGetKey(window, GLFW_KEY_S)) {
            camera->position -= camera->cam_speed * dt * camera->orientation;
        }

        if (glfwGetKey(window, GLFW_KEY_D)) {
            camera->position += camera->cam_speed * dt * glm::normalize(glm::cross(camera->orientation, camera->up));
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE)) {
            camera->position.y += camera->cam_speed * dt;
        
        }

        if (glfwGetKey(window, GLFW_KEY_C)) {
            camera->position.y -= camera->cam_speed * dt;
        }
    }

    static void apply_mouse(Camera* camera, double xpos, double ypos) {
        static bool firstMouse = true;
        static float lastX = 0.0f;
        static float lastY = 0.0f;

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        xoffset *= camera->cam_sense;
        yoffset *= camera->cam_sense;

        camera->yaw += xoffset;
        camera->pitch += yoffset;

        // Clamp the pitch to avoid screen flip
        if (camera->pitch > 89.0f) camera->pitch = 89.0f;
        if (camera->pitch < -89.0f) camera->pitch = -89.0f;

        // Update orientation vector
        glm::vec3 newOrientation;
        newOrientation.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        newOrientation.y = sin(glm::radians(camera->pitch));
        newOrientation.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        camera->orientation = glm::normalize(newOrientation);
    }
};

void mouse_callback_wrapper(GLFWwindow* window, double xpos, double ypos) {
    Camera* camera = (Camera*) (glfwGetWindowUserPointer(window));
    if (camera) {
        Camera::apply_mouse(camera, xpos, ypos);
    }
}

// Variable to store the previous key state
bool cursor_locked = true;
bool ctrl_was_pressed = false;

void handle_cursor_lock(GLFWwindow* window) {
    int ctrl_state = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);

    // Check for a transition from pressed to released
    if (ctrl_state == GLFW_RELEASE && ctrl_was_pressed) {
        if (cursor_locked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        
        cursor_locked = !cursor_locked;
    }

    // Update the previous state
    ctrl_was_pressed = (ctrl_state == GLFW_PRESS);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(sw, sh, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOGE("glad init failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // setup camera
    Camera camera = Camera::create();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, mouse_callback_wrapper);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ShaderProgram shader = ShaderProgram::create();
    ShaderProgram::add(&shader, "shaders/triangle.frag");
    ShaderProgram::add(&shader, "shaders/triangle.vert");
    ShaderProgram::link(&shader);

    Texture texture = Texture::create("assets/tex.png", Texture::TTYPE_TEXTURE);

    std::vector<Vertex> vertices = {
        // Front face
        {{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top right
        {{-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top left
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
        {{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom right

        // Back face
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top right
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top left
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom right

        // Left face
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top front
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top back
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom back
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom front

        // Right face
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top front
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top back
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom back
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom front

        // Top face
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // front right
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // front left
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // back left
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // back right

        // Bottom face
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // front right
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // front left
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // back left
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}  // back right
    };

    std::vector<GLuint> indices = {
        // Front face
        0, 1, 2, 0, 2, 3,
        // Back face
        4, 5, 6, 4, 6, 7,
        // Left face
        8, 9, 10, 8, 10, 11,
        // Right face
        12, 13, 14, 12, 14, 15,
        // Top face
        16, 17, 18, 16, 18, 19,
        // Bottom face
        20, 21, 22, 20, 22, 23
    };

    Mesh mesh = Mesh::create(vertices, indices, {
        texture
    });

    float rotation = 0.0f;
    float rotspeed = 100.f;
    double prevtime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        handle_cursor_lock(window);

        // Render background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1, 0.1, 0.1, 1.0);

        double curtime = glfwGetTime();
        double dt = curtime - prevtime;
        prevtime = curtime;

        rotation += rotspeed * dt;
        rotation = fmodf(rotation, 360.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.5f, 0.5f, 0.0f));
        ShaderProgram::set_uniform_maxtrix_4fv(&shader, "model", glm::value_ptr(model));

        Camera::apply_inputs(&camera, window, dt);
        Camera::pass_matrix(&camera, window, &shader, "view");

        Mesh::draw(&mesh, &shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ShaderProgram::destroy(&shader);
    Mesh::destroy(&mesh);
    Texture::destroy(&texture);

    glfwTerminate();
    return 0;
}