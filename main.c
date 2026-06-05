#include <glad/glad.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

// shadery napisane GLSL do skompilowania w trakcie(dlatego są w cudzysłowiach)
const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "void main()\n"
    "{\n"
    "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "    Normal = normalize(aPos);\n"
    "    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\n";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "in vec3 FragPos;\n"
    "in vec3 Normal;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 viewPos;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 objectColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    float ambientStrength = 0.1;\n"
    "    vec3 ambient = ambientStrength * lightColor;\n"
    "    vec3 lightDir = normalize(lightPos - FragPos);\n"
    "    float diff = max(dot(Normal, lightDir), 0.0);\n"
    "    vec3 diffuse = diff * lightColor;\n"
    "    float specularStrength = 0.5;\n"
    "    vec3 viewDir = normalize(viewPos - FragPos);\n"
    "    vec3 reflectDir = reflect(-lightDir, Normal);\n"
    "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"
    "    vec3 specular = specularStrength * spec * lightColor;\n"
    "    vec3 result = (ambient + diffuse + specular) * objectColor;\n"
    "    FragColor = vec4(result, 1.0);\n"
    "}\n";
void DrawSphere(GLfloat *vertices, GLuint *indices, float r,
                GLfloat x, GLfloat y, GLfloat z,
                int stacks, int slices);
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    float r = 0.4f;
    int stacks = 32;
    int slices = 32;

    GLuint v_num = (stacks + 1) * (slices + 1) * 3;
    GLuint i_num = stacks * slices * 6;

    GLfloat *verticies = malloc(sizeof(GLfloat) * v_num);
    GLuint *indicies = malloc(sizeof(GLuint) * i_num);

    DrawSphere(verticies, indicies, r, 0.0f, 0.0f, 0.0f, stacks, slices);

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * i_num, indicies,
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
        glClearColor(0.1f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        float identity[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, identity);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, identity);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, identity);
        glUniform3f(glGetUniformLocation(shader_program, "lightPos"), 1.2f, 1.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shader_program, "viewPos"), 0.0f, 0.0f, 3.0f);
        glUniform3f(glGetUniformLocation(shader_program, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader_program, "objectColor"), 0.2f, 0.5f, 1.0f);
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

    for (int i = 0; i <= (int)(360 * 12 * r); i++)
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

void DrawSphere(GLfloat *vertices, GLuint *indices, float r,
                GLfloat x, GLfloat y, GLfloat z,
                int stacks, int slices)
{
    int vidx = 0;
    int iidx = 0;

    for (int i = 0; i <= slices; i++)
    {
        float alpha = (float)i * M_PI / slices;
        float sa = sinf(alpha);
        float ca = cosf(alpha);

        for (int j = 0; j <= stacks; j++)
        {
            float beta = (float)j * 2.0f * M_PI / stacks;
            float sb = sinf(beta);
            float cb = cosf(beta);

            vertices[vidx++] = r * sa * cb + x;
            vertices[vidx++] = r * ca + y;
            vertices[vidx++] = r * sa * sb + z;
        }
    }

    for (int i = 0; i < slices; i++)
    {
        for (int j = 0; j < stacks; j++)
        {
            int A = i * (stacks + 1) + j;
            int B = i * (stacks + 1) + j + 1;
            int C = (i + 1) * (stacks + 1) + j;
            int D = (i + 1) * (stacks + 1) + j + 1;

            indices[iidx++] = A;
            indices[iidx++] = B;
            indices[iidx++] = C;

            indices[iidx++] = B;
            indices[iidx++] = D;
            indices[iidx++] = C;
        }
    }
}
