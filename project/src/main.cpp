#include "main.hpp"

float sw = 800.f;
float sh = 800.f;

std::vector<Vertex> cubeVertices = {
    // Front face
    {{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // top right
    {{-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // top left
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // bottom right

    // Back face
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // top right
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // top left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, // bottom left
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // bottom right

    // Left face
    {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // top front
    {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // top back
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, // bottom back
    {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // bottom front

    // Right face
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // top front
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // top back
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, // bottom back
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // bottom front

    // Top face
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // front right
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // front left
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, // back left
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // back right

    // Bottom face
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // front right
    {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // front left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, // back left
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}  // back right
};

std::vector<Vertex> lightVertices = {
    // Front face
    {{ 0.1f,  0.1f, 0.1f}}, // top right
    {{-0.1f,  0.1f, 0.1f}}, // top left
    {{-0.1f, -0.1f, 0.1f}}, // bottom left
    {{ 0.1f, -0.1f, 0.1f}}, // bottom right

    // Back face
    {{ 0.1f,  0.1f, -0.1f}}, // top right
    {{-0.1f,  0.1f, -0.1f}}, // top left
    {{-0.1f, -0.1f, -0.1f}}, // bottom left
    {{ 0.1f, -0.1f, -0.1f}}, // bottom right

    // Left face
    {{-0.1f,  0.1f,  0.1f}}, // top front
    {{-0.1f,  0.1f, -0.1f}}, // top back
    {{-0.1f, -0.1f, -0.1f}}, // bottom back
    {{-0.1f, -0.1f,  0.1f}}, // bottom front

    // Right face
    {{ 0.1f,  0.1f,  0.1f}}, // top front
    {{ 0.1f,  0.1f, -0.1f}}, // top back
    {{ 0.1f, -0.1f, -0.1f}}, // bottom back
    {{ 0.1f, -0.1f,  0.1f}}, // bottom front

    // Top face
    {{ 0.1f,  0.1f,  0.1f}}, // front right
    {{-0.1f,  0.1f,  0.1f}}, // front left
    {{-0.1f,  0.1f, -0.1f}}, // back left
    {{ 0.1f,  0.1f, -0.1f}}, // back right

    // Bottom face
    {{ 0.1f, -0.1f,  0.1f}}, // front right
    {{-0.1f, -0.1f,  0.1f}}, // front left
    {{-0.1f, -0.1f, -0.1f}}, // back left
    {{ 0.1f, -0.1f, -0.1f}}  // back right
};

std::vector<GLuint> cubeIndicies = {
    // Front face
    0, 1, 2, 0, 2, 3,
    // Back face
    7, 6, 4, 6, 5, 4,
    // Left face
    8, 9, 10, 8, 10, 11,
    // Right face
    15, 14, 12, 14, 13, 12,
    // Top face
    19, 18, 16, 18, 17, 16,
    // Bottom face
    20, 21, 22, 20, 22, 23
};

std::vector<GLuint> basePlateIndices = {
    19, 18, 17, 19, 17, 16,
    16, 17, 19, 17, 18, 19,
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// binds to camera
void mouse_callback_wrapper(GLFWwindow* window, double xpos, double ypos) {
    Camera* camera = (Camera*) (glfwGetWindowUserPointer(window));
    camera->apply_mouse(xpos, ypos);
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // setup camera
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback_wrapper);

    SceneObject light_object1(glm::vec3(0, 3, 0));
    SceneObject light_object2(glm::vec3(3, 3, 0));
    SceneObject mesh_object(glm::vec3(0, 1, 0));
    SceneObject mesh_object1(glm::vec3(3, 1, 0));
    SceneObject base_plate_object(glm::vec3(3, -1, 0), 0, 0, glm::vec3(100, 1, 100));

    SceneObject camera_object;
    Camera camera(window, &camera_object);
    camera.bind();

    Light light1(glm::vec4(1.0, 0.0, 0.0, 1.0));
    Light light2(glm::vec4(0.0, 0.0, 1.0, 1.0));

    // load a mesh
    Mesh cubeMesh(cubeVertices, cubeIndicies);
    Mesh lightMesh(lightVertices, cubeIndicies);
    Mesh basePlateMesh(cubeVertices, basePlateIndices);

    // create a shader program
    ShaderProgram shader;
    shader.add("shaders/triangle.frag");
    shader.add("shaders/triangle.vert");
    shader.link();

    int dx = 1;
    int dz = 1;

    double prevtime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double curtime = glfwGetTime();
        double dt = curtime - prevtime;
        prevtime = curtime;

        /*
            update call
        */

        handle_cursor_lock(window);

        if (fabsf(light_object1.position.x) > 2) dx*= -1;
        if (fabsf(light_object1.position.z) > 2) dz*= -1;

        light_object1.position.x += 2 * dx * dt;
        light_object1.position.z += 1.5 * dz * dt;

        camera.apply_inputs(dt);

        /*
            draw call
        */

        // Render background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1, 0.1, 0.1, 1.0);

        shader.activate();
        light1.render(&shader, &light_object1, 0);
        light2.render(&shader, &light_object2, 1);
        shader.set_uniform_1i("numLights", 2);

        camera.apply_viewport(&shader, "view");
        camera.apply_position(&shader, "cameraPosition");

        cubeMesh.draw(&shader, &mesh_object);
        cubeMesh.draw(&shader, &mesh_object1);
        basePlateMesh.draw(&shader, &base_plate_object);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}