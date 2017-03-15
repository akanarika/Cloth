/**************************************
 * cloth.cpp
 */

#include <iostream>
#include <vector>
#include <math.h>
#include <glm/ext.hpp>
#include "cloth.h"

Cloth::Cloth() {

}

Cloth::Cloth(int r, int c): row_count(r), col_count(c) {
    grid_size = 1.0f / (float)(std::max(r-1, c-1));
    vertex_count = r * c;
    mass = 1.0f;
    
    // Vertices initialization
    for (int i=0; i<r; i++) {
        for (int j=0; j<c; j++) {
            Point* curr_point = new Point(grid_size * j, 0.0f, grid_size * i);
            curr_point->set_index(i * c + j);
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

std::vector<float> Cloth::get_vertices() {
    std::vector<float> vertices;
    for (int i=0; i<vertex_count; i++) {
        vertices.push_back(points[i]->pos.x);
        vertices.push_back(points[i]->pos.y);
        vertices.push_back(points[i]->pos.z);
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

bool Cloth::update_points(std::vector<float> &vertices) {
    glm::vec3 force;
    glm::vec3 gravity;
    glm::vec3 spring_force;
    float vertex_mass = mass / vertex_count;
    float timestep = 0.0001f;
    float damping = 0.02f;
    float k = 8.0f;
    gravity = glm::vec3(0, 9.8f, 0);
    force = vertex_mass * gravity;
    for (int i=0; i<vertex_count; i++) {
        force = vertex_mass * gravity;
        Point* curr_point = points[i];
        //std::cout <<"point: " << i << std::endl;
        std::vector<int> s_neighbors = curr_point->get_s_neighbors(i, col_count, 
                                                                   row_count);
        std::vector<int> d_neighbors = curr_point->get_d_neighbors(i, col_count, 
                                                                   row_count);    
        for (int j=0; j<s_neighbors.size(); j++) {
            //std::cout <<"neighbor: " <<  s_neighbors[j] << std::endl;
            Point* neighbor_point = points[s_neighbors[j]];
            glm::vec3 x = neighbor_point->pos - curr_point->pos;
            force += x * (glm::length(x) - grid_size) * k/ glm::length(x);
        }
        for (int j=0; j<d_neighbors.size(); j++) {
            //std::cout <<"neighbor: " <<  d_neighbors[j] << std::endl;
            Point* neighbor_point = points[d_neighbors[j]];
            glm::vec3 x = neighbor_point->pos - curr_point->pos;
            force += x * (glm::length(x)- grid_size * (float)sqrt(2)) * k / glm::length(x);
        }
        curr_point->set_vel(curr_point->get_vel() + timestep * force / vertex_mass);
    }
    for (int i=0; i<vertex_count; i++) {
        if(i!=0 && i!=col_count-1) points[i]->pos = points[i]->pos * (1.0f - damping) + points[i]->get_vel();
        vertices[i*3] = points[i]->pos.x;
        vertices[i*3+1] = points[i]->pos.y;
        vertices[i*3+2] = points[i]->pos.z;
    }
    return true;
}
