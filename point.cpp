/***************************************
 * point.cpp
 */

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "point.h"

Point::Point() {
}

Point::Point(float x, float y, float z) {
    pos = glm::vec3(x, y, z); 
    old_pos = glm::vec3(x, y, z);
}

void Point::set_index(int i) {
    index = i;
}

void Point::set_acc(glm::vec3 acc) {
    acceleration = acc;
}

glm::vec3 Point::get_acc() {
    return acceleration;
}

std::vector<int> Point::get_s_neighbors(int i, int col_count, int row_count) {
    std::vector<int> s_neighbors;
    int row = i / col_count;
    int col = i - row * col_count;
    int up = (row - 1) * col_count + col;
    if (row > 0) s_neighbors.push_back(up);
    int down = (row + 1) * col_count + col;
    if (row < row_count-1) s_neighbors.push_back(down);
    int left = row * col_count + col - 1;
    if (col > 0) s_neighbors.push_back(left);
    int right = row * col_count + col + 1;
    if (col < col_count-1) s_neighbors.push_back(right);
    return s_neighbors;
}

std::vector<int> Point::get_d_neighbors(int i, int col_count, int row_count) {
    std::vector<int> d_neighbors;
    int row = i / col_count;
    int col = i - row * col_count;
    int up_left = (row - 1) * col_count + col - 1;
    if (row > 0 && col > 0) d_neighbors.push_back(up_left);
    int down_right = (row + 1) * col_count + col + 1;
    if (row < row_count-1 && col < col_count-1) d_neighbors.push_back(down_right);
    int up_right = (row - 1) * col_count + col + 1;
    if (row > 0 && col < col_count-1) d_neighbors.push_back(up_right);
    int down_left = (row + 1) * col_count + col - 1;
    if (col > 0 && row < row_count-1) d_neighbors.push_back(down_left);
    return d_neighbors;
}


