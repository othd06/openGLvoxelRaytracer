

#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#include "shaders.h"
#include "loadModel.h"

#include <ctime>
bool Wait(const unsigned long &Time)
{
    clock_t Tick = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
    if(Tick < 0) // if clock() fails, it returns -1
        return 0;
    clock_t Now = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
    if(Now < 0)
        return 0;
    while( (Now - Tick) < Time )
    {
        Now = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
        if(Now < 0)
            return 0;
    }
    return 1;
}


const int WIDTH = 800;
const int HEIGHT = 600;

int winWidth = WIDTH;
int winHeight = HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLuint accumTex[2];
int frame = 0;




void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
    Shader showTexture("shaders/vertex.vs", "shaders/fragTexture.fs");

    GLuint FBO;
    int pingpong = 0;
    glGenTextures(2, accumTex);
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH*2 , HEIGHT*2, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glGenFramebuffers(1, &FBO);

    glDisable(GL_DEPTH_TEST);

    
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTex[pingpong], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }
        glViewport(0, 0, winWidth*2, winHeight*2);
        
        

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
        myShader.setInt("time", frame);
        myShader.setInt("frameCount", frame);
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
        Wait(2*frame);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width*2, height*2, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
}  




