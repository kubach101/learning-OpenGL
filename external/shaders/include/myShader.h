#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> /* or <GL/gl.h> depending on your setup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── The "class" ── */
typedef struct
{
    unsigned int ID; /* OpenGL program ID */
} Shader;

/* ── Forward declarations (the "methods") ── */
Shader shader_create(const char *vertexPath, const char *fragmentPath);
void shader_use(const Shader *s);
void shader_delete(const Shader *s);

/* Uniform setters */
void shader_set_bool(const Shader *s, const char *name, int value);
void shader_set_int(const Shader *s, const char *name, int value);
void shader_set_float(const Shader *s, const char *name, float value);

/* ================================================================
   IMPLEMENTATION  (define SHADER_IMPLEMENTATION in exactly ONE .c
   file before including this header, just like stb libraries)
   ================================================================ */
void shader_set_mat4(const Shader *s, const char *name, const float *value);
void shader_set_vec3(const Shader *s, const char *name, const float *value);
#ifdef SHADER_IMPLEMENTATION

void shader_set_mat4(const Shader *s, const char *name, const float *value)
{
    glUniformMatrix4fv(glGetUniformLocation(s->ID, name), 1, GL_FALSE, value);
}

void shader_set_vec3(const Shader *s, const char *name, const float *value)
{
    glUniform3fv(glGetUniformLocation(s->ID, name), 1, value);
}

/* ── Internal helper: read a whole file into a heap string ── */
static char *_read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "SHADER: cannot open file: %s\n", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *buf = (char *)malloc(len + 1);
    if (!buf)
    {
        fclose(f);
        return NULL;
    }

    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

/* ── Internal helper: compile one shader stage and check errors ── */
static unsigned int _compile_shader(const char *src, GLenum type)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int success;
    char log[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(id, 512, NULL, log);
        fprintf(stderr, "SHADER COMPILE ERROR (%s):\n%s\n",
                type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT", log);
    }
    return id;
}

/* ── Constructor ── */
Shader shader_create(const char *vertexPath, const char *fragmentPath)
{
    Shader s;
    s.ID = 0;

    char *vsrc = _read_file(vertexPath);
    char *fsrc = _read_file(fragmentPath);
    if (!vsrc || !fsrc)
    {
        free(vsrc);
        free(fsrc);
        return s;
    }

    unsigned int vert = _compile_shader(vsrc, GL_VERTEX_SHADER);
    unsigned int frag = _compile_shader(fsrc, GL_FRAGMENT_SHADER);
    free(vsrc);
    free(fsrc);

    /* Link into a program */
    s.ID = glCreateProgram();
    glAttachShader(s.ID, vert);
    glAttachShader(s.ID, frag);
    glLinkProgram(s.ID);

    int success;
    char log[512];
    glGetProgramiv(s.ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(s.ID, 512, NULL, log);
        fprintf(stderr, "SHADER LINK ERROR:\n%s\n", log);
    }

    glDeleteShader(vert); /* shaders are baked into the program now */
    glDeleteShader(frag);
    return s;
}

/* ── Methods ── */
void shader_use(const Shader *s) { glUseProgram(s->ID); }
void shader_delete(const Shader *s) { glDeleteProgram(s->ID); }

void shader_set_bool(const Shader *s, const char *name, int value)
{
    glUniform1i(glGetUniformLocation(s->ID, name), value);
}

void shader_set_int(const Shader *s, const char *name, int value)
{
    glUniform1i(glGetUniformLocation(s->ID, name), value);
}

void shader_set_float(const Shader *s, const char *name, float value)
{
    glUniform1f(glGetUniformLocation(s->ID, name), value);
}

#endif /* SHADER_IMPLEMENTATION */
#endif /* SHADER_H */