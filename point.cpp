/***************************************
 * point.cpp
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "point.h"

Point::Point() {
}

Point::Point(GLfloat x, GLfloat y, GLfloat z) {
    std::vector<GLfloat> tmp;
    tmp.push_back(x);
    tmp.push_back(y);
    tmp.push_back(z);
    pos = tmp;
}
