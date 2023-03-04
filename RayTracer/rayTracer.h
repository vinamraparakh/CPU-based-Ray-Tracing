#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <fstream>
#include <vector>
#include <limits>
#include <math.h>
#include <future>
#include <thread>
#include <chrono>

#include "stb_image.h"
#include "utils.h"
#include "camera.h"
#define tfov2 0.579
#define REFLECTION_COUNT 4
#define CLIPPING_DISTANCE 1000.0f
//#define MT
#define BG
vec3 BG_COLOR(0.2, 0.4, 0.8);

class RayTracer {
public:
    static void Init(ivec2 dimensions = ivec2(0)) {
        SetDimensions(dimensions);
#ifdef BG
        generateBG("/Users/anirudh/Desktop/OpenGLProjects/RayTracer/Resources/images/envmap.jpg");
#endif
        GenerateScene();
    }
    static void RenderScene(Camera& camera) {
        DrawToFile(scene.Spheres, scene.Lights, camera);
    }
    
    static void GenerateScene() {
        //green
        scene.Spheres.push_back(Sphere(vec3(-2, -3, -26), 4, vec3(0.4, 0.2, 0.0), 0.0, vec3(0.2, 0.7, 0.8), 40, 1.0));
        //purple
        scene.Spheres.push_back(Sphere(vec3(3, -4, -20), 1,  vec3(0.6, 0.2, 0), 0.0, vec3(0.4, 0.2, 0.8), 10, 1.0));
        //mirror
        scene.Spheres.push_back(Sphere(vec3(5, 5, -30), 5,  vec3(0.0, 10.0, 0.9), 0.0, vec3(1, 1, 1), 1500, 1.0));
        //glass
        scene.Spheres.push_back(Sphere(vec3(-4.5, 2, -18), 2,  vec3(0.0, 0.2, 0.1), 0.8, vec3(0.6, 0.7, 0.8), 125, 1.5));
        //light blue
        scene.Spheres.push_back(Sphere(vec3(-7, 3, -28), 1, vec3(0.4, 0.2, 0.1), 0.0, vec3(0.3, 0.3, 0.6), 40, 1.0));
        //red
        scene.Spheres.push_back(Sphere(vec3(-10, 3, -28), 1, vec3(0.4, 0.2, 0.1), 0.0, vec3(0.8, 0.1, 0.2), 40, 1.0));
        scene.Spheres.push_back(Sphere(vec3(8, 5, -25), 0.75, vec3(0.4, 0.2, 0.1), 0.0, vec3(0.0, 1.0, 0.2), 40, 1.0));
        //scene.Spheres.push_back(Sphere(vec3(3, 3, -23), 0.5, vec3(0.4, 0.2, 0.1), 0.0, vec3(0.8, 0.1, 0.8), 40, 1.0));
        
        scene.Lights.push_back(Light(vec3(20, 20, -10), 0.6));
        scene.Lights.push_back(Light(vec3(-15, -15, 20), 0.4));
    }
    
    static void SetDimensions(ivec2 dimensions) {
        WIDTH = dimensions.x;
        HEIGHT = dimensions.y;
    }

private:
    inline static unsigned char* background_data;
    inline static int WIDTH, HEIGHT;
    inline static int ENV_WIDTH, ENV_HEIGHT;
    inline static Scene scene;
    inline static std::mutex mutex;
    inline static std::condition_variable cv;
    inline static std::atomic<int> completedThreads{0};
    inline static int renderTime;
    
    static void generateBG(const char* path) {
        int nrChannels;
        background_data = stbi_load(path, &ENV_WIDTH, &ENV_HEIGHT, &nrChannels, STBI_default);
    }

    static vec3 getBGColor(int pixel) {
#ifdef BG
        return vec3((float)background_data[pixel * 3] / 510, (float)background_data[pixel * 3 + 1] / 510, (float)background_data[pixel * 3 + 2] / 510);
#else
        return BG_COLOR;
#endif
        
    }
    
