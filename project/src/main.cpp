#include "main.hpp"

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

    ShaderProgram shader = ShaderProgram::create();
    ShaderProgram::add(&shader, "shaders/triangle.frag");
    ShaderProgram::add(&shader, "shaders/triangle.vert");
    ShaderProgram::link(&shader);

    // Define the vertices for the triangle
    float vertices[] = {
         0.0f,  0.5f, 0.0f,  // top
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f   // bottom right
    };

    // Set up vertex buffer and vertex array objects
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the Vertex Array Object
    glBindVertexArray(VAO);

    // Bind and set the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind buffer and array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // Render background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        ShaderProgram::run(&shader);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ShaderProgram::destroy(&shader);
    glfwTerminate();
    return 0;
}