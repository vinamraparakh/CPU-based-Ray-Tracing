#ifndef RENDERED_IMAGE_H
#define RENDERED_IMAGE_H

#include <GL/glew.h>

#include "texture.h"
#include "shader.h"

class RenderedImage {
public:
    RenderedImage(Shader& shader, Texture& texture, ivec2 dimensions)
    : m_shader{shader}, m_texture{texture}, m_dimensions{dimensions} {
        this->setup();
    }
    
    ~RenderedImage() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
    
    void Draw() const {
        m_shader.Use();
        glBindVertexArray(m_VAO);
        m_texture.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        Texture::UnBind();
    }
    
    void ResetTexture(Texture& texture) {
        m_texture = texture;
    }
    
private:
    unsigned int m_VBO, m_VAO, m_EBO;
    Shader m_shader;
    Texture m_texture;
    ivec2 m_dimensions;
    
    void setup() {
        float vertices[] = {
            // positions         // texture coords
              1.0f,   1.0f,  1.0f, 1.0f,   // top right
              1.0f,  -1.0f,  1.0f, 0.0f,   // bottom right
             -1.0f,  -1.0f,  0.0f, 0.0f,   // bottom left
             -1.0f,   1.0f,  0.0f, 1.0f    // top left
        };
        unsigned int indices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };
        
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};


#endif
