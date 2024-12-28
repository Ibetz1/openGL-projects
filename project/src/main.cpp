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

struct Light {
    glm::vec4 color;
    float intensity;
    float brightness;

    static Light create(glm::vec4 color) {
        Light light = { };

        light.color = color;
        light.intensity = 1.0f;
        light.brightness = 1.0f;

        return light;
    }

    static void destroy(Light* light) {
        if (!light) return;

        *light = { };
    }

    static void set_color(Light* light, glm::vec4 color) {
        light->color = color;
    }

    static void set_intensity(Light* light, float intensity) {
        light->intensity = intensity;
    }

    static void set_brightness(Light* light, float brightness) {
        light->brightness = brightness;
    }

    static void render(const Light* light, const ShaderProgram* shader, const WorldObject* binding, int idx) {
        std::string shaderReference = "lights[" + std::to_string(idx) + "]";

        ShaderProgram::set_uniform_4fv(shader, (shaderReference + ".Color").c_str(), glm::value_ptr(light->color));
        ShaderProgram::set_uniform_3fv(shader, (shaderReference + ".Position").c_str(), glm::value_ptr(binding->position));
        ShaderProgram::set_uniform_1f(shader, (shaderReference + ".Intensity").c_str(), light->intensity);
        ShaderProgram::set_uniform_1f(shader, (shaderReference + ".Brightness").c_str(), light->brightness);
    }
};

struct LightContainer {
    static constexpr int maxLights = 10;
    std::unordered_map<const WorldObject*, Light*> lights;

    static LightContainer create() {
        LightContainer container = { };

        return container;
    }

    static void destroy(LightContainer* container) {
        container->lights.clear();
        *container = { };
    }

    // adds a light with an object binding
    static void add_light(LightContainer* container, const WorldObject* binding, Light* light) {
        if (container->lights.size() >= maxLights) {
            LOGE("exceeded max lights");
            return;
        }

        container->lights.insert({binding, light});
    }

    // returns the light reference from its object binding
    static Light* get_light(LightContainer* container, const WorldObject* binding) {
        if (container->lights.count(binding)) {
            return container->lights.at(binding);
        }

        return nullptr;
    }

    // remove a light based on a pointer reference to the light
    static void remove_light(LightContainer* container, const Light* light) {
        for (auto it = container->lights.begin(); it != container->lights.end(); ) {
            if (it->second == light) {
                container->lights.erase(it);
                LOGI("removed light from light container");
                return;
            } else {
                ++it;
            }
        }

        LOGE("failed to remove light: not found");
    }

    // remove light based on its object binding
    static void remove_light(LightContainer* container, const WorldObject* binding) {
        if (container->lights.count(binding)) {
            container->lights.erase(binding);
            LOGI("removed light from light container");
        }
    }

    // rebind light to world object
    static void rebind_light(LightContainer* container, Light* light, const WorldObject* binding) {
        remove_light(container, light);
        add_light(container, binding, light);
    }

    static void render(LightContainer* container, ShaderProgram* shader) {
        int idx = 0;

        for (const auto & [binding, light] : container->lights) {
            Light::render(light, shader, binding, idx);
            ++idx;
        }

        ShaderProgram::set_uniform_1i(shader, "numLights", MIN(int, container->lights.size(), maxLights));
    }
};

struct MeshContainer {
    std::unordered_map<const WorldObject*, Mesh*> meshes;

    static MeshContainer create() {
        MeshContainer container = { };

        return container;
    }

    static void destroy(MeshContainer* container) {
        container->meshes.clear();
        *container = { };
    }

    static void add_mesh(MeshContainer* container, const WorldObject* binding, Mesh* mesh) {
        container->meshes.insert({binding, mesh});
    }

    // returns the light reference from its object binding
    static Mesh* get_mesh(MeshContainer* container, const WorldObject* binding) {
        if (container->meshes.count(binding)) {
            return container->meshes.at(binding);
        }

        return nullptr;
    }

    // remove a light based on a pointer reference to the light
    static void remove_mesh(MeshContainer* container, const Mesh* mesh) {
        for (auto it = container->meshes.begin(); it != container->meshes.end(); ) {
            if (it->second == mesh) {
                container->meshes.erase(it);
                LOGI("removed light from light container");
                return;
            } else {
                ++it;
            }
        }

        LOGE("failed to remove light: not found");
    }

    // remove light based on its object binding
    static void remove_mesh(MeshContainer* container, const WorldObject* binding) {
        container->meshes.erase(binding);
        LOGI("removed light from light container");
    }

