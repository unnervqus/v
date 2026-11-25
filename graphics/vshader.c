#include "graphics/vshader.h"
#include "core/vmemory.h"
#include "core/vlogging.h"

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

struct vShader {
    GLuint id;
};

static char* vShaderLoadSource(const char* path) {
    FILE* file = fopen(path, "rb");

    if (!file) {
        vLog("[SHADER] Failed to open: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    if (size < 0) {
        fclose(file);
        return NULL;
    }

    char* source = vMalloc((size_t)size + 1);

    if (!source) {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(source, 1, (size_t)size, file);
    fclose(file);

    if (read_size != (size_t)size) {
        vFree(source);
        return NULL;
    }

    source[size] = '\0';
    return source;
}

static GLuint vShaderCompile(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        char* log = malloc((size_t)log_length + 1);

        if (log) {
            glGetShaderInfoLog(shader, log_length, NULL, log);
            vLog("[SHADER] Compilation error:\n%s\n", log);
            free(log);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

vShader vShaderCreate(
    const char* vertex_path,
    const char* fragment_path
) {
    if (!vertex_path || !fragment_path) {
        vLog("[SHADER] Create failed: source path is null\n");
        return NULL;
    }

    vLog("[SHADER] Creating program (vertex='%s', fragment='%s')\n", vertex_path, fragment_path);

    char* vertex_source = vShaderLoadSource(vertex_path);
    char* fragment_source = vShaderLoadSource(fragment_path);

    if (!vertex_source || !fragment_source) {
        vFree(vertex_source);
        vFree(fragment_source);
        return NULL;
    }

    GLuint vertex = vShaderCompile(
        GL_VERTEX_SHADER,
        vertex_source
    );

    GLuint fragment = vShaderCompile(
        GL_FRAGMENT_SHADER,
        fragment_source
    );

    vFree(vertex_source);
    vFree(fragment_source);

    if (!vertex || !fragment) {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        return NULL;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (!success) {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        char* log = malloc((size_t)log_length + 1);

        if (log) {
            glGetProgramInfoLog(program, log_length, NULL, log);
            vLog("[SHADER] Linking error:\n%s\n", log);
            free(log);
        }

        glDeleteProgram(program);
        return NULL;
    }

    vShader shader = vMalloc(sizeof(*shader));

    if (!shader) {
        glDeleteProgram(program);
        return NULL;
    }

    shader->id = program;

    vLog("[SHADER] Program ready (id=%u)\n", program);
    return shader;
}

void vShaderDestroy(vShader shader) {
    if (!shader) return;

    glDeleteProgram(shader->id);
    vFree(shader);
}

void vShaderUse(vShader shader) {
    if (!shader) return;

    glUseProgram(shader->id);
}

u32 vShaderGetID(vShader shader) {
    return shader ? shader->id : 0;
}

i32 vShaderGetUniformLocation(vShader shader, const char* name) {
    if (!shader || !name) return -1;

    return glGetUniformLocation(shader->id, name);
}

void vShaderSetInt(vShader shader, i32 location, i32 value) {
    if (!shader || location < 0) return;

    glUniform1i(location, value);
}

void vShaderSetFloat(vShader shader, i32 location, float value) {
    if (!shader || location < 0) return;

    glUniform1f(location, value);
}

void vShaderSetVec2(vShader shader, i32 location, vVec2 value) {
    if (!shader || location < 0) return;

    glUniform2f(location, value.x, value.y);
}

void vShaderSetVec3(vShader shader, i32 location, vVec3 value) {
    if (!shader || location < 0) return;

    glUniform3f(location, value.x, value.y, value.z);
}

void vShaderSetVec4(vShader shader, i32 location, vVec4 value) {
    if (!shader || location < 0) return;

    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void vShaderSetMat3(vShader shader, i32 location, const float* value) {
    if (!shader || location < 0 || !value) return;

    glUniformMatrix3fv(location, 1, GL_FALSE, value);
}

void vShaderSetMat4(vShader shader, i32 location, const float* value) {
    if (!shader || location < 0 || !value) return;

    glUniformMatrix4fv(location, 1, GL_FALSE, value);
}