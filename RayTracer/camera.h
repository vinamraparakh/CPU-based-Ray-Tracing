#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
    Camera(vec3 position = vec3(0), float speed = 100.0f) : m_position(position), m_speed{speed} { }
    
    void MoveVertical(GLfloat dt, int direction) {
        m_position += vec3(0.0f, direction * m_speed * dt, 0.0f);
    }
    
    void MoveHorizontal(float dt, int direction) {
        m_position += vec3(direction * m_speed * dt, 0.0f, 0.0f);
    }
    
    vec3 GetPosition() const {
        return m_position;
    }
    
private:
    vec3 m_position;
    float m_speed;
};

#endif
