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

// Key Callback
void key_callback(GLFWwindow* window, int key, int scancode, 
                  int action, int mode);

// Window
GLFWwindow* window;

// Window size
const GLuint WIDTH = 800, HEIGHT = 600;

// Cloth attribute
float cloth_size = 10.0f;
int row_count = (int)cloth_size + 1;
int col_count = (int)cloth_size + 1;
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
    
    GLfloat vertices[vertex_count*3];
    // Vertices list
    for (int i=0; i<row_count; i++) {
        int stride = i * col_count * 3;
        int grid_size = cloth_size / (col_count-1);
        for (int j=0; j<col_count; j++) {
            vertices[stride + j*3] = grid_size * i;
            vertices[stride + j*3 + 1] = grid_size * j;
            vertices[stride + j*3 + 2] = 0.0f;
        }
    }
    std::cout << "count: " << vertex_count << std::endl;

    int index_count = (row_count - 1) * (col_count - 1) * 6;
    GLuint indices[index_count];
    // Index list
    for (int i=0; i<row_count-1; i++) {
        int stride = i * (col_count-1) * 6; 
        for (int j=0; j<col_count-1; j++) {
            indices[stride + j*6] = i * col_count + j;
            indices[stride + j*6 + 1] = i * col_count + j + 1;
            indices[stride + j*6 + 2] = (i + 1) * col_count + j;

            indices[stride + j*6 + 3] = i * col_count + j + 1;
            indices[stride + j*6 + 4] = (i + 1) * col_count + j;
            indices[stride + j*6 + 5] = (i + 1) * col_count + j + 1;

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw
        shader_program.Use();
        
        // Perspective projection
        glm::mat4 projection = glm::perspective(glm::radians(25.0f), 
                                                (float)WIDTH/(float)HEIGHT, 
                                                0.1f, 100.0f);
		// Camera matrix
		glm::mat4 view = glm::lookAt(
			glm::vec3(0, 0, 3),
		    glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );
        // Model matrix
        glm::mat4 model;
        float scale = 0.5f;
        model = glm::scale(model, glm::vec3(scale * 1.0f/cloth_size, 
                                            scale * 1.0f/cloth_size, 
                                            0.1f));
        model = glm::translate(model, glm::vec3(-cloth_size/2.0f, 
                                                -cloth_size/2.0f, 0.0f));

        // Put transformation matrics together
        glm::mat4 mvp = projection * view * model;
        GLint mvp_loc = glGetUniformLocation(shader_program.Program, "mvp");
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

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
