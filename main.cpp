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
    particles[0].mass = 10000.0f;

    particles[1].position = glm::vec3(0.2f, 0.0f, 0.0f);
    particles[1].mass = 1.0f;

    particles[2].position = glm::vec3(-1.0f, 0.5f, 0.0f);
    particles[2].mass = 58.0f;

    particles[3].position = glm::vec3(-0.3f, 0.2f, 0.0f);
    particles[3].mass = 317.0f;

    // calculate orbital velocities around sun
    for (int i = 1; i < particles.size(); ++i) {
        glm::vec3 r = particles[i].position - particles[0].position;
        float speed = sqrt(G * particles[0].mass / glm::length(r));
        glm::vec3 dir(r.y, -r.x, 0.0f);
        dir = glm::normalize(dir);
        particles[i].velocity = dir * speed;
    }
}


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

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(-5.0, 5.0, -5.0, 5.0, -1.0, 1.0);  
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

bool paused = false;

// initialize GLEW after content creation
GLenum err = glewInit();
if(err != GLEW_OK) {
    std::cout << "GLEW initialization failed" << glewGetErrorString(err) << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
}

std::vector<Particle> particles(4);

    for (auto& p : particles) {
      p.acceleration = glm::vec3(0.0f); // reset acceleration, as acceleration is accumalated from gravitational forces
    }

/* masses have been incredibly scaled down*/

        particles[0].name = "Sun";
        particles[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
        particles[0].velocity = glm::vec3(0.0f, 0.0f, 0.f);
        particles[0].acceleration = glm::vec3(0.0f);
        particles[0].mass = 1000.0f; 

        particles[1].name = "Mercury";
        particles[1].position = glm::vec3(0.2f, 0.0f, 0.0f);
        particles[1].velocity = glm::vec3(1.0f, 2.0f, 0.f);
        particles[1].acceleration = glm::vec3(0.0f);
        particles[1].mass = 1.0f; 


        particles[2].name = "Venus";
        particles[2].position = glm::vec3(-1.0f, 0.5f, 0.0f);
        particles[2].velocity = glm::vec3(0.5f, 0.0f, 0.f);
        particles[2].acceleration = glm::vec3(0.0f);
        particles[2].mass = 10.0f; 

        particles[3].name = "Earth";
        particles[3].position = glm::vec3(-0.3f, 0.2f, 0.0f);
        particles[3].velocity = glm::vec3(2.0f, 0.5f, 0.f);
        particles[3].acceleration = glm::vec3(0.0f);
        particles[3].mass = 20.0f; 

// set clear color to black
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glPointSize(10.0f);

auto previousTime = std::chrono::high_resolution_clock::now();
const float G = 0.05f;
resetParticles(particles, G);

/** 
// Set Mercury's orbital velocity
glm::vec3 r1 = particles[1].position - particles[0].position;
float orbitalVelocity = sqrt(G * particles[0].mass / glm::length(r1)); 
glm::vec3 orbitalDirection (-r1.y, r1.x, 0.0f);
orbitalDirection = glm::normalize(orbitalDirection);
particles[1].velocity = orbitalDirection * orbitalVelocity;

// Venus 
glm::vec3 r2 = particles[2].position - particles[0].position;
float v2 = sqrt(G * particles[0].mass / glm::length(r2)); 
glm::vec3 orbitalDirection2 (-r2.y, r2.x, 0.0f);
particles[2].velocity = glm::normalize(orbitalDirection2) * v2;

// Earth
glm::vec3 r3 = particles[3].position - particles[0].position;
float v3 = sqrt(G * particles[0].mass / glm::length(r3)); 
glm::vec3 orbitalDirection3 (-r3.y, r3.x, 0.0f);
particles[3].velocity = glm::normalize(orbitalDirection3) * v3;

*/

while(!glfwWindowShouldClose(window)) {

// resest acceleration each frame 
    for (auto& p : particles) {
      p.acceleration = glm::vec3(0.0f); // reset acceleration, as acceleration is accumalated from gravitational forces
    }

    // calculate deltaTime  
     auto currentTime = std::chrono::high_resolution_clock::now();
     float timeScale = 0.1f;
     float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
     deltaTime = glm::clamp(deltaTime, 0.0f, 0.016f); // max ~60fps
     deltaTime *= timeScale;
     previousTime = currentTime;

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

            float damping = 0.8f;

            // reflect off X bounds
            if (p.position.x < -2.0f || p.position.x > 2.0f) {
                p.velocity.x *= -2.0f * damping;
                p.position.x = glm::clamp(p.position.x, -2.0f, 2.0f);
            }

               if (p.position.y < -2.0f || p.position.y > 2.0f) {
                p.velocity.y *= -2.0f * damping;
                p.position.y = glm::clamp(p.position.y, -2.0f, 2.0f);
            }

            p.position.z = 0.0f;
        }
  } 

    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POINTS);
    
    // Red particle (Alpha)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(particles[0].position.x, particles[0].position.y);    

    // Yellow particle (Beta)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2f(particles[1].position.x, particles[1].position.y);    

    // Blue particle (Gamma)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(particles[2].position.x, particles[2].position.y);    

    // Green particle (Delta)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(particles[3].position.x, particles[3].position.y);    


    glEnd();

    glfwPollEvents();

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