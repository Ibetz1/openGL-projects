#include "main.hpp"

float sw = 800.f;
float sh = 800.f;

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// binds to camera
void mouse_callback_wrapper(GLFWwindow* window, double xpos, double ypos) {
    Camera* camera = (Camera*) (glfwGetWindowUserPointer(window));
    if (camera) {
        Camera::apply_mouse(camera, xpos, ypos);
    }
}

// Variable to store the previous key state
void handle_cursor_lock(GLFWwindow* window) {
    static bool cursor_locked = true;
    static bool ctrl_was_pressed = false;

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

    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // setup camera
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback_wrapper);

    // define world objects
    WorldObject camera_object = WorldObject::create();
    WorldObject mesh_object = WorldObject::create();
    WorldObject mesh_object1 = WorldObject::create(glm::vec3(3, 0, 0));

    // create a shader program
    ShaderProgram shader = ShaderProgram::create();
    ShaderProgram::add(&shader, "shaders/triangle.frag");
    ShaderProgram::add(&shader, "shaders/triangle.vert");
    ShaderProgram::link(&shader);
    
    // load a texture
    Texture texture = Texture::create("assets/tex.png", Texture::TTYPE_TEXTURE);
    
    // load a mesh
    Mesh mesh = Mesh::create(vertices, indices, { texture });
    
    // define camera
    Camera camera = Camera::create(window, &camera_object);
    Camera::bind(&camera);

    double prevtime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double curtime = glfwGetTime();
        double dt = curtime - prevtime;
        prevtime = curtime;

        handle_cursor_lock(window);

        // Render background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1, 0.1, 0.1, 1.0);

        Camera::apply_inputs(&camera, dt);
        Camera::apply_viewport(&camera, &shader, "view");

        Mesh::draw(&mesh, &shader, &mesh_object);
        Mesh::draw(&mesh, &shader, &mesh_object1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ShaderProgram::destroy(&shader);
    Mesh::destroy(&mesh);
    Texture::destroy(&texture);

    glfwTerminate();
    return 0;
}