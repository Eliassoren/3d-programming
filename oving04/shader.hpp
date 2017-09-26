#pragma once
#include <GL/glew.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <mutex>

///A reference-counted singleton class: the instance will be deleted when it is no longer in use.
class Shader {
  const GLchar *vertex_shader_source = R"(
    #version 150 core
    in vec3 vertex;
    in vec3 color;
    out vec3 fragment_color;
    
    uniform mat4 MVP;
    
    void main() {
      fragment_color = color;
      gl_Position = MVP * vec4(vertex, 1.0);
    }
  )";

  const GLchar *fragment_shader_source = R"(
    #version 150 core
    in vec3 fragment_color;
    out vec3 color;
    
    void main() {
      color = fragment_color;
    }
  )";

  Shader() {
    auto vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader_id);

    auto fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader_id);

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glBindFragDataLocation(program_id, 0, "fragment_color"); //fragment_color is sent from the vertex shader to the fragment shader
    glLinkProgram(program_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    MVP_id = glGetUniformLocation(program_id, "MVP");
  }

public:
  GLuint program_id;
  GLuint MVP_id;

  ///If an instance is already in use, return cached instance, otherwise return a new instance.
  static std::shared_ptr<Shader> get() {
    static std::weak_ptr<Shader> cache;
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    auto instance = cache.lock();
    if (!instance)
      cache = instance = std::shared_ptr<Shader>(new Shader());
    return instance;
  }

  ~Shader() {
    glDeleteProgram(program_id);
  }
};