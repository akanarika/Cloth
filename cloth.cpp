/**************************************
 * cloth.cpp
 */

#include <iostream>
#include "cloth.h"

Cloth::Cloth() {

}

Cloth::Cloth(int r, int c): row_count(r), col_count(c) {
    grid_size = 1.0f / (float)(std::max(row_count-1, col_count-1));
    vertex_count = row_count * col_count;
    
    // Vertices initialization
    for (int i=0; i<row_count; i++) {
        for (int j=0; j<col_count; j++) {
            Point* curr_point = new Point(grid_size * i, grid_size * j, 0.0f);
            points.push_back(curr_point);
        }
    }

    // Indices initialization
    int index_count = (r - 1) * (c - 1) * 6;
    for (int i=0; i<r-1; i++) {
        int stride = i * (c - 1);
        for (int j=0; j<c-1; j++) {
            indices.push_back(i * c + j);
            indices.push_back(i * c + j + 1);
            indices.push_back((i + 1) * c + j);

            indices.push_back(i * c + j + 1);
            indices.push_back((i + 1) * c + j);
            indices.push_back((i + 1) * c + j + 1);
        }
    }
            
}

std::vector<int> Cloth::get_indices() {
    return indices;
}

std::vector<GLfloat> Cloth::get_vertices() {
    std::vector<GLfloat> vertices;
    for (int i=0; i<vertex_count; i++) {
        vertices.push_back(points[i]->pos[0]);
        vertices.push_back(points[i]->pos[1]);
        vertices.push_back(points[i]->pos[2]);
    }
    return vertices;
}

float Cloth::get_grid_size() {
    return grid_size;
}

int Cloth::get_row_count() {
    return row_count;
}

int Cloth::get_col_count() {
    return col_count;
}
