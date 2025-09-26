#define _USE_MATH_DEFINES 
#include <iostream>
#include <vector>
#include <cmath> // For sin and cos

// GLEW
#include <GL/glew.h> 

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "car.h"
#include "shader.h"
#include "circuit.h"
// #include "dashboard.h"

// Window dimensions (initial values)
GLint WIDTH = 800, HEIGHT = 800;

// Camera variables
// Initial camera position
glm::vec3 cameraPos   = glm::vec3(-30.0f, 2.0f, 0.0f); 
// Camera looks towards negative Z-axis by default
glm::vec3 cameraFront = glm::vec3(1.0f, -0.2f, 0.0f); 
// Camera's up direction
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
// Camera mode: 0 = static, 1 = follow car
int cameraMode = 0;   


// Create a Car object
Car myCar;
Circuit ground;
// Dashboard dashboard;

// Movement speed for camera and ball
float cameraSpeed = 2.5f; // Units per second
float ballSpeed = 1.5f;   // Units per second

// Time variables for frame-rate independent movement
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


/**
 * @brief Keyboard input callback function.
 * Handles camera movement and ball movement based on key presses.
 * @param window The GLFW window that received the event.
 * @param key The keyboard key that was pressed or released.
 * @param scancode The system-specific scancode of the key.
 * @param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
 * @param mods Bit field describing which modifier keys were held down.
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Only process key presses and repeats
    if (action == GLFW_PRESS || GLFW_REPEAT) {
        // Camera controls (W, S, A, D, Q, E)
        if (key == GLFW_KEY_W) // Move camera forward
            cameraPos += cameraSpeed * deltaTime * cameraFront;
        if (key == GLFW_KEY_S) // Move camera backward
            cameraPos -= cameraSpeed * deltaTime * cameraFront;
        if (key == GLFW_KEY_A) // Move camera left
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
        if (key == GLFW_KEY_D) // Move camera right
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
        if (key == GLFW_KEY_Q) // Move camera up
            cameraPos += cameraSpeed * deltaTime * cameraUp;
        if (key == GLFW_KEY_E) // Move camera down
            cameraPos -= cameraSpeed * deltaTime * cameraUp;

        // Brake & Throttle & Wheel controls (Arrow keys)
        if (key == GLFW_KEY_UP){ // Move ball up
            myCar.setThrottle(true);
            // myCar.updateAcceleration({1, 0, 0});
        }
        if (key == GLFW_KEY_DOWN){ // Move ball down
            myCar.setBreak(true);
            // myCar.addBreak(true);
        }
        if (key == GLFW_KEY_LEFT){ // Move ball left
            myCar.setDeltaLeft(true);
            // myCar.turnLeft(true);
        }
        if (key == GLFW_KEY_RIGHT){ // Move ball right
            myCar.setDeltaRight(true);
            // myCar.turnRight(true);
        }
        
    }

    if (action == GLFW_RELEASE){
        if (key == GLFW_KEY_DOWN){
            myCar.setBreak(false);
        }
        if (key == GLFW_KEY_UP){
            myCar.setThrottle(false);
        }
        if (key == GLFW_KEY_LEFT){
            myCar.setDeltaLeft(false);
        }
        if (key == GLFW_KEY_RIGHT){
            myCar.setDeltaRight(false);
        }
        // Camera mode switch (C key)
        if (key == GLFW_KEY_C) {
            cameraMode = !cameraMode; // Toggle between 0 and 1
            // std::cout << "Camera mode: " << (cameraMode == 0 ? "Static" : "Follow Car") << std::endl;
        }
    }
}

/**
 * @brief Callback for when the window is resized.
 * Updates the OpenGL viewport to match the new window dimensions.
 * @param window The GLFW window that was resized.
 * @param width The new width of the window.
 * @param height The new height of the window.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }
    glViewport(0, 0, width, height);
    WIDTH = width; // Update global WIDTH
    HEIGHT = height; // Update global HEIGHT
    // dashboard.setWindowSize(WIDTH, HEIGHT);
}

int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cout << "GLFW initialization failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 2. Configure GLFW window options
    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use core profile (no deprecated functions)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Required for macOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // Enable window resizing
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // Changed to GL_TRUE

    // 3. Create GLFW window object
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Formula 1", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set the keyboard callback function
    glfwSetKeyCallback(window, key_callback);
    // Set the framebuffer size callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Added callback

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Required for core profile functionality
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Enable depth testing for correct 3D rendering (objects closer obscure those farther)
    glEnable(GL_DEPTH_TEST);

    // Define the rendering area within the window
    glViewport(0, 0, WIDTH, HEIGHT);
    
    // Setup dashboard
    // dashboard.setWindowSize(WIDTH, HEIGHT);

    // Initial model matrix for the static sphere (Sphere 1)
    glm::mat4 model1 = glm::mat4(1.0f); 
    model1 = glm::translate(model1, glm::vec3(-1.0f, 0.0f, 0.0f)); // Position static sphere to the left

    // Initial model matrix for the cube
    glm::mat4 modelCube = glm::mat4(1.0f);
    modelCube = glm::translate(modelCube, glm::vec3(1.0f, 0.0f, 0.0f)); // Position cube to the right

    // load the customized shader
    Shader carshader("assets/shaders/carShader.vert", "assets/shaders/carShader.frag");

    // Load an OBJ model (replace with your actual .obj file)

    ground.setupGPUBuffers();

    if (myCar.loadModel()) {
        myCar.setupGPUBuffers(); // Setup GPU buffers after loading
    } else {
        std::cerr << "Failed to load car model." << std::endl;
        // Handle error, maybe use a fallback primitive
        return -1;
    }

    // TODO: Set texture (implement texture loading properly)
    // myCar.setTexture("path/to/your/car_texture.png");

    // Initial car properties
    // myCar.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));

    glm::vec3 lightPos(0.0f, 2.0f, 0.0f); // 你的光源位置
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // 光源颜色

    double lastFrameTime = glfwGetTime();

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate deltaTime for frame-rate independent movement
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and process events (e.g., keyboard input)
        glfwPollEvents();

        myCar.update(deltaTime);

        // Clear the color buffer with a dark teal background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
        // Clear both color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // --- Projection Matrix (calculated once per frame, then passed to both shaders) ---
        // Create a perspective projection matrix
        // Parameters: Field of View (45 degrees), Aspect Ratio, Near Plane (0.1), Far Plane (100.0)
        glm::mat4 projection = glm::perspective(glm::radians(50.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        // --- View Matrix (Camera Transformation, calculated once per frame) ---
        // Create the view matrix using the LookAt function
        // Parameters: Camera Position, Point Camera is Looking At, Camera Up Vector
        glm::vec3 viewTarget = cameraPos + cameraFront;
        
        // If in follow mode, update camera position to follow the car
        if (cameraMode == 1) {
            glm::vec3 carPos = myCar.getPosition();
            glm::mat3 carOrientation = glm::mat3(myCar.getModelMatrix());
            cameraPos = carPos + carOrientation * glm::vec3(-8.0f, 2.0f, 0.0f); // Follow behind and above the car
            viewTarget = carPos; // Look at the car
        }
        
        glm::mat4 view = glm::lookAt(cameraPos, viewTarget, cameraUp);

        glBindVertexArray(0); // Unbind VAO to prevent accidental modification from the last frame

        carshader.use();
        carshader.setMat4("view", view);
        carshader.setMat4("projection", projection);
        carshader.setVec3("lightPos", lightPos);
        carshader.setVec3("lightColor", lightColor);
        
        // printMat4(view);
        ground.draw(carshader);
        myCar.draw(carshader);

        // Render dashboard with current RPM and speed
        int rpm = static_cast<int>(glm::length(myCar.getVelocity()) * 20);
        float speed = glm::length(myCar.getVelocity()) * 3.6f; // Convert m/s to km/h
        // dashboard.render(rpm, speed);

        // Swap front and back buffers (double buffering)
        glfwSwapBuffers(window);
        // while (true) {};   
    }

    glfwTerminate(); // Terminate GLFW
    return 0;
}
