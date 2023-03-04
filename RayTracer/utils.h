#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <vector>

struct ivec2 {
    int x, y;
    ivec2(int x, int y) : x{x}, y{y} { }
    ivec2(int i) : x{i}, y{i} { }
    ivec2() : x{0}, y{0} { }
};

struct vec3 {
    float x, y, z;
    vec3(float x, float y, float z) : x{x}, y{y}, z{z} { }
    vec3(float x, float y) : x{x}, y{y}, z{0} { }
    vec3(float i) : x{i}, y{i}, z{i} { }
    vec3() : x{0}, y{0}, z{0} { }
    
    const float& operator[](const int i) const { return i == 0 ? x : (i == 1 ? y : z);}
    float operator*(const vec3 other) const {return (x * other.x + y * other.y + z * other.z);}
    vec3 operator*(const float t) const {return vec3(t * x, t * y, t * z);}
    vec3 operator+(const vec3 other) const {return vec3(x + other.x, y + other.y, z + other.z);}
    vec3 operator+=(const vec3 other) {x += other.x; y += other.y; z += other.z; return *this;}
    vec3 operator-(const vec3 other) const {return vec3(x - other.x, y - other.y, z - other.z);}
    float length() const {return sqrtf(x * x + y * y + z * z);}
    vec3 normalize() const {return vec3(x, y, z) * (1.f / length());}
};

struct Sphere {
    vec3 Position;
    float Radius;
    vec3 Albedo_Color;
    float Refractive_Comp;
    vec3 Diffuse_Color;
    float Specular;
    float Refractive_Index;
    
    Sphere(vec3 position, float radius, vec3 albedo, float rc, vec3 color, float specular, float ri) : Position{position}, Radius{radius}, Albedo_Color{albedo}, Refractive_Comp{rc}, Diffuse_Color{color} , Specular{specular}, Refractive_Index{ri} { }
    Sphere() : Radius{-1} { }
    
    bool intersects(vec3& origin, vec3& dir, float &point) {
        vec3 lDir = Position - origin;
        float tca = lDir*dir;
        float dist = lDir*lDir - tca*tca;
        if (dist > Radius*Radius) return false;
        float thc = sqrtf(Radius * Radius - dist);
        point = tca - thc;
        float t1 = tca + thc;
        if (point < 0) point = t1;
        if (point < 0) return false;
        return true;
    }
};

struct Light {
    vec3 Position;
    float Intensity;
    
    Light(vec3 position, float intensity) : Position(position), Intensity(intensity) {}
    Light() { }
};

struct Scene {
    std::vector<Sphere> Spheres;
    std::vector<Light> Lights;
    
    Scene() { }
};

#endif
