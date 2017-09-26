#include "shader.hpp"

class Axis {
  std::shared_ptr<Shader> shader;

  GLuint vertex_array_id;
  GLuint vertex_buffer_id;
  GLuint color_buffer_id;

public:
  Axis() : shader(Shader::get()) {
    //Receive the attribute ids from attribute names in the vertex shader source
    auto vertex_attribute_id = glGetAttribLocation(shader->program_id, "vertex");
    auto color_attribute_id = glGetAttribLocation(shader->program_id, "color");

    //The vertex array stores the buffers created below
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    //Write vertices to GPU buffer
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    GLfloat vertices[] = {
        // x-axis
        50.0, 0.0, 0.0,
        -50.0, 0.0, 0.0,

        // y-axis
        0.0, 50.0, 0.0,
        0.0, -50.0, 0.0,

        //z-axis
        0.0, 0.0, 50.0,
        0.0, 0.0, -50.0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Set vertex input attributes for shader program
    glEnableVertexAttribArray(vertex_attribute_id);
    glVertexAttribPointer(vertex_attribute_id, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


    //Write colors to GPU buffer
    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    GLfloat colors[] = {
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,

        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,

        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,

    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    //Set color input attributes for shader program
    glEnableVertexAttribArray(color_attribute_id);
    glVertexAttribPointer(color_attribute_id, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    //Unbind vertex array
    glBindVertexArray(0);
  }

  ~Axis() {
    glDeleteBuffers(1, &vertex_buffer_id);
    glDeleteBuffers(1, &color_buffer_id);
    glDeleteVertexArrays(1, &vertex_array_id);
  }

  void draw(glm::mat4 &MVP) {


    glUseProgram(shader->program_id);

    //Set MVP attribute for shader program
    glUniformMatrix4fv(shader->MVP_id, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(vertex_array_id);
    // primitive, amount, datatype, start
    glDrawArrays(GL_LINES, 0, 6);
    //Unbind vertex array
    glBindVertexArray(0);

    //Unlink shader program
    glUseProgram(0);
  }
};