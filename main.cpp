#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>                 
#include <glm/gtx/string_cast.hpp>    
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particle {
    public:
     glm::vec3 position;
     glm::vec3 velocity;
     glm::vec3 acceleration;
     float mass;
     std::string name;
     int color;

     void updatePosition(float deltaTime) {
                position += velocity * deltaTime + 0.5f * acceleration * deltaTime * deltaTime;
            }    

    void updateVelocity(float deltaTime, const glm::vec3& newAcceleration) {
                velocity +=  0.5f * (acceleration + newAcceleration) * deltaTime; 
                acceleration = newAcceleration;
            }            
};

void resetParticles(std::vector<Particle>& particles, const float G) {
    particles[0].position = glm::vec3(0.0f);
    particles[0].velocity = glm::vec3(0.0f);
    particles[0].mass = 1000.0f;

    particles[1].position = glm::vec3(0.2f, 0.0f, 0.0f);
    particles[1].mass = 1.0f;

    particles[2].position = glm::vec3(-1.0f, 0.5f, 0.0f);
    particles[2].mass = 0.815f;

    particles[3].position = glm::vec3(-0.3f, 0.2f, 0.0f);
    particles[3].mass = 0.055f;

    // calculate orbital velocities around sun

    float ellipticalFactors[] = { 0.0f, 0.92f, 0.88f, 0.85f };

    for (int i = 1; i < particles.size(); ++i) {
        glm::vec3 r = particles[i].position - particles[0].position;
        float circularSpeed = sqrt(G * particles[0].mass / glm::length(r));

        float ellipticalFactor = 0.85f + 0.05f * i;
        float speed = circularSpeed * ellipticalFactors[i];

        glm::vec3 dir(r.y, -r.x, 0.02f * i);
        dir = glm::normalize(dir);
        particles[i].velocity = dir * speed;
    }
}

float radius = 5.0f;
float yaw = -90.0f; // horizontal angle, in degrees
float pitch = 0.0f;

glm::vec3 cameraPos;
glm::vec3 cameraFront; // float around the sun (like a target)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int main() {
GLFWwindow *window; 

if (!glfwInit()) {
    std::cout << "GLFW not initialized" << std::endl;
    return -1;
}

window = glfwCreateWindow(640, 480, "Particle Physics", NULL, NULL);

if(!window) {
    std::cout << "Window was not created" << std::endl;
    return -1;
}

glfwMakeContextCurrent(window);

bool paused = false;

// initialize GLEW after content creation
GLenum err = glewInit();
if(err != GLEW_OK) {
    std::cout << "GLEW initialization failed" << glewGetErrorString(err) << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
}

glEnable(GL_DEPTH_TEST);

std::vector<Particle> particles(4);

cameraFront = particles[0].position;


    for (auto& p : particles) {
      p.acceleration = glm::vec3(0.0f); // reset acceleration, as acceleration is accumalated from gravitational forces
    }

