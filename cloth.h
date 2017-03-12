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
private:
    int row_count;  // Row count (for points)
    int col_count;  // Column count (for points)
    float grid_size;
    int vertex_count;
    std::vector<Point*> points;  
    std::vector<int> indices;
};

#endif
