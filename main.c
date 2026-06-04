#include <glad/glad.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

// shadery napisane GLSL do skompilowania w trakcie(dlatego są w cudzysłowiach)

// vertex shader odpowiada za pozycję
const char *vertexShaderSource =
    "#version 330 core\n"
    "\n"
    "layout(location = 0) in vec3 aPos;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "}\n";

// fragment shader odpowiada za kolory
const char *fragmentShaderSource =
    "#version 330 core\n"
    "\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

void DrawCirc(GLfloat *verticies, GLuint *indicies, float r, GLfloat x, GLfloat y);
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    float r = 0.3;
    GLuint v_num = r * 12 * 360 * 3 + 3;
    GLuint i_num = r * 12 * 360 * 3;

    GLfloat *verticies = malloc(sizeof(GLfloat) * v_num);
    GLuint *indices = malloc(sizeof(GLuint) * i_num);

    DrawCirc(verticies, indices, r, 0.0f, 0.0f);

    GLFWwindow *window = glfwCreateWindow(800, 800, "my app", NULL, NULL);
    if (window == NULL)
    {
        printf("Error creating a window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 800, 800);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * v_num, verticies, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * i_num, indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glClearColor(0.0f, 0.3f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.0f, 0.3f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, i_num, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();
}
void DrawCirc(GLfloat *verticies, GLuint *indicies, float r, GLfloat x, GLfloat y)
{
    verticies[0] = x;
    verticies[1] = y;
    verticies[2] = 0.0f;

    for (int i = 0; i < (int)(360 * 12 * r); i++)
    {
        GLuint idx = 3 * i;
        float angle = M_PI * i / 180;
        float s = sin(angle);
        float c = cos(angle);
        verticies[idx + 3] = (GLfloat)(-r * s + x);
        verticies[idx + 4] = (GLfloat)(r * c + y);
        verticies[idx + 5] = 0.0f;
        indicies[idx] = 0;
        indicies[idx + 1] = i + 1;
        indicies[idx + 2] = i + 2;
    }
}
