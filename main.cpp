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
// Cloth class
#include "cloth.h"

// Key Callback
void key_callback(GLFWwindow* window, int key, int scancode, 
                  int action, int mode);
void view_control();
bool keys[1024];

// Mouse Scroll Callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Transform
void view_transform(Shader shader_program, float grid_size,
                    int row_count, int col_count);

// Window
GLFWwindow* window;
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
float fov = 45.0f;
glm::vec3 camera_pos = glm::vec3(0, 0, 1.5f);
glm::vec3 camera_front = glm::vec3(0, 0, -1.0f);
glm::vec3 camera_up = glm::vec3(0, -1.0f, 0);
GLfloat yaw   = -90.0f;    // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch =   0.0f;
GLfloat lastX =  WIDTH  / 2.0;
GLfloat lastY =  HEIGHT / 2.0;
void reset_camera(int view);

// Frames
GLfloat delta_time = 0.0f;
GLfloat last_frame = 0.0f;

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
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
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
    Cloth* cloth = new Cloth(30, 30);
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

    // Init FPS parameters
    int nb_frames = 0;

    // Last key press
    bool last_z_pressed = false;
    bool last_x_pressed = false;
    bool last_c_pressed = false;
        
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Get FPS
        float curr_frame = glfwGetTime();
        nb_frames++;
        if (curr_frame - last_frame >= 1.0 ) {
            printf("%d f/s\n", nb_frames);
            nb_frames = 0;
            last_frame += 1.0;
            delta_time = curr_frame - last_frame;
        }
       
        // Check if any events have been activited
        glfwPollEvents();
        view_control();
        if (keys[GLFW_KEY_Z] && !last_z_pressed) {
            cloth->wind_on();
        }
        
        // Some cloth controlling
        last_z_pressed = keys[GLFW_KEY_Z];
        if (keys[GLFW_KEY_R]) {
            reset_camera(1);
        }
        if (keys[GLFW_KEY_T]) {
            reset_camera(2);
        }
        if (keys[GLFW_KEY_X] && !last_x_pressed) {
            cloth->add_k();
        }
        if (keys[GLFW_KEY_C] && !last_c_pressed) {
            cloth->reduce_k();
        }
        last_x_pressed = keys[GLFW_KEY_X];
        last_c_pressed = keys[GLFW_KEY_C];
        if (keys[GLFW_KEY_I]) cloth->ball_control('I');
        if (keys[GLFW_KEY_K]) cloth->ball_control('K');
        if (keys[GLFW_KEY_J]) cloth->ball_control('J');
        if (keys[GLFW_KEY_L]) cloth->ball_control('L');
        if (keys[GLFW_KEY_U]) cloth->ball_control('U');
        if (keys[GLFW_KEY_O]) cloth->ball_control('O');
        if (keys[GLFW_KEY_LEFT_BRACKET]) cloth->ball_control('[');
        if (keys[GLFW_KEY_RIGHT_BRACKET]) cloth->ball_control(']');

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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (action == GLFW_PRESS) keys[key] = true;
    else if (action == GLFW_RELEASE) keys[key] = false;
}

void view_control() {
	// Camera controls
    GLfloat cameraSpeed = 5.0f * delta_time;
    if (keys[GLFW_KEY_W]) camera_pos -= cameraSpeed * camera_up;
    if (keys[GLFW_KEY_S]) camera_pos += cameraSpeed * camera_up;
    if (keys[GLFW_KEY_A]) camera_pos -= glm::normalize(glm::cross(camera_front,
                                                      camera_up)) * cameraSpeed;
    if (keys[GLFW_KEY_D]) camera_pos += glm::normalize(glm::cross(camera_front,
                                                      camera_up)) * cameraSpeed;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (fov >= 1.0f && fov <= 45.0f) fov -= yoffset;
    if (fov <= 1.0f) fov = 1.0f;
    if (fov >= 45.0f) fov = 45.0f;
}

void view_transform(Shader shader_program, float grid_size, 
                    int row_count, int col_count) {
    // Perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(fov), 
                                            (float)WIDTH/(float)HEIGHT, 
                                            0.1f, 100.0f);
    // Camera matrix
    glm::mat4 view = glm::lookAt(camera_pos, 
                                 camera_pos + camera_front, camera_up);
    // Model matrix
    glm::mat4 model;
    float scale = 0.5f;
    model = glm::scale(model, glm::vec3(0.5f));
    model = glm::translate(model, glm::vec3(-grid_size*col_count/2.0f, 
                                            -grid_size*row_count/1.2f, 0));

    // Put transformation matrics together
    glm::mat4 mvp = projection * view * model;
    GLint mvp_loc = glGetUniformLocation(shader_program.Program, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
}

bool first_mouse = false;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (keys[GLFW_KEY_E]) {
		if (first_mouse) {
			lastX = xpos;
			lastY = ypos;
			first_mouse = false;
		}
	  
		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos; 
		lastX = xpos;
		lastY = ypos;

		GLfloat sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw   += xoffset;
		pitch += yoffset;

		if(pitch > 89.0f)
			pitch = 89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camera_front = glm::normalize(front);
	}
}  

void reset_camera(int view) {
	yaw   = -90.0f;    // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	pitch =   0.0f;
	lastX =  WIDTH  / 2.0;
	lastY =  HEIGHT / 2.0;
    fov = 45.0f;
    switch(view) {
        case 1:
            camera_pos = glm::vec3(0, 0, 1.5f);
            camera_front = glm::vec3(0, 0, -1.0f);
            camera_up = glm::vec3(0, -1.0f, 0);
            break;
        case 2:
            camera_pos = glm::vec3(-1.2f, -0.1f, 0.5f);
            camera_front = glm::vec3(1.0f, 0, -0.5f);
            camera_up = glm::vec3(0, -1.0f, 0);
            break;
        }
}