/* masses have been incredibly scaled down*/

        particles[0].name = "Sun";
        particles[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
        particles[0].acceleration = glm::vec3(0.0f);
        particles[0].mass = 1.0f;  // Sun = 1 solar mass 

        particles[1].name = "Mercury";
        particles[1].position = glm::vec3(0.39f, 0.0f, 0.0f); // 0.39 AU from sun
        particles[1].acceleration = glm::vec3(0.0f);
        particles[1].mass = 3.3e-6f; 

        particles[2].name = "Venus";
        particles[2].position = glm::vec3(-1.0f, 0.5f, 0.0f);
        particles[2].acceleration = glm::vec3(0.0f);
        particles[2].mass = 4.87e-6f; 

        particles[3].name = "Earth";
        particles[3].position = glm::vec3(0.72f, 0.2f, 0.0f);
        particles[3].acceleration = glm::vec3(0.0f);
        particles[3].mass = 5.97e-6f; 

// set clear color to black
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glPointSize(10.0f);

auto previousTime = std::chrono::high_resolution_clock::now();
const float G = 4.0f * M_PI * M_PI;
resetParticles(particles, G);

while(!glfwWindowShouldClose(window)) {


 std::cout << "CameraPos: " << glm::to_string(cameraPos) << " CameraFront: " << glm::to_string(cameraFront) << std::endl;
// resest acceleration each frame 
    for (auto& p : particles) {
      p.acceleration = glm::vec3(0.0f); // reset acceleration, as acceleration is accumalated from gravitational forces
    }

    // calculate deltaTime  
     auto currentTime = std::chrono::high_resolution_clock::now();
     float timeScale = 0.05f;
     float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
     deltaTime = glm::clamp(deltaTime, 0.0f, 0.008f); // max ~60fps
     deltaTime *= timeScale;
     previousTime = currentTime;

    /*
     const float cameraSpeed = 2.5f * deltaTime; // adjust speed
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;         
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;  
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;   
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            cameraPos += cameraFront * cameraSpeed;
    */ 

     std::vector<glm::vec3> oldAcceleration(particles.size(), glm::vec3(0.0f));

     // evaluate gravitational forces
     for(int i = 0; i < particles.size(); ++i) {
        for(int j = i + 1; j < particles.size(); ++j) {
            if (i == j) continue;
            
            glm::vec3 r = particles[j].position - particles[i].position; // evaluates the vector from i -> j
            float distance = glm::length(r);
            if (distance < 1e-5f) continue; 
            glm::vec3 direction = glm::normalize(r);  // gives direction of the force

            float force = G * (particles[i].mass * particles[j].mass) / (distance * distance); // graviational force
            glm::vec3 fVec = force * direction;

            oldAcceleration[i] += (fVec / particles[i].mass); // a = F / m
            oldAcceleration[j] -= (fVec / particles[j].mass); // a = F / m
        }        
    }
    for (int i = 0; i < particles.size(); ++i) {
       if (i == 0) continue;
        particles[i].updatePosition(deltaTime);
    }    
     
std::vector<glm::vec3> newAcceleration(particles.size(), glm::vec3(0.0f));

    // evaluate gravitational forces
     for(int i = 0; i < particles.size(); ++i) {
        for(int j = i + 1; j < particles.size(); ++j) {
            if (i == j) continue;
            
            glm::vec3 r = particles[j].position - particles[i].position; // evaluates the vector from i -> j
            float distance = glm::length(r);
            if (distance < 1e-5f) continue; 
            glm::vec3 direction = glm::normalize(r);  // gives direction of the force

            float force = G * (particles[i].mass * particles[j].mass) / (distance * distance); // graviational force
            glm::vec3 fVec = force * direction;

            newAcceleration[i] += (fVec / particles[i].mass); // a = F / m
            newAcceleration[j] -= (fVec / particles[j].mass);
        }        
    }

    for (int i = 0; i < particles.size(); ++i) {
            if (i == 0) continue;
       particles[i].updateVelocity(deltaTime, newAcceleration[i]);
    }    

  if(!paused) {
    for (auto& p : particles) {
        if(glm::length(p.position) > 100.0f) {
            p.velocity = glm::vec3(0.0f);
        }

        }
  } 

// clamp pitch to prevent gimbal lock
pitch = glm::clamp(pitch, -89.0f, 89.0f);

// convert spherical coordinates to caresian
float yawRad = glm::radians(yaw);
float pitchRad = glm::radians(pitch);

cameraPos.x = radius * cos(pitchRad) * cos(yawRad);
cameraPos.y = radius * sin(pitchRad);
cameraPos.z = radius * cos(pitchRad) * sin(yawRad);

// always face the center (sun)
glm::vec3 cameraDirection = glm::normalize(cameraFront - cameraPos);

glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);

glm::mat4 projection = glm::perspective(glm::radians(75.0f), 640.0f / 480.0f, 0.1f, 100.0f);

glMatrixMode(GL_PROJECTION);
glLoadMatrixf(glm::value_ptr(projection));

glMatrixMode(GL_MODELVIEW);
glLoadMatrixf(glm::value_ptr(view));

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_POINTS);
    
    // Red particle (Alpha)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(particles[0].position.x, particles[0].position.y, particles[0].position.z);    

    // Yellow particle (Beta)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(particles[1].position.x, particles[1].position.y, particles[1].position.z);    

    // Blue particle (Gamma)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(particles[2].position.x, particles[2].position.y, particles[2].position.z);    

    // Green particle (Delta)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(particles[3].position.x, particles[3].position.y, particles[3].position.z);    


    glEnd();

    glfwPollEvents();

    const float angleSpeed = 50.0f * deltaTime;
    const float zoomSpeed = 5.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += angleSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= angleSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= angleSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += angleSpeed;
    
    // zoom in/out
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        radius -= zoomSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        radius += zoomSpeed;

    // clamp zoom to prevent flipping
    radius = glm::clamp(radius, 1.0f, 100.0f);    

    // Escape to close the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Space to pause the window
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        paused = !paused;
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // spam prevention
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        resetParticles(particles, G);
        for (auto& p : particles) {
            p.acceleration = glm::vec3(0.0f);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // spam prevention
    }

    glfwSwapBuffers(window);

        }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

    }
    