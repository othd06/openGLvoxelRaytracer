

#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#include "shaders.h"
#include "loadModel.h"


const int WIDTH = 800;
const int HEIGHT = 600;

int winWidth = WIDTH;
int winHeight = HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Shader* myShaderPtr = nullptr;
unsigned int VAO;
GLuint texID;
GLFWwindow* window;
Model myModel = loadModel("dragon.binvox");
int frame = 0;

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        myShaderPtr->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, texID);
        myShaderPtr->setInt("model", 0);
        myShaderPtr->setInt3("modelDimms", myModel.xDim, myModel.yDim, myModel.zDim);
        myShaderPtr->setFloat("aspect", static_cast<float>(winWidth)/static_cast<float>(winHeight));
        myShaderPtr->setInt("time", frame);
        myShaderPtr->setInt("numRays", 100);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
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

    window = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Tracer", NULL, NULL);
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

    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_3D, texID);
    // Nearest filtering (no interpolation)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
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


    

    Shader myShader("shaders/vertex.vs", "shaders/fragment.fs");
    myShaderPtr = &myShader;


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    while(!glfwWindowShouldClose(window))
    {
        frame += 1;
        glfwPollEvents();
        processInput(window);
    }


    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    winWidth = width;
    winHeight = height;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
}  




