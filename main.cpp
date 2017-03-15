/**************************************
 * CMU 15664 Technical Animation
 * Mini Project 2
 * Cloth Simulation
 *
 * Author: Chaoying Du
 * E-mail: chaoyind@andrew.cmu.edu
 * Date: Mar.8 2017
 */

#include <iostream>
#include <vector>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// Glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Shader loading
#include "shader.h"
#include "cloth.h"

// Key Callback
void key_callback(GLFWwindow* window, int key, int scancode, 
                  int action, int mode);

// Transform
void view_transform(Shader shader_program, float grid_size,
                    int row_count, int col_count);

// Window
GLFWwindow* window;

// Window size
const GLuint WIDTH = 800, HEIGHT = 600;

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Important in Mac
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "Cloth", NULL, NULL);
    if (window == NULL)
    {
            fprintf(stderr, "Failed to create GLFW window\n");
            glfwTerminate();
            return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Shader reading
    Shader shader_program("vs.glsl", "fs.glsl");
    Cloth* cloth = new Cloth(20, 20);
    std::vector<GLfloat> vertices = cloth->get_vertices();
    std::vector<int> indices = cloth->get_indices();

    // Vertex buffer objects, Vertex array objects
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices[0]), 
                 &vertices[0], GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(indices[0]), 
                 &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                          3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Some attributes
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        //std::cout << cloth->get_vertices()[1] << std::endl;
        
        glfwPollEvents();
        // Render
        glClearColor(.2f, .3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw
        shader_program.Use();
        cloth->update_points(vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices[0]), 
                     &vertices[0], GL_DYNAMIC_DRAW);
        view_transform(shader_program, cloth->get_grid_size(),
                       cloth->get_row_count(), cloth->get_col_count());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
    }
    // De-allocate
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, 
                  int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void view_transform(Shader shader_program, float grid_size, 
                    int row_count, int col_count) {
    // Perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(25.0f), 
                                            (float)WIDTH/(float)HEIGHT, 
                                            0.1f, 100.0f);
    // Camera matrix
    glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 3),
        glm::vec3(0, 0, 0),
        glm::vec3(0, -1, 0)
    );
    // Model matrix
    glm::mat4 model;
    float scale = 0.5f;
    model = glm::scale(model, glm::vec3(0.5f));
    model = glm::translate(model, glm::vec3(-grid_size*col_count/2.0f, 
                                            -grid_size*row_count/2.0f, 0.0f));

    // Put transformation matrics together
    glm::mat4 mvp = projection * view * model;
    GLint mvp_loc = glGetUniformLocation(shader_program.Program, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
}
