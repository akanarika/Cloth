/**************************************
 * cloth.h
 * The cloth class which maintains
 * the attributes in one cloth
 */

#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "point.h"

class Cloth {
public:
    Cloth();
    Cloth(int, int);
    std::vector<int> get_indices();
    std::vector<GLfloat> get_vertices();
    float get_grid_size();
    int get_row_count();
    int get_col_count(); 
    bool update_points(std::vector<float> &vertices);
    void wind_on();
    void ball_control(char input);
    void add_k();
    void reduce_k();
private:
    int row_count;  // Row count (for points)
    int col_count;  // Column count (for points)
    int vertex_count;
    float grid_size;
    float mass;
    float k;
    float time;
    glm::vec3 ball_center;
    float ball_radius;
    bool wind;
    std::vector<Point*> points;  
    std::vector<int> indices;
};

#endif
