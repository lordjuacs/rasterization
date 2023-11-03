#ifndef GLUT_PLY_H
#define GLUT_PLY_H
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Objeto.h"
#include "shader_m.h"
using namespace std;
using namespace glm;

class Model_PLY: public Objeto {
public:
	Model_PLY();
    int		Load(char *filename);
    void    imprimir();
    int     enviar_GPU();
    GLuint  setup() { return enviar_GPU(); }
    void    display(Shader &sh);
    float orba = 1;
    float rotx = 0.0f;
    float roty = 0.0f;
    float rotz = 0.0f;
    void move_around_point(vec3 point, float orbital_angle) {
        float radius_ = sqrt(
                pow(point.x - this->bs.getc().x, 2) + pow(point.y - this->bs.getc().y, 2) +
                pow(point.z - this->bs.getc().z, 2));
        auto new_x = point.x + radius_ * cos(orbital_angle * orba);
        auto new_z = point.z + radius_ * sin(orbital_angle * orba);
        this->traslacion = vec3(new_x - this->centro.x, 0, new_z - this->centro.z);
        this->bs.traslacion = this->traslacion;
    }
};

#endif
