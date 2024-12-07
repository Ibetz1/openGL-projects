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

std::vector<GLuint> basePlateIndices = {
    16, 17, 18, 16, 18, 19,
};

struct Light {
    WorldObject* object_binding;
    glm::vec4 color;

    static Light create(WorldObject* binding, glm::vec4 color) {
        Light light = { 0 };

        light.object_binding = binding;
        light.color = color;

        return light;
    }

    static void destroy(Light* light) {
        if (!light) return;

        *light = { 0 };
    }

    static void render(Light* light, ShaderProgram* shader) {

        ShaderProgram::set_uniform_3fv(shader, "lightPosition", glm::value_ptr(light->object_binding->position));
        ShaderProgram::set_uniform_4fv(shader, "lightColor", glm::value_ptr(light->color));
    }
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
    WorldObject mesh_object = WorldObject::create(glm::vec3(0, 1, 0));
    WorldObject mesh_object1 = WorldObject::create(glm::vec3(3, 1, 0));
    WorldObject light_object = WorldObject::create(glm::vec3(0, 3, 0));
    WorldObject base_plate_object = WorldObject::create(glm::vec3(3, -1, 0), 0, 0, glm::vec3(100, 1, 100));

    // create a shader program
    ShaderProgram shader = ShaderProgram::create();
    ShaderProgram::add(&shader, "shaders/triangle.frag");
    ShaderProgram::add(&shader, "shaders/triangle.vert");
    ShaderProgram::link(&shader);
    
    ShaderProgram lightShader = ShaderProgram::create();
    ShaderProgram::add(&lightShader, "shaders/light.frag");
    ShaderProgram::add(&lightShader, "shaders/light.vert");
    ShaderProgram::link(&lightShader);

    // load a texture
    Texture texture = Texture::create("assets/tex.png", Texture::TTYPE_TEXTURE, GL_TEXTURE0);
    Texture specular = Texture::create("assets/specular.png", Texture::TTYPE_SPECULAR, GL_TEXTURE1);
    Texture plate = Texture::create("assets/plate.png", Texture::TTYPE_TEXTURE, GL_TEXTURE0);
    Texture plateSpecular = Texture::create("assets/plate_specular.png", Texture::TTYPE_SPECULAR, GL_TEXTURE1);

    // load a mesh
    Mesh cubeMesh = Mesh::create(cubeVertices, cubeIndicies, { texture, specular });
    Mesh lightMesh = Mesh::create(lightVertices, cubeIndicies, {  });
    Mesh basePlateMesh = Mesh::create(cubeVertices, basePlateIndices, { plate, plateSpecular });
    basePlateMesh.texture_tile_count = 50;
    
    // define camera
    Camera camera = Camera::create(window, &camera_object);
    Camera::bind(&camera);

    double prevtime = glfwGetTime();

    Light light = Light::create(&light_object, glm::vec4(1.0, 0.5, 0.5, 1.0));

    int dx = 1;
    int dz = 1;

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

        if (fabsf(light_object.position.x) > 2) dx*= -1;
        if (fabsf(light_object.position.z) > 2) dz*= -1;

        light_object.position.x += 2 * dx * dt;
        light_object.position.z += 1.5 * dz * dt;

        ShaderProgram::activate(&lightShader);
        Light::render(&light, &lightShader);
        Camera::apply_viewport(&camera, &lightShader, "view");
        Mesh::draw(&lightMesh, &lightShader, &light_object);

        ShaderProgram::activate(&shader);
        Light::render(&light, &shader);
        Camera::apply_viewport(&camera, &shader, "view");
        Camera::apply_position(&camera, &shader, "cameraPosition");
        Mesh::draw(&cubeMesh, &shader, &mesh_object);
        Mesh::draw(&cubeMesh, &shader, &mesh_object1);
        Mesh::draw(&basePlateMesh, &shader, &base_plate_object);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ShaderProgram::destroy(&shader);
    Mesh::destroy(&cubeMesh);
    // Texture::destroy(&texture);
    Texture::destroy(&specular);

    glfwTerminate();
    return 0;
}