    static bool sceneIntersects(vec3& origin, vec3& dir, std::vector<Sphere>& spheres, vec3 &point, vec3& normal, Sphere& sphere) {
        float sDist = std::numeric_limits<float>::max();
        for(int i = 0; i < spheres.size(); i++) {
            float dist_i;
             if (spheres[i].intersects(origin, dir, dist_i) && dist_i < sDist) {
                 sDist = dist_i;
                 point = origin + dir*dist_i;
                 sphere = spheres[i];
                 normal = (point - spheres[i].Position).normalize();
             }
        }
        return sDist < CLIPPING_DISTANCE;
    }

    static vec3 refract(vec3& incident, vec3 normal, float ri) {
        float cosi = incident * normal;
        float ni = 1;
        if(cosi > 0.0f) {
            cosi = -cosi;
            std::swap(ni, ri);
            normal = normal * -1.0f;
        }
        float u = ni / ri;
        float k = 1.0f - u * u * (1.0f - cosi * cosi);
        if(k < 0.0f)
            return vec3(1.0, 0.0, 0.0);
        else
            return (incident * u - normal * (sqrtf(k) - u * cosi));
    }

    static vec3 reflect(vec3 incident, vec3 normal) {
        return incident - normal * (2.0f * (incident * normal));
    }
    
    static vec3 castRay(vec3 origin, vec3& dir, std::vector<Sphere>& spheres, std::vector<Light>& lights, int pixel, int depth = 0) {
        vec3 point, normal;
        Sphere sphere;
        if(depth < REFLECTION_COUNT && sceneIntersects(origin, dir, spheres, point, normal, sphere)) {
            float diffuse_light_intensity = 0;
            float specular_light_intensity = 0;
            
            vec3 reflectDir = reflect(dir, normal).normalize();
            vec3 refractDir = refract(dir, normal, sphere.Refractive_Index).normalize();
            vec3 reflectOrigin = reflectDir * normal < 0 ? point - normal * 1e-3 : point + normal * 1e-3;
            vec3 refractOrigin = refractDir * normal < 0 ? point - normal * 1e-3 : point + normal * 1e-3;
            vec3 reflectColor = castRay(reflectOrigin, reflectDir, spheres, lights, pixel, depth + 1);
            vec3 refractColor(0, 0, 0);
            if(sphere.Refractive_Comp > 0.1f)
                refractColor = castRay(refractOrigin, refractDir, spheres, lights, pixel, depth + 1);
            
            for (int i = 0; i < lights.size(); i++) {
                
                vec3 light_dir = (lights[i].Position - point).normalize();
                float light_distance = (lights[i].Position - point).length();

                vec3 shadowOrigin = light_dir * normal < 0 ? point - normal * 1e-3 : point + normal * 1e-3;
                vec3 shadowPoint, shadowNormal;
                Sphere shadowSphere;
                
                 if (sceneIntersects(shadowOrigin, light_dir, spheres, shadowPoint, shadowNormal, shadowSphere) && (shadowPoint - shadowOrigin).length() < light_distance)
                     continue;
                
                diffuse_light_intensity  += lights[i].Intensity * std::max(0.f, light_dir * normal);
                specular_light_intensity += powf(std::max(0.f, reflect(light_dir * -1.0f, normal) * dir * -1.0f), sphere.Specular)*lights[i].Intensity;
            }

            return sphere.Diffuse_Color * diffuse_light_intensity * sphere.Albedo_Color[0] + vec3(1, 1, 1) * specular_light_intensity * sphere.Albedo_Color[1] + reflectColor * sphere.Albedo_Color[2] + refractColor * sphere.Refractive_Comp;
        }
#ifdef BG
        int x = (int)(((atan2(dir.z, dir.x) / (2.0f * 3.14f)) + 0.5f) * ENV_WIDTH + (int)((2.3f * ENV_WIDTH + origin.x * CLIPPING_DISTANCE) / ( 2.0f * 3.14f))) % ENV_WIDTH;
        int y = acos(dir.y) / 3.14f * ENV_HEIGHT;
        x = std::max(0, std::min(x, ENV_WIDTH - 1));
        y = std::max(0, std::min(y, ENV_HEIGHT - 1));
        
        return getBGColor(x + y * ENV_WIDTH);
#else
        return getBGColor(1);
#endif
    }

