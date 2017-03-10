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
// Shader loading
#include "shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, 
                  int action, int mode);

GLFWwindow* window;
int row_count = 10;
int col_count = 10;
int vertex_count = row_count * col_count;

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
    
    window = glfwCreateWindow(800, 600, "Cloth", NULL, NULL);
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

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Shader reading
    Shader myShader("vs.glsl", "fs.glsl");
    
    GLfloat vertices[vertex_count*3];
    // Vertices list
    for (int i=0; i<row_count; i++) {
        for (int j=0; j<col_count; j++) {
            vertices[i*col_count*3 + j*3] = 0.1f * i;
            vertices[i*col_count*3 + j*3 + 1] = 0.1f * j;
            vertices[i*col_count*3 + j*3 + 2] = 0.0f;
        }
    }
    std::cout << "count: " << vertex_count << std::endl;

    int index_count = (row_count - 1) * (col_count - 1) * 6;
    GLuint indices[index_count];
    // Index list
    for (int i=0; i<row_count-1; i++) {
        for (int j=0; j<col_count-1; j++) {
            indices[i*(col_count-1)*6 + j*6] = i * col_count + j;
            indices[i*(col_count-1)*6 + j*6 + 1] = i * col_count + j + 1;
            indices[i*(col_count-1)*6 + j*6 + 2] = (i + 1) * col_count + j;

            indices[i*(col_count-1)*6 + j*6 + 3] = i * col_count + j + 1;
            indices[i*(col_count-1)*6 + j*6 + 4] = (i + 1) * col_count + j;
            indices[i*(col_count-1)*6 + j*6 + 5] = (i + 1) * col_count + j + 1;

        }
    }
    
    // Vertex buffer objects, Vertex array objects
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
        glfwPollEvents();
        // Render
        glClearColor(.2f, .3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw
        myShader.Use();
        glBindVertexArray(VAO);
        //glDrawArrays(GL_POINTS, 0, vertex_count*3);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
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
