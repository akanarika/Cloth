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
    glm::vec3 old_pos;
    glm::vec3 force;
    void set_index(int);
    void set_acc(glm::vec3);
    glm::vec3 get_acc();
    std::vector<int> get_s_neighbors(int i, int col_count, int row_count);
    std::vector<int> get_d_neighbors(int i, int col_count, int row_count);
private:
    glm::vec3 acceleration;
    glm::vec3 color;
    int index;
};

#endif
