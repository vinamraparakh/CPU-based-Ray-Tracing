#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include "stb_image.h"

enum class TextureFormats {
    JPG,
    PNG,
    PPM
};

class Texture {
public:
    GLuint ID = 0;
    
    Texture(const char* imagePath, TextureFormats format, bool persistentData = false) {
        specs.reset(new ImageSpecs());
        this->generateTexture(imagePath, format);
        
        // delete image data and image specs if not required to be persistent
        // ------------------------------------------------------------------
        if(!persistentData) {
            if(specs->data) {
                stbi_image_free(specs->data);
                specs->data = nullptr;
            }
            
        }
    }
    // empty texture
    // -------------
    Texture() {
        specs = NULL;
    }
    ~Texture() {
        glDeleteTextures(1, &ID);
    }
    
    void Bind() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
    
    static void UnBind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    int GetTexWidth() const {
        if(specs != nullptr) {
            return specs->width;
        }
        
        return -1;
    }
    
    int GetTexHeight() const {
        if(specs != nullptr) {
            return specs->height;
        }
        
        return -1;
    }
    
    bool DeleteImageData()
    {
        if(specs != nullptr && specs->data != nullptr) {
            delete specs->data;
            specs->data = nullptr;
            return true;
        }
        
        return false;
    }
    
private:
    struct ImageSpecs {
        int width;
        int height;
        int nrChannels;
        unsigned char* data;
        
        ImageSpecs() : width{0}, height{0}, nrChannels{0}, data{nullptr} {}
    };
    std::shared_ptr<ImageSpecs> specs;
    
    void generateTexture(const char* path, TextureFormats format) {
        
        stbi_set_flip_vertically_on_load(true);
        specs->data = stbi_load(path, &specs->width, &specs->height, &specs->nrChannels, format == TextureFormats::PNG ? STBI_rgb_alpha : STBI_default);
        
        if(specs->data) {
            glGenTextures(1, &this->ID);
            glBindTexture(GL_TEXTURE_2D, this->ID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            glTexImage2D(GL_TEXTURE_2D, 0, format == TextureFormats::PNG ? GL_RGBA : GL_RGB, specs->width, specs->height, 0, format == TextureFormats::PNG ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, specs->data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "ERROR | Failed to load texture" << std::endl;
        }
    }
};
#endif
