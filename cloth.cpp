/**************************************
 * cloth.cpp
 */

#include <iostream>
#include <vector>
#include <math.h>
#include <glm/ext.hpp>
#include "cloth.h"

#define SQRT_2 1.4142135f


Cloth::Cloth(int r, int c, int m, bool p)
    : row_count(r), col_count(c), mode(m), pin_four(p) {
    grid_size = 1.0f / (float)(std::max(r-1, c-1));
    vertex_count = r * c;
    mass = 1.0f;
    k = 2.0f;
    time = 0.0f;
    wind = false;
    
    // Vertices initialization
    for (int i=0; i<r; i++) {
        for (int j=0; j<c; j++) {
            float x = grid_size * j;
            float y, z;
            if(!p) {
                y = 0.0f;
                z = grid_size * i;
            } else {
                y = grid_size * i;
                z = 0.0f;
            }
            Point* curr_point = new Point(x, y, z);
            curr_point->set_index(i * c + j);
            if (i*c+j == 0 || i*c+j == col_count-1) 
                curr_point->pined = true;
            if (p){
                if(i*c+j == (row_count-1)*col_count 
                   || i*c+j == vertex_count-1) 
                    curr_point->pined = true;
            }
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

    // Ball initialization
    ball_center = glm::vec3(grid_size * c * 0.5f, grid_size * r * 1.8f, 0);
    ball_radius = 0.3f;

    // Constaints initialization
    if(mode == 1) get_constraints();
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
    glm::vec3 force;  // Force on each point
    glm::vec3 gravity;  // The gravity vector
    float vertex_mass = mass / vertex_count;  // Mass of each vertex
    float timestep = 0.00005f;  // Timestep
    float damping = 0.01f;  // Damping (air resistance)
    glm::vec3 wind_force = glm::vec3(0);
    gravity = 0.1f * glm::vec3(0, 9.8f, 0);
    
    for (int i=0; i<vertex_count; i++) {
        Point* curr_point = points[i];
        // Force initialization (gravity)
        curr_point->force += vertex_mass * gravity;         
        /* Get the neighbors on straight directions 
         * (up/down/left/right) */
        std::vector<int> s_neighbors = curr_point->get_s_neighbors(i, col_count, 
                                                                   row_count);
        /* Get the neighbors on diagonal directions 
         * (up_left/up_right/down_left/down_right) */
        std::vector<int> d_neighbors = curr_point->get_d_neighbors(i, col_count, 
                                                                   row_count);    
        // Spring force accumulation 
        for (int j=0; j<s_neighbors.size(); j++) {
            Point* neighbor_point = points[s_neighbors[j]];
            glm::vec3 x = neighbor_point->pos - curr_point->pos;
            curr_point->force += glm::normalize(x) 
                                 * (glm::length(x) - grid_size) 
                                 * k; 
        }
        for (int j=0; j<d_neighbors.size(); j++) {
            Point* neighbor_point = points[d_neighbors[j]];
            glm::vec3 x = neighbor_point->pos - curr_point->pos;
            curr_point->force += glm::normalize(x) 
                                 * (glm::length(x) - grid_size * SQRT_2) 
                                 * k;
        }

        /* Add wind force */
        float x_force = 0; // cos(0.8f*time) * (rand()/RAND_MAX-0.5f);
        float y_force = std::abs(sin(0.1f*time) - 0.2f);
        float z_force = std::abs(cos(sin(curr_point->pos[0]*time) - 0.8f));
        if (wind) {
            wind_force = glm::vec3(x_force, 
                                   -0.0005f * y_force, 
                                   -0.002f * z_force);
            curr_point->force += wind_force;
        }

        /* Set the velocity of each point */
        curr_point->set_acc(curr_point->force / vertex_mass);

        /* Reset force */
        curr_point->force = glm::vec3(0);
    }

    /* Position update and Object collision */
    for (int i=0; i<vertex_count; i++) {
        glm::vec3 temp = points[i]->pos;
        if(!points[i]->pined) {
            points[i]->pos = points[i]->pos + (1.0f - damping) 
                             * (points[i]->pos - points[i]->old_pos)
                             + points[i]->get_acc()*timestep;
            glm::vec3 offset = points[i]->pos - ball_center;
            if (glm::length(offset) < ball_radius) {
                points[i]->pos += glm::normalize(offset) 
                                  * (ball_radius - glm::length(offset));
            }
                
        }
        points[i]->old_pos = temp;
        vertices[i*3] = points[i]->pos.x;
        vertices[i*3+1] = points[i]->pos.y;
        vertices[i*3+2] = points[i]->pos.z;
    }
    time += 0.03f;
    return true;
}

void Cloth::wind_on() {
    if (wind) {
        wind = false;
        std::cout << "Wind mode off" << std::endl;
    }
    else {
        wind = true;
        std::cout << "Wind mode on" << std::endl;
    }
}

void Cloth::add_k() {
    if (k < 4.7f) k += 0.1f;
    std::cout << "Current k: " << k << std::endl;
}

void Cloth::reduce_k() {
    if (k > 0.2f) k -= 0.1f;
    std::cout << "Current k: " << k << std::endl;
}

void Cloth::ball_control(char input) {
    switch(input) {
        case 'I':
            ball_center -= glm::vec3(0, 0.002f, 0);
            break;
        case 'K':
            ball_center += glm::vec3(0, 0.002f, 0);
            break;
        case 'J':
            ball_center -= glm::vec3(0.002f, 0, 0);
            break;
        case 'L':
            ball_center += glm::vec3(0.002f, 0, 0);
            break;
        case 'U':
            ball_center -= glm::vec3(0, 0, 0.002f);
            break;
        case 'O':
            ball_center += glm::vec3(0, 0, 0.002f);
            break;
        case '[':
            if(ball_radius > 0.2f) ball_radius -= 0.002f;
            break;
        case ']':
            if(ball_radius < 4.0f) ball_radius += 0.002f;
            break;
    }
}

bool Cloth::update_points_constraint(std::vector<float> &vertices) {
    glm::vec3 force;  // Force on each point
    glm::vec3 gravity;  // The gravity vector
    float vertex_mass = mass / vertex_count;  // Mass of each vertex
    float timestep = 0.0001f;  // Timestep
    float damping = 0.02f;  // Damping (air resistance)
    glm::vec3 wind_force = glm::vec3(0);
    gravity = 0.1f * glm::vec3(0, 9.8f, 0);
    
    for (int i=0; i<vertex_count; i++) {
        Point* curr_point = points[i];
        curr_point->force += vertex_mass * gravity; // Force initialization (gravity)

        /* Add wind force */
        float x_force = 0;
        float y_force = std::abs(sin(0.1f*time) - 0.2f);
        float z_force = std::abs(cos(sin(curr_point->pos[0]*time) - 0.8f));
        if (wind) {
            wind_force = glm::vec3(x_force, -0.0005f * y_force, -0.002f * z_force);
            curr_point->force += wind_force;
        }

        /* Set the velocity of each point */
        curr_point->set_acc(curr_point->force / vertex_mass);

        /* Reset force */
        curr_point->force = glm::vec3(0);
    }

    /* Position update and Object collision */
    for (int i=0; i<vertex_count; i++) {
        glm::vec3 temp = points[i]->pos;
        if(!points[i]->pined) {
            points[i]->pos = points[i]->pos + (1.0f - damping) 
                             * (points[i]->pos - points[i]->old_pos)
                             + points[i]->get_acc()*timestep;
            glm::vec3 offset = points[i]->pos - ball_center;
            if (glm::length(offset) < ball_radius) {
                points[i]->pos += glm::normalize(offset) 
                                  * (ball_radius - glm::length(offset));
            }
                
        }
        points[i]->old_pos = temp;
    }

    /* Satisfy constraint */
    for (int j=0; j<10; j++) {
        for (int i=0; i<constraints.size(); i++) {
            Constraint* it = constraints[i];
            Point* a = points[it->a];
            Point* b = points[it->b];
            //std::cout << it->a << "," << it->b << std::endl;
            //getchar();
            float rest_distance = it->rest_distance;
            float distance = glm::length(a->pos - b->pos);
            if (distance > rest_distance) {
                float offset = (distance - rest_distance) / distance;
                glm::vec3 correction = 0.5f * offset * (a->pos - b->pos);
                if(!points[it->a]->pined)
                    a->pos -= correction;
                if(!points[it->b]->pined)
                    b->pos += correction;
            }

            float tear_distance = it->rest_distance * 3.0f;
            if (distance > tear_distance) {
                float offset = (distance - rest_distance) / distance;
                glm::vec3 correction = offset * (a->pos - b->pos);
                constraints.erase(constraints.begin() + i);
            }
        }
    }

    for (int i=0; i<vertex_count; i++) {
        vertices[i*3] = points[i]->pos.x;
        vertices[i*3+1] = points[i]->pos.y;
        vertices[i*3+2] = points[i]->pos.z;
    }

    time += 0.03f;
    
    return true;
}

void Cloth::get_constraints() {
    for (int i=0; i<vertex_count; i++) {
        int row = i / col_count;
        int col = i - row * col_count;
        if (col < col_count-1) {
            Constraint* right = new Constraint();
            right->rest_distance = grid_size;
            right->a = i;
            right->b = i+1;
            constraints.push_back(right);
        }
        if (row < row_count-1) {
            Constraint* down = new Constraint();
            down->rest_distance = grid_size;
            down->a = i;
            down->b = i+col_count;
            constraints.push_back(down);
        }
        if (col < col_count-1 && row < row_count-1) {
            Constraint* down_right = new Constraint();
            down_right->rest_distance = grid_size * SQRT_2;
            down_right->a = i;
            down_right->b = i+col_count+1;
            constraints.push_back(down_right);
        }
        if (row > 0 && col < col_count-1) {
            Constraint* up_right = new Constraint();
            up_right->rest_distance = grid_size * SQRT_2;
            up_right->a = i;
            up_right->b = i-col_count+1;
            constraints.push_back(up_right);
        }
        if (col < col_count-2) {
            Constraint* right = new Constraint();
            right->rest_distance = grid_size * 2;
            right->a = i;
            right->b = i+1;
            constraints.push_back(right);
        }
        if (row < row_count-2) {
            Constraint* down = new Constraint();
            down->rest_distance = grid_size * 2;
            down->a = i;
            down->b = i+col_count;
            constraints.push_back(down);
        }
        if (col < col_count-2 && row < row_count-2) {
            Constraint* down_right = new Constraint();
            down_right->rest_distance = grid_size * SQRT_2 * 2;
            down_right->a = i;
            down_right->b = i+col_count+1;
            constraints.push_back(down_right);
        }
        if (row > 1 && col < col_count-2) {
            Constraint* up_right = new Constraint();
            up_right->rest_distance = grid_size * SQRT_2 * 2;
            up_right->a = i;
            up_right->b = i-col_count+1;
            constraints.push_back(up_right);
        }
    }
}
