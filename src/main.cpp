

#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#include "shaders.h"
#include "loadModel.h"
#include "helper.h"
#include <cmath>





const int WIDTH = 800;
const int HEIGHT = 600;
const float PI = 3.1415926535897932384;
const float DEGREE = PI/180;

int winWidth = WIDTH;
int winHeight = HEIGHT;
double resolutionScale = 1.0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

GLuint accumTex[2];
int frame = 0;

Vector3 position = {x: 0.0, y: 300.0, z: -850.0};
float pitch = -22.0*DEGREE;
float yaw = 0;
mat3 yawMat = mat3{cos(yaw) , 0.0, sin(yaw),
                          0.0      , 1.0, 0.0,
                          -sin(yaw), 0.0, cos(yaw)};
mat3 pitchMat = mat3{1.0, 0.0        , 0.0,
                          0.0, cos(pitch) , sin(pitch),
                          0.0, -sin(pitch), cos(pitch)};
mat3 direction = mul(yawMat, pitchMat);


double currentTime;
double lastTime;
double deltaTime;

bool mouseDisabled = false;
bool escapePressed = false;


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        if (escapePressed) return;
        escapePressed = true;
        if (!mouseDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseDisabled = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseDisabled = false;
        }
    }
    else escapePressed = false;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += mul(yawMat, Vector3{x: 0.0, y: 0.0, z: 140.0*deltaTime});

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position += mul(yawMat, Vector3{x: 0.0, y: 0.0, z: -140.0*deltaTime});

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += mul(yawMat, Vector3{x: 140.0*deltaTime, y: 0.0, z: 0});

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position += mul(yawMat, Vector3{x: -140.0*deltaTime, y: 0.0, z: 0.0});

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        position.y += 140.0*deltaTime;

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        position.y -= 140.0*deltaTime;

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        yaw += 20*DEGREE*deltaTime;
        yawMat = mat3{cos(yaw) , 0.0, sin(yaw),
                      0.0      , 1.0, 0.0,
                      -sin(yaw), 0.0, cos(yaw)};
        direction = mul(yawMat, pitchMat);

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        yaw -= 20*DEGREE*deltaTime;
        yawMat = mat3{cos(yaw) , 0.0, sin(yaw),
                      0.0      , 1.0, 0.0,
                      -sin(yaw), 0.0, cos(yaw)};
        direction = mul(yawMat, pitchMat);

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        pitch += 15*DEGREE*deltaTime;
        if (pitch > 75*DEGREE) pitch = 75*DEGREE;
        pitchMat = mat3{1.0, 0.0        , 0.0,
                        0.0, cos(pitch) , sin(pitch),
                        0.0, -sin(pitch), cos(pitch)};
        direction = mul(yawMat, pitchMat);

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        pitch -= 15*DEGREE*deltaTime;
        if (pitch < -75*DEGREE) pitch = -75*DEGREE;
        pitchMat = mat3{1.0, 0.0        , 0.0,
                        0.0, cos(pitch) , sin(pitch),
                        0.0, -sin(pitch), cos(pitch)};
        direction = mul(yawMat, pitchMat);

        frame = 0;
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, accumTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        }
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Tracer", NULL, NULL);
    if (!window)
    {
        std::cout << "failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);


    float vertices[] =
    {
         1.0f,  1.0f, 0.0f,  // top right
         1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left 
    };
    unsigned int indices[] =
    {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };   

    unsigned int VAO;
    glGenVertexArrays(1, &VAO); 

    glBindVertexArray(VAO);


    unsigned int VBO;
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    Model myModel = loadModel("dragon.binvox");

    GLuint texID;
    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_3D, texID);
    // Nearest filtering (no interpolation)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    
    std::vector<unsigned char> flat = flatData(myModel);
    glTexImage3D(GL_TEXTURE_3D,
                 0,                // mip level
                 GL_R8UI,          // internal format: 1 channel, 8 bits
                 myModel.xDim,
                 myModel.yDim,
                 myModel.zDim,
                 0,                // border
                 GL_RED_INTEGER,   // format of input data
                 GL_UNSIGNED_BYTE, // type of input data
                 &flat[0]);        // pointer to voxel data

    flat.clear();
    free(myModel);

    

    Shader myShader("shaders/vertex.vs", "shaders/fragment.fs");
    Shader showTexture("shaders/vertex.vs", "shaders/fragTexture.fs");

    GLuint FBO;
    int pingpong = 0;
    glGenTextures(2, accumTex);
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(WIDTH)*resolutionScale), int(float(HEIGHT)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glGenFramebuffers(1, &FBO);

    glDisable(GL_DEPTH_TEST);

    
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTex[pingpong], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }
        glViewport(0, 0, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale));
        
        

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        myShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, texID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, accumTex[1 - pingpong]);
        myShader.setInt("prevFrame", 1);

        myShader.setInt("model", 0);
        myShader.setInt3("modelDimms", myModel.xDim, myModel.yDim, myModel.zDim);
        myShader.setFloat("aspect", static_cast<float>(winWidth)/static_cast<float>(winHeight));
        myShader.setInt("time", frame + int(floor(currentTime*1000)));
        myShader.setInt("frameCount", frame);
        myShader.setMat3("directionMat", direction);
        myShader.setFloat3("position", position.x, position.y, position.z);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, winWidth, winHeight);

        showTexture.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[pingpong]);
        showTexture.setInt("screenTex", 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        pingpong = 1 - pingpong;
        frame += 1;


        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    winWidth = width;
    winHeight = height;
    

    frame = 0;
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(width)*resolutionScale), int(float(height)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
    }
}  

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if (!mouseDisabled) return;

    static bool firstMouse = true;
    static double lastX, lastY;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // reversed: y-coordinates go top->bottom
    lastX = xpos;
    lastY = ypos;

    // Apply sensitivity
    float sensitivity = 0.001f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Update your yaw/pitch
    yaw   += (float)xoffset;
    pitch += (float)yoffset;

    yawMat = mat3{cos(yaw) , 0.0, sin(yaw),
                  0.0      , 1.0, 0.0,
                  -sin(yaw), 0.0, cos(yaw)};
    pitchMat = mat3{1.0, 0.0        , 0.0,
                    0.0, cos(pitch) , sin(pitch),
                    0.0, -sin(pitch), cos(pitch)};
    direction = mul(yawMat, pitchMat);

    frame = 0;
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(float(winWidth)*resolutionScale), int(float(winHeight)*resolutionScale), 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    // Clamp pitch to avoid flipping
    if (pitch > 75.0*DEGREE)  pitch = 75.0*DEGREE;
    if (pitch < -75.0*DEGREE) pitch = -75.0*DEGREE;
}



