#include <glad/glad.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#define SHADER_IMPLEMENTATION
#include "myShader.h"

typedef struct
{
    vec3 pos;
} body;

void DrawSphere(GLfloat *vertices, GLuint *indices, float r,
                GLfloat x, GLfloat y, GLfloat z,
                int stacks, int slices);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float r = 0.2f;
    int stacks = 16;
    int slices = 16;

    GLuint v_num = (stacks + 1) * (slices + 1) * 3;
    GLuint i_num = stacks * slices * 6;

    GLfloat *verticies = malloc(sizeof(GLfloat) * v_num);
    GLuint *indicies = malloc(sizeof(GLuint) * i_num);

    body spheres[8] = {{{0.3f, 0.3f, 0.3f}},
                       {{-0.3f, 0.3f, 0.3f}},
                       {{0.3f, -0.3f, 0.3f}},
                       {{-0.3f, -0.3f, 0.3f}},
                       {{0.3f, 0.3f, -0.3f}},
                       {{-0.3f, 0.3f, -0.3f}},
                       {{0.3f, -0.3f, -0.3f}},
                       {{-0.3f, -0.3f, -0.3f}}};

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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, 800, 800);

    Shader myShader = shader_create(
        "external/shaders/default.vert",
        "external/shaders/default.frag");

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * v_num, verticies, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * i_num, indicies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glClearColor(0.54, 0.17, 0.89, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.54, 0.17, 0.89, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float angle = (float)glfwGetTime();

        for (int i = 0; i < 8; i++)
        {
            mat4 model, view, proj, mvp;
            glm_mat4_identity(model);
            glm_rotate(model, angle, (vec3){1.0f, 1.0f, 1.0f});
            glm_translate(model, spheres[i].pos);
            glm_rotate(model, angle, (vec3){1.0f, 1.0f, 1.0f});
            glm_lookat(
                (vec3){0.0f, 0.0f, 2.5f},
                (vec3){0.0f, 0.0f, 0.0f},
                (vec3){0.0f, 1.0f, 0.0f},
                view);

            glm_perspective(
                glm_rad(45.0f),
                800.0f / 800.0f,
                0.1f, 100.0f,
                proj);

            mat4 temp;
            glm_mat4_mul(proj, view, temp);
            glm_mat4_mul(temp, model, mvp);

            vec3 light_dir = {-1.0f, -1.0f, 1.0f};
            glm_vec3_normalize(light_dir);
            shader_use(&myShader);
            shader_set_mat4(&myShader, "uMVP", (float *)mvp);
            shader_set_mat4(&myShader, "uModel", (float *)model);
            shader_set_vec3(&myShader, "uLDir", (float *)light_dir);
            shader_set_float(&myShader, "uTime", (float)glfwGetTime());

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, i_num, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, i_num, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    shader_delete(&myShader);

    // glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
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