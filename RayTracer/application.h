#ifndef APPLICATION_H
#define APPLICATION_H

#include "rayTracer.h"

const char* outImagePath = "/Users/anirudh/Desktop/OpenGLProjects/RayTracer/Resources/images/out.ppm";
const char* vertexShaderPath = "/Users/anirudh/Desktop/OpenGLProjects/RayTracer/Resources/shaders/vertex.vs";
const char* fragmentShaderPath = "/Users/anirudh/Desktop/OpenGLProjects/RayTracer/Resources/shaders/fragment.fs";

class Application {
public:
    Application(ivec2 dimensions) {
        KeyMap[GLFW_KEY_W] = 0;
        KeyMap[GLFW_KEY_A] = 1;
        KeyMap[GLFW_KEY_S] = 2;
        KeyMap[GLFW_KEY_D] = 3;
        KeyMap[GLFW_KEY_UP] = 4;
        KeyMap[GLFW_KEY_LEFT] = 5;
        KeyMap[GLFW_KEY_DOWN] = 6;
        KeyMap[GLFW_KEY_RIGHT] = 7;
        KeyMap[GLFW_KEY_J] = 8;
        KeyMap[GLFW_KEY_L] = 9;
        
        camera = new Camera();
        RayTracer::Init(dimensions);
        RayTracer::RenderScene(*camera);
        
        texture = new Texture(outImagePath, TextureFormats::PPM);
        shader = new Shader(vertexShaderPath, fragmentShaderPath);
        
        renderedImage = new RenderedImage(*shader, *texture, dimensions);
    }
    ~Application() {
        glDeleteShader(shader->ID);
    }
    
    void Draw() const {
        renderedImage->Draw();
    }
    
    void Update(GLfloat deltaTime) {
        this->deltaTime = deltaTime;
    }
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if(action == GLFW_PRESS || action == GLFW_REPEAT) {
            if(key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, 1);
            }
            else {
                switch(key) {
                    case GLFW_KEY_UP:
                    case GLFW_KEY_W:
                        if(Keys[KeyMap[key]] == false) {
                            camera->MoveVertical(deltaTime, 1);
                            reRender();
                            Keys[KeyMap[key]] = true;
                        }
                        break;
                    case GLFW_KEY_DOWN:
                    case GLFW_KEY_S:
                        if(Keys[KeyMap[key]] == false) {
                            camera->MoveVertical(deltaTime, -1);
                            reRender();
                            Keys[KeyMap[key]] = true;
                        }
                        break;
                    case GLFW_KEY_LEFT:
                    case GLFW_KEY_A:
                        if(Keys[KeyMap[key]] == false) {
                            camera->MoveHorizontal(deltaTime, -1);
                            reRender();
                            Keys[KeyMap[key]] = true;
                        }
                        break;
                    case GLFW_KEY_RIGHT:
                    case GLFW_KEY_D:
                        if(Keys[KeyMap[key]] == false) {
                            camera->MoveHorizontal(deltaTime, 1);
                            reRender();
                            Keys[KeyMap[key]] = true;
                        }
                        break;
                    case GLFW_KEY_J:
                        if(Keys[KeyMap[key]] == false) {
                            RayTracer::gammaCorrection += 0.05f;
                            reRender();
                            Keys[KeyMap[key]] = true;
                        }
                        break;
                    case GLFW_KEY_L:
                        if(Keys[KeyMap[key]] == false) {
                            RayTracer::gammaCorrection -= 0.05f;
                            reRender();
                            Keys[KeyMap[key]] = true;
                        }
                        break;
                }
            }
        }
        else if(action == GLFW_RELEASE) {
            if((key >= GLFW_KEY_RIGHT && key <= GLFW_KEY_UP) || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_S || key == GLFW_KEY_W || key == GLFW_KEY_J || key == GLFW_KEY_L) {
                Keys[KeyMap[key]] = false;
            }
        }
    }

    // process mouse movements
    // -----------------------
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        
    }

    // process mouse button clicks
    // ---------------------------
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        
        if(action == GLFW_PRESS) {
            
        }
        else if(action == GLFW_RELEASE) {
            
        }
    }

    // process mouse scrolls
    // ---------------------
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        
    }

    
private:
    inline static Camera* camera;
    inline static RenderedImage* renderedImage;
    inline static GLfloat deltaTime;
    inline static Shader* shader;
    inline static Texture* texture;
    
    inline static bool Keys[8] = {false};
    inline static std::map<int, int> KeyMap;
    
    inline static void reRender() {
        RayTracer::RenderScene(*camera);
        
        delete texture;
        texture = new Texture(outImagePath, TextureFormats::PPM);
        renderedImage->ResetTexture(*texture);
    }
};

#endif
