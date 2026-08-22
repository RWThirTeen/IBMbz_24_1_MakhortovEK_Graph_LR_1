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
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    // матрицы проекции и вида
    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f), 
        1.333f,
        0.1f,
        100.0f);

    


    // тело цикла отрисовки
    while (!glfwWindowShouldClose(window))
    {
        float timeValue = glfwGetTime();

        glClearColor(1.0f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);


        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

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
