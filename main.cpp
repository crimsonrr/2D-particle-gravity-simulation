#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>                 
#include <glm/gtx/string_cast.hpp>    
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

class Particle {
    public:
     glm::vec3 position;
     glm::vec3 velocity;
     glm::vec3 acceleration;
     float mass;
     std::string name;
     int color;

     void update(float deltaTime) {
                velocity += acceleration * deltaTime;
                position += velocity * deltaTime;
            }    
};

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

std::vector<Particle> particles(4);


        particles[0].name = "Alpha";
        particles[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
        particles[0].velocity = glm::vec3(0.0f, 1.0f, 0.f);
        particles[0].acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
        
        particles[1].name = "Beta";
        particles[1].position = glm::vec3(0.2f, 0.0f, 0.0f);
        particles[1].velocity = glm::vec3(1.0f, 2.0f, 0.f);
        particles[1].acceleration = glm::vec3(0.0f, -9.8f, 0.0f);

        particles[2].name = "Gamma";
        particles[2].position = glm::vec3(-1.0f, 0.5f, 0.0f);
        particles[2].velocity = glm::vec3(0.5f, 0.0f, 0.f);
        particles[2].acceleration = glm::vec3(0.0f, -9.8f, 0.0f);

        particles[3].name = "Delta";
        particles[3].position = glm::vec3(-0.3f, 0.2f, 0.0f);
        particles[3].velocity = glm::vec3(2.0f, 0.5f, 0.f);
        particles[3].acceleration = glm::vec3(0.0f, -9.8f, 0.0f);

// set clear color to black
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glPointSize(10.0f);

auto previousTime = std::chrono::high_resolution_clock::now();

while(!glfwWindowShouldClose(window)) {
    // calculate deltaTime
     auto currentTime = std::chrono::high_resolution_clock::now();
     float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
     previousTime = currentTime;

  if(!paused) {
    for (auto& p : particles) {
            p.update(deltaTime);
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

    // to prevent particles from flying out of the screen
    for (auto& p : particles) {
        if (p.position.x > 1.0f) {
            p.position.x = 1.0f;
            p.velocity.x *=-1;
        }
        if(p.position.x < -1.0f) {
            p.position.x = -1.0f;
            p.velocity.x *=-1;
        }
         if (p.position.y > 1.0f) {
            p.velocity.y *=-0.9;
            p.velocity.y = -1.0f;
        }
        if (p.position.y < -1.0f) {
            p.velocity.y *=-1;
            p.position.y = -1.0f;
        }

}
    
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
        particles[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
        particles[0].velocity = glm::vec3(0.0f, 1.0f, 0.f);

        particles[1].position = glm::vec3(0.2f, 0.0f, 0.0f);
        particles[1].velocity = glm::vec3(1.0f, 2.0f, 0.f);

        particles[2].position = glm::vec3(-1.0f, 0.5f, 0.0f);
        particles[2].velocity = glm::vec3(0.5f, 0.0f, 0.f);

        particles[3].position = glm::vec3(-0.3f, 0.2f, 0.0f);
        particles[3].velocity = glm::vec3(2.0f, 0.5f, 0.f);

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // spam prevention
    }

    glfwSwapBuffers(window);
}

glfwDestroyWindow(window);
glfwTerminate();

return 0;
}
