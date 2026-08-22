#define GLEW_DLL
#define GLFW_DLL
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>
#include "ShaderLoader.h"

#include <Windows.h>



// переменные камеры
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;

const float cameraSpeed = 0.05f;
float cameraSensitivity = 0.1f;

bool firstMouse = true;



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

    // создание контекста окна
    GLFWwindow* window = glfwCreateWindow(512, 512, "MainWindow", NULL, NULL);
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


    // вычисление позиций точек
    float points[12];

    const float PI = 3.1415926535f;
    const float R = 0.8f;

    for (int i = 0; i < 6; i++)
    {
        float angle = -i * PI / 3.0f;
        float x = R * cos(angle);
        float y = R * sin(angle);
        points[2 * i] = x;
        points[2 * i + 1] = y;
    }

    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5
    };

    // генерация буферов
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // привязка буферов
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // отвязка буферов
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // Подключение шейдеров                                      

    GLuint shader_program = LoadShaders("Shaders/vertex.txt", "Shaders/fragment.txt");
    if (shader_program == 0)
    {
        std::cerr << "Failed to load shaders!" << std::endl;
        glfwTerminate();
        return -1;
    }


    // получение адреса для изменения цвета
    GLint colourLocation = glGetUniformLocation(shader_program, "ourColour");

    // получение адресов для вершинного шейдера
    GLint projectionLocation = glGetUniformLocation(shader_program, "projection");
    GLint viewLocation = glGetUniformLocation(shader_program, "view");
    GLint modelLocation = glGetUniformLocation(shader_program, "model");
    GLint transformLocation = glGetUniformLocation(shader_program, "transform");

    // настройка камеры

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw) * cos(glm::radians(pitch)));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw) * cos(glm::radians(pitch)));

    glfwSetCursorPosCallback(window, mouse_callback);
    
    

    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    // 
    //glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    //glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    //glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    

    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    

    // матрицы проекции и вида
    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f), 
        1.333f,
        0.1f,
        100.0f);

    


    // тело цикла отрисовки
    while (!glfwWindowShouldClose(window))
    {
        // управление камерой
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraFront * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraFront * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;



        // отрисовка

        float timeValue = glfwGetTime();

        glClearColor(1.0f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);


        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

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

        glUniform4f(
            colourLocation,
            (float)0.3f + 0.5f * sin(1.5f * timeValue),
            (float)0.6f + 0.3f * sin(3.0f * timeValue),
            (float)1.0f - 0.8f * sin(timeValue + 0.5f * PI),
            1.0f);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    
    xoffset *= cameraSensitivity;
    yoffset *= cameraSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
}