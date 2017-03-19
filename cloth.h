/**************************************
 * cloth.h
 * The cloth class which maintains
 * the attributes in one cloth
 */

#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "point.h"

struct Constraint {
    int a;
    int b;
    float rest_distance;
};

class Cloth {
public:
    Cloth();
    Cloth(int, int, int, bool);
    int mode;  // 0 for mass-spring, 1 for constraint based
    std::vector<int> get_indices();
    std::vector<GLfloat> get_vertices();
    float get_grid_size();
    int get_row_count();
    int get_col_count(); 
    bool update_points(std::vector<float> &vertices);
    bool update_points_constraint(std::vector<float> &vertices);
    void wind_on();
    void ball_control(char input);
    void add_k();
    void reduce_k();
    void get_constraints();
    float get_ball_radius();
    glm::vec3 get_ball_center();
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
    bool pin_four;
    std::vector<Point*> points;  
    std::vector<int> indices;
    std::vector<Constraint*> constraints;
};

#endif
