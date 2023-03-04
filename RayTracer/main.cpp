#include <iostream>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"
#include "rayTracer.h"
#include "texture.h"
#include "shader.h"
#include "renderedImage.h"
#include "camera.h"
#include "application.h"

const ivec2 SCREEN_DIMENSIONS = ivec2(720, 580);

GLfloat frameStartTime = -1.0f;
GLfloat deltaTime;

int main(int argc, const char * argv[])
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_DIMENSIONS.x, SCREEN_DIMENSIONS.y, "MT Ray tracer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, Application::key_callback);
    glfwSetCursorPosCallback(window, Application::cursor_position_callback);
    glfwSetMouseButtonCallback(window, Application::mouse_button_callback);
    glfwSetScrollCallback(window, Application::scroll_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    Application* application = new Application(SCREEN_DIMENSIONS);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window)) {
        GLfloat frameEndTime = glfwGetTime();
        if(frameStartTime <= 0.0f) {
            frameStartTime = 0.0f;
        }
        deltaTime = frameEndTime - frameStartTime;
        frameStartTime = frameEndTime;
        
        glfwPollEvents();
        
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        
        application->Update(deltaTime);
        application->Draw();
        
        glfwSwapBuffers(window);
    }
    
    delete application;
    glfwTerminate();
    return 0;
}
