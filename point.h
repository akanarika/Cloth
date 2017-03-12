/***************************************
 * point.h
 * The point class which maintains
 * the attributes in one point in the cloth
 */

#ifndef POINT_H
#define POINT_H

#include <GL/glew.h>

class Point {
public:
    Point();
    Point(GLfloat, GLfloat, GLfloat);
    std::vector<GLfloat> pos;
private:
    std::vector<GLfloat> color;
    std::vector<GLfloat> force;
    std::vector<GLfloat> velocity;
};

#endif
