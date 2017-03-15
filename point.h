/***************************************
 * point.h
 * The point class which maintains
 * the attributes in one point in the cloth
 */

#ifndef POINT_H
#define POINT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Point {
public:
    Point();
    Point(float, float, float);
    glm::vec3 pos;
    void set_index(int);
    void set_vel(glm::vec3);
    glm::vec3 get_vel();
    std::vector<int> get_s_neighbors(int i, int col_count, int row_count);
    std::vector<int> get_d_neighbors(int i, int col_count, int row_count);
private:
    glm::vec3 velocity;
    glm::vec3 color;
    int index;
};

#endif
