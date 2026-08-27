#define GLEW_DLL
#define GLFW_DLL
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <Windows.h>
#include <iostream>

#include "Camera.h"
#include "ShaderLoader.h"
#include "Model.h"



// переменные камеры
Camera* camera = nullptr;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    camera -> RotateCamera(xposIn, yposIn);
}


// поля для управления объектом объекта

float rotationBase = 0.0f;
const float baseRotationMinValue = -120.0f;
const float baseRotationMaxValue = 120.0f;

float rotationShoulder1 = 0.0f;
const float shoulder1RotationMinValue = -100.0f;
const float shoulder1RotationMaxValue = 30.0f;

float rotationShoulder2 = 0.0f;
const float shoulder2RotationMinValue = -45.0f;
const float shoulder2RotationMaxValue = 45.0f;




int main()
{
    // Проверка инициализации библиотеки GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3. \n");
        return 1;
    }

    // задание версии библиотеки GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    // создание контекста окна
    GLFWwindow* window = glfwCreateWindow(1024, 768, "MainWindow", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    

    

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    // инициализация GLEW
    GLenum ret = glewInit();
    if (GLEW_OK != ret)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        return 1;
    }

    const GLubyte* version_str = glGetString(GL_VERSION);
    const GLubyte* device_str = glGetString(GL_RENDERER);

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("This version OpenGL running is %s\n", version_str);
    printf("This device OpenGL running is %s\n", device_str);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Подключение шейдеров                                      

    GLuint shader_program = LoadShaders("Shaders/vertex.txt", "Shaders/fragment.txt");
    if (shader_program == 0)
    {
        std::cerr << "Failed to load shaders!" << std::endl;
        glfwTerminate();
        return -1;
    }


    // подключение модели
    Model model("../Libs/Graph_LR3.obj");

    

    // получение адреса униформ шейдеров
    GLint colourLocation = glGetUniformLocation(shader_program, "lightColour");
    GLint projectionLocation = glGetUniformLocation(shader_program, "projection");
    GLint viewLocation = glGetUniformLocation(shader_program, "view");
    GLint modelLocation = glGetUniformLocation(shader_program, "model");
    GLint transformLocation = glGetUniformLocation(shader_program, "transform");
    GLint normalMatrixLocation = glGetUniformLocation(shader_program, "normalMatrix");
    GLint viewPositionLocation = glGetUniformLocation(shader_program, "viewPos");

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    // настройка камеры

    glfwSetCursorPosCallback(window, mouse_callback);

    Camera cameraObject;
    camera = &cameraObject;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // матрицы проекции и вида
    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f),
        1.333f,
        0.1f,
        100.0f);

    // настройка света
    glm::vec3 lightPos(1.5f, 1.0f, 3.0f);
    glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuse(0.8f, 0.8f, 0.8f);
    glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);

    GLint lightPosLocation = glGetUniformLocation(shader_program, "light.position");
    GLint lightAmbLocation = glGetUniformLocation(shader_program, "light.ambient");
    GLint lightDifLocation = glGetUniformLocation(shader_program, "light.diffuse");
    GLint lightSpecLocation = glGetUniformLocation(shader_program, "light.specular");

    

    // настройка материала
    glm::vec3 materialAmbient(0.1f, 0.1f, 0.8f);
    glm::vec3 materialDiffuse(0.1f, 0.1f, 0.5f);
    glm::vec3 materialSpecular(1.0f, 1.0f, 1.0f);
    float materialShininess(50.0f);
    
    GLint materialAmbLocation = glGetUniformLocation(shader_program, "material.ambient");
    GLint materialDifLocation = glGetUniformLocation(shader_program, "material.diffuse");
    GLint materialSpecLocation = glGetUniformLocation(shader_program, "material.specular");
    GLint materialShiLocation = glGetUniformLocation(shader_program, "material.shininess");


    float lastFrame = 0.0f;

    // тело цикла отрисовки
    while (!glfwWindowShouldClose(window))
    {
        cameraObject.MoveCamera(window);


        // управление деталями объекта

        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        const float rotationSpeed = 60.0f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            rotationBase -= rotationSpeed;
            if (rotationBase < baseRotationMinValue)
                rotationBase = baseRotationMinValue;
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            rotationBase += rotationSpeed;
            if (rotationBase > baseRotationMaxValue)
                rotationBase = baseRotationMaxValue;
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        {
            rotationShoulder1 += rotationSpeed;
            if (rotationShoulder1 > shoulder1RotationMaxValue)
                rotationShoulder1 = shoulder1RotationMaxValue;
        }

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        {
            rotationShoulder1 -= rotationSpeed;
            if (rotationShoulder1 < shoulder1RotationMinValue)
                rotationShoulder1 = shoulder1RotationMinValue;
        }

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            rotationShoulder2 += rotationSpeed;
            if (rotationShoulder2 > shoulder2RotationMaxValue)
                rotationShoulder2 = shoulder2RotationMaxValue;
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            rotationShoulder2 -= rotationSpeed;
            if (rotationShoulder2 < shoulder2RotationMinValue)
                rotationShoulder2 = shoulder2RotationMinValue;
        }


        // отрисовка

        glClearColor(0.5f, 0.5f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);


        glm::mat4 view = cameraObject.GetViewMatrix();

        glm::mat4 modelMatrix = glm::mat4(1.0f);

        glUniformMatrix4fv(
            projectionLocation,
            1,
            GL_FALSE,
            glm::value_ptr(projection));

        glUniformMatrix4fv(
            viewLocation,
            1,
            GL_FALSE,
            glm::value_ptr(view));

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(modelMatrix));

        glUniformMatrix3fv(
            normalMatrixLocation,
            1,
            GL_FALSE,
            glm::value_ptr(normalMatrix));

        glUniform3f(
            viewPositionLocation,
            cameraObject.cameraPos.x,
            cameraObject.cameraPos.y, 
            cameraObject.cameraPos.z);


        glUniform3f(lightPosLocation, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(lightAmbLocation, lightAmbient.x, lightAmbient.y, lightAmbient.z);
        glUniform3f(lightDifLocation, lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
        glUniform3f(lightSpecLocation, lightSpecular.x, lightSpecular.y, lightSpecular.z);

        glUniform3f(materialAmbLocation, materialAmbient.x, materialAmbient.y, materialAmbient.z);
        glUniform3f(materialDifLocation, materialDiffuse.x, materialDiffuse.y, materialDiffuse.z);
        glUniform3f(materialSpecLocation, materialSpecular.x, materialSpecular.y, materialSpecular.z);
        glUniform1f(materialShiLocation, materialShininess);

        

        model.Draw(shader_program, rotationBase, rotationShoulder1, rotationShoulder2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

