#include "shader.hpp"

class Cube {

private:
    int id;
    GLfloat vertices[24];
    GLfloat indices[18];
    GLfloat colors[36];

public:
    Cube(int id, GLfloat vertices, GLfloat indices, GLfloat colors);
};
Cube::
    int
    Cube::getId() const {
    return id;
}
GLfloat[] Cube::getVertices() const {
    return vertices;
}

GLfloat[] Cube::getIndices() const {
    return vertices;
}

GLfloat[] Cube::getColors() const {
    return vertices;
}
void Cube::setVertices(GLfloat vertices) {
    this->vertices = vertices;
}
}
;