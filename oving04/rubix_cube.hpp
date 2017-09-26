#include "shader.hpp"

class RubixCube {
  std::shared_ptr<Shader> shader;

  GLuint vertex_array_id;
  GLuint vertex_buffer_id;
  GLuint color_buffer_id;
  GLuint element_buffer_id;

public:
  RubixCube() : shader(Shader::get()) {
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
        //Defining the corners
        2.0, -2.0, -2.0, // p0
        2.0, -2.0, 2.0,  // p1
        2.0, 2.0, -2.0,  // p2
        2.0, 2.0, 2.0,   // p3

        -2.0, -2.0, -2.0, // p4
        -2.0, -2.0, 2.0,  // p5
        -2.0, 2.0, -2.0,  // p6
        -2.0, 2.0, 2.0,   // p7
    };
    GLfloat colors[] = {};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //Set vertex input attributes for shader program
    glEnableVertexAttribArray(vertex_attribute_id);
    glVertexAttribPointer(vertex_attribute_id, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &element_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
    // Indexes to assign combinations of points
    unsigned int indices[] = {
        // Side 1
        0, 1, 2,
        1, 2, 3,

        // Side 2
        0, 4, 6,
        0, 2, 6,

        // Side 3
        4, 5, 7,
        4, 6, 7,

        // Bottom
        0, 4, 5,
        0, 1, 5,

        // Side 4
        1, 5, 7,
        1, 3, 7,

        // Top
        2, 3, 7,
        2, 6, 7};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Write colors to GPU buffer
    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);


    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    //Set color input attributes for shader program
    glEnableVertexAttribArray(color_attribute_id);
    glVertexAttribPointer(color_attribute_id, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    //Unbind vertex array
    glBindVertexArray(0);
  }

  ~RubixCube() {
    glDeleteBuffers(1, &vertex_buffer_id);
    glDeleteBuffers(1, &color_buffer_id);
    glDeleteVertexArrays(1, &vertex_array_id);
  }

  void draw(const glm::mat4 &MVP) {


    glUseProgram(shader->program_id);

    //Set MVP attribute for shader program
    glUniformMatrix4fv(shader->MVP_id, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(vertex_array_id);
    // primitive, amount, datatype, start
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //Unbind vertex array
    glBindVertexArray(0);

    //Unlink shader program
    glUseProgram(0);
  }
};