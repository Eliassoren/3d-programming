#include <GL/glew.h>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

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
      0.0, 0.0, -50.0
    };
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

class Cube {
  std::shared_ptr<Shader> shader;

  GLuint vertex_array_id;
  GLuint vertex_buffer_id;
  GLuint color_buffer_id;
  GLuint element_buffer_id;
public:
  Cube() : shader(Shader::get()) {
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
      2.0, -2.0, 2.0, // p1
      2.0, 2.0, -2.0, // p2
      2.0, 2.0, 2.0, // p3
      
      -2.0, -2.0, -2.0, // p4
      -2.0, -2.0, 2.0, // p5
      -2.0, 2.0, -2.0, // p6
      -2.0, 2.0, 2.0, // p7
    };
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
      2, 6, 7
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //Write colors to GPU buffer
    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    GLfloat colors[] = {
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      
      0.0, 1.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 1.0, 0.0,
      
      0.0, 0.0, 1.0,
      0.0, 0.0, 1.0,
      0.0, 0.0, 1.0,
      0.0, 0.0, 1.0,
      0.0, 0.0, 1.0,
      0.0, 0.0, 1.0,
      
      0.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      0.0, 1.0, 1.0,
      
      1.0, 1.0, 0.0,
      1.0, 1.0, 0.0,
      1.0, 1.0, 0.0,
      1.0, 1.0, 0.0,
      1.0, 1.0, 0.0,
      1.0, 1.0, 0.0,
      
      1.0, 1.0, 1.0,
      1.0, 1.0, 1.0,
      1.0, 1.0, 1.0,
      1.0, 1.0, 1.0,
      1.0, 1.0, 1.0,
      1.0, 1.0, 1.0,
                
    };
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    //Set color input attributes for shader program
    glEnableVertexAttribArray(color_attribute_id);
    glVertexAttribPointer(color_attribute_id, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
   
    //Unbind vertex array
    glBindVertexArray(0);
  }

  ~Cube() {
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

class SFMLApplication {
  class Window : public sf::Window {
  public:
    Window(const sf::ContextSettings &context_settings) : sf::Window(sf::VideoMode(800, 600), "SFML Example", sf::Style::Default, context_settings) {
      glewExperimental = GL_TRUE;
      if (glewInit() != GLEW_OK)
        std::cerr << "Failed to initialize GLEW" << std::endl;
    }
  };

  sf::ContextSettings context_settings;
  Window window;

  glm::mat4 projection;

  Cube cube;
  Axis axis;

public:
  SFMLApplication() : context_settings(24, 0, 0, 3, 3, sf::ContextSettings::Attribute::Core),
                      window(context_settings) {
    window.setFramerateLimit(144);
    window.setVerticalSyncEnabled(true);
   
    //Various settings
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projection = glm::perspective(glm::radians(45.0), 4.0 / 3.0, 0.1, 100.0);
  }

  void start() {
    glm::vec3 camera(0.0, 0.0, 6.0);
    
    bool running = true;
  
    while (running) {
      //Handle events
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed) {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            running = false;
          }
        }
        else if (event.type == sf::Event::MouseMoved) {
          camera.x = 0.01 * -(event.mouseMove.x - static_cast<int>(window.getSize().x) / 2);
          camera.y = 0.01 * (event.mouseMove.y - static_cast<int>(window.getSize().y) / 2);
        }
        else if (event.type == sf::Event::Closed) {
          window.close();
          running = false;
        }
      }

      //Draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      auto view = glm::lookAt(glm::vec3(camera.x * 10, camera.y * 10, 10.0), //Camera position in World Space
                              glm::vec3(0.0, 0.0, 0.0),      //Camera looks towards this position
                              glm::vec3(0.0, 2.0, 0.0));               //Up

      //Model matrix : an identity matrix (model will be at the origin)
      auto model = glm::mat4(1.0);
      
     // model = glm::scale(model,glm::vec3(0.5f, 0.5f, 1.0f));
      //The Model-View-Projection matrix
      auto MVP = projection * view * model;

      cube.draw(MVP);
      axis.draw(MVP);


      //Swap buffer (show result)
      window.display();
    }
  }
};

int main() {
  SFMLApplication sfml_application;
  sfml_application.start();

  return 0;
}