    // rebind light to world object
    static void rebindLight(MeshContainer* container, Mesh* mesh, const WorldObject* binding) {
        remove_mesh(container, mesh);
        add_mesh(container, binding, mesh);
    }

    // renders all meshes in the container
    static void render(MeshContainer* container, ShaderProgram* shader) {
        int idx = 0;

        for (const auto & [binding, mesh] : container->meshes) {
            Mesh::draw(mesh, binding, shader);
            ++idx;
        }
    }
};

struct Scene {
    MeshContainer meshes;
    LightContainer lights;

    ShaderProgram* primaryShader;
    Camera* camera;

    static Scene create(
        ShaderProgram* primaryShader,
        Camera* camera
    ) {
        Scene scene = {};

        scene.meshes = MeshContainer::create();
        scene.lights = LightContainer::create();
        scene.primaryShader = primaryShader;
        scene.camera = camera;

        return scene;
    }

    static void destroy(Scene* scene) {
        MeshContainer::destroy(&scene->meshes);
        LightContainer::destroy(&scene->lights);

        *scene = {};
    }

    static void add_object(Scene* scene, WorldObject* binding, Light* light) {

    }

    static void add_object(Scene* scene, WorldObject* binding, Mesh* mesh) {

    }

    static void render(Scene* scene) {
        ShaderProgram::activate(scene->primaryShader);
        LightContainer::render(&scene->lights, scene->primaryShader); // draw lights

        Camera::apply_viewport(scene->camera, scene->primaryShader, "view");
        Camera::apply_position(scene->camera, scene->primaryShader, "cameraPosition");

        MeshContainer::render(&scene->meshes, scene->primaryShader); // draw meshes
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // setup camera
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback_wrapper);

    WorldObject light_object1 = WorldObject::create(glm::vec3(0, 3, 0));
    WorldObject light_object2 = WorldObject::create(glm::vec3(3, 3, 0));
    WorldObject mesh_object = WorldObject::create(glm::vec3(0, 1, 0));
    WorldObject mesh_object1 = WorldObject::create(glm::vec3(3, 1, 0));
    WorldObject base_plate_object = WorldObject::create(glm::vec3(3, -1, 0), 0, 0, glm::vec3(100, 1, 100));
    WorldObject camera_object = WorldObject::create();

    // move to heap

    Light light1 = Light::create(glm::vec4(1.0, 0.0, 0.0, 1.0));
    Light light2 = Light::create(glm::vec4(0.0, 0.0, 1.0, 1.0));

    LightContainer lights = LightContainer::create();
    LightContainer::add_light(&lights, &light_object1, &light1);
    LightContainer::add_light(&lights, &light_object2, &light2);

    int dx = 1;
    int dz = 1;

    // load a mesh
    Mesh cubeMesh = Mesh::create(cubeVertices, cubeIndicies);
    Mesh lightMesh = Mesh::create(lightVertices, cubeIndicies);
    Mesh basePlateMesh = Mesh::create(cubeVertices, basePlateIndices);

    // define world objects
    MeshContainer meshes = MeshContainer::create();
    MeshContainer::add_mesh(&meshes, &mesh_object, &cubeMesh);
    MeshContainer::add_mesh(&meshes, &mesh_object1, &cubeMesh);
    MeshContainer::add_mesh(&meshes, &base_plate_object, &basePlateMesh);

    // define camera
    Camera camera = Camera::create(window, &camera_object);
    Camera::bind(&camera);

    // create a shader program
    ShaderProgram shader = ShaderProgram::create();
    ShaderProgram::add(&shader, "shaders/triangle.frag");
    ShaderProgram::add(&shader, "shaders/triangle.vert");
    ShaderProgram::link(&shader);

    double prevtime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double curtime = glfwGetTime();
        double dt = curtime - prevtime;
        prevtime = curtime;

        if (fabsf(light_object1.position.x) > 2) dx*= -1;
        if (fabsf(light_object1.position.z) > 2) dz*= -1;

        light_object1.position.x += 2 * dx * dt;
        light_object1.position.z += 1.5 * dz * dt;

        handle_cursor_lock(window);

        Camera::apply_inputs(&camera, dt);

        // Render background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1, 0.1, 0.1, 1.0);

        ShaderProgram::activate(&shader);
        LightContainer::render(&lights, &shader);

        Camera::apply_viewport(&camera, &shader, "view");
        Camera::apply_position(&camera, &shader, "cameraPosition");

        MeshContainer::render(&meshes, &shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ShaderProgram::destroy(&shader);
    Mesh::destroy(&cubeMesh);

    glfwTerminate();
    return 0;
}