    static void mtRender(std::vector<vec3>& framebuffer, std::vector<Sphere>& spheres, std::vector<Light>& lights, Camera& camera, int rowMin, int rowMax) {
        
        for (int j = rowMin; j < rowMax; j++) {
            for (int i = 0; i<WIDTH; i++) {
                
                float x =  (2*(i + 0.5)/(float)WIDTH  - 1)*tfov2*WIDTH/(float)HEIGHT;
                float y = -(2*(j + 0.5)/(float)HEIGHT - 1)*tfov2;
                vec3 dir = vec3(x, y, -1).normalize();
                
                framebuffer[i+j*WIDTH] = castRay(camera.GetPosition(), dir, spheres, lights, i + j * WIDTH);
            }
        }
        
        {
            std::lock_guard<std::mutex> lock(mutex);
            completedThreads++;
            cv.notify_one();
        }
    }
    
    static void render(std::vector<vec3>& framebuffer, std::vector<Sphere>& spheres, std::vector<Light>& lights, Camera& camera) {
        
        for (int j = 0; j < HEIGHT; j++) {
            for (int i = 0; i<WIDTH; i++) {
                
                float x =  (2*(i + 0.5)/(float)WIDTH  - 1)*tfov2*WIDTH/(float)HEIGHT;
                float y = -(2*(j + 0.5)/(float)HEIGHT - 1)*tfov2;
                vec3 dir = vec3(x, y, -1).normalize();
                
                framebuffer[i+j*WIDTH] = castRay(camera.GetPosition(), dir, spheres, lights, i + j * WIDTH);
            }
        }
    }
    
    static void DrawToFile(std::vector<Sphere>& spheres, std::vector<Light>& lights, Camera& camera) {
        std::vector<vec3> framebuffer(WIDTH * HEIGHT);
        auto begin = std::chrono::high_resolution_clock::now();
        
#ifdef MT
        int maxThreads = std::thread::hardware_concurrency() + 1;
        int rowsPerThread = HEIGHT / maxThreads;
        int leftOverRows = HEIGHT % maxThreads;
        
        std::vector<std::thread> threads;
        
        completedThreads = 0;
        
        std::thread t([&framebuffer, &spheres, &lights, &camera, &rowsPerThread, &maxThreads, &leftOverRows]() {
            mtRender(framebuffer, spheres, lights, camera, 0, rowsPerThread + leftOverRows);
        });
        threads.push_back(std::move(t));
        
        for(int i = 1; i < maxThreads; i++) {
            int startRow = i * rowsPerThread + leftOverRows;
            
            std::thread t([&framebuffer, &spheres, &lights, &camera, &rowsPerThread, &maxThreads, startRow]() {
                mtRender(framebuffer, spheres, lights, camera, startRow, startRow + rowsPerThread);
            });
            threads.push_back(std::move(t));
        }
        
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [&maxThreads] {
                return completedThreads == maxThreads;
            });
        }
        
        for (std::thread& t : threads)
        {
            t.join();
        }
#else
        render(framebuffer, spheres, lights, camera);
#endif
        
        auto finish = std::chrono::high_resolution_clock::now();
        renderTime = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(finish - begin).count());
        std::cout << renderTime << std::endl;
        
        std::ofstream texture;
        texture.open("/Users/anirudh/Desktop/OpenGLProjects/RayTracer/Resources/images/out.ppm", std::ios::binary);
        texture << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
        for (int i = 0; i < HEIGHT * WIDTH; i++) {
            for (int j = 0; j < 3; j++) {
                texture << (char)(510 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
            }
        }
        texture.close();
    }

};

#endif
