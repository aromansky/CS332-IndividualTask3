#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <ctime>

#include "../include/Shaders.h" 
#include "../include/Entity.h"
#include "../include/ModelLoader.h"

// Функция для компиляции шейдеров
GLuint compileProgram(const char* vSource, const char* fSource) {
    auto compileShader = [](GLenum type, const char* source) {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &source, NULL);
        glCompileShader(s);
        return s;
        };
    GLuint vs = compileShader(GL_VERTEX_SHADER, vSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fSource);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    return prog;
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    float totalTime = 0.0f;

    // 1. Настройка окна SFML
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::Window window(sf::VideoMode(1280, 720), "Christmas Airship", sf::Style::Default, settings);
    window.setFramerateLimit(60);
    window.setActive(true);

    // 2. Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // 3. Компиляция шейдеров
    GLuint shaderProgram = compileProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);

    // 4. Загрузка ресурсов
    sf::Texture texGrass, texTree, texShip, texHouse, texCloud, texCat;
    texGrass.loadFromFile("assets/grass.png");
    texTree.loadFromFile("assets/tree.png");
    texShip.loadFromFile("assets/hat.png");
    texHouse.loadFromFile("assets/house.png");
    texCat.loadFromFile("assets/cat.png");
    texCloud.loadFromFile("assets/sphere.png");

    auto treeMesh = ModelLoader::loadOBJ("assets/tree.obj");
    auto shipMesh = ModelLoader::loadOBJ("assets/hat.obj");
    auto grassMesh = ModelLoader::loadOBJ("assets/grass.obj");
    auto houseMesh = ModelLoader::loadOBJ("assets/house.obj");
    auto catMesh = ModelLoader::loadOBJ("assets/cat.obj");
    auto snowManMesh = ModelLoader::loadOBJ("assets/snowman.obj");
    auto cloudMesh = ModelLoader::loadOBJ("assets/sphere.obj");


    // 5. Создание объектов сцены
    Entity ground;
    ground.setupMesh(grassMesh);
    ground.setTexture(texGrass);
    ground.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
    ground.position = glm::vec3(0, 0, 0);

    Entity tree;
    tree.setupMesh(treeMesh);
    tree.setTexture(texTree);
    tree.position = glm::vec3(0, 0, 0); // Ёлка в центре
    tree.scale = glm::vec3(2.0f);

    Entity airship;
    airship.setupMesh(shipMesh);
    airship.setTexture(texShip);
    airship.scale = glm::vec3(0.1, 0.1, 0.1);
    airship.position = glm::vec3(0, 10, 20);

    std::vector<Entity> environment;
    // Нам нужно не меньше 5 домиков
    for (int i = 0; i < 10; ++i) {
        Entity h;
        h.setupMesh(houseMesh);
        h.setTexture(texHouse);

        float x = static_cast<float>(rand() % 150) - 50.0f;
        float z = static_cast<float>(rand() % 150) - 50.0f;

        // 2. Если дом попал слишком близко к елке (центру), отодвигаем его подальше
        if (std::abs(x) < 15.0f && std::abs(z) < 15.0f) {
            x += (x > 0) ? 20.0f : -20.0f; // Отодвигаем в зависимости от стороны
            z += (z > 0) ? 20.0f : -20.0f;
        }

        h.position = glm::vec3(x, 8.0f, z);

        h.scale = glm::vec3(0.1f, 0.1f, 0.1f);

        h.rotation.y = static_cast<float>(rand() % 360);

        environment.push_back(h);
    }

    for (int i = 0; i < 5; ++i) {
        Entity cat;
        cat.setupMesh(catMesh);
        cat.setTexture(texCat);

        // Случайное положение на поле
        float x = static_cast<float>(rand() % 140) - 70.0f;
        float z = static_cast<float>(rand() % 140) - 70.0f;

        cat.position = glm::vec3(x, 8.0f, z);
        cat.scale = glm::vec3(1.5f);
        cat.rotation.y = static_cast<float>(rand() % 360);

        environment.push_back(cat);
    }

    for (int i = 0; i < 4; ++i) {
        Entity snowman;
        snowman.setupMesh(snowManMesh);
        snowman.setTexture(texGrass);

        // Случайное положение на поле
        float x = static_cast<float>(rand() % 140) - 70.0f;
        float z = static_cast<float>(rand() % 140) - 70.0f;

        snowman.position = glm::vec3(x, 8.0f, z);
        snowman.scale = glm::vec3(2.0f);
        snowman.rotation.y = static_cast<float>(rand() % 360);

        environment.push_back(snowman);
    }

    std::vector<Entity> clouds;
    for (int i = 0; i < 6; ++i) {
        Entity c;
        c.setupMesh(cloudMesh);
        c.setTexture(texCloud);
        c.position = glm::vec3(rand() % 100 - 50, 25 + rand() % 10, rand() % 100 - 50);
        c.scale = glm::vec3(3.0f);
        clouds.push_back(c);
    }


    // 6. Параметры освещения
    GLint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    GLint lightColLoc = glGetUniformLocation(shaderProgram, "lightColor");
    glUniform3f(lightDirLoc, -0.2f, -1.0f, -0.3f);
    glUniform3f(lightColLoc, 1.0f, 1.0f, 0.9f);

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        totalTime += dt;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // 7. Управление дирижаблем
        float speed = 15.0f;
        float rotSpeed = 90.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            airship.position += glm::vec3(sin(glm::radians(airship.rotation.y)), 0, cos(glm::radians(airship.rotation.y))) * speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            airship.position -= glm::vec3(sin(glm::radians(airship.rotation.y)), 0, cos(glm::radians(airship.rotation.y))) * speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) airship.rotation.y += rotSpeed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) airship.rotation.y -= rotSpeed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) airship.position.y += speed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) airship.position.y -= speed * dt;

        // 8. Камера (Вид сзади и сверху)
        glm::vec3 cameraOffset = glm::vec3(-sin(glm::radians(airship.rotation.y)) * 15.0f, 8.0f, -cos(glm::radians(airship.rotation.y)) * 15.0f);
        glm::vec3 cameraPos = airship.position + cameraOffset;
        glm::mat4 view = glm::lookAt(cameraPos, airship.position, glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 500.0f);

        // 9. Рендеринг
        glClearColor(0.5f, 0.7f, 0.9f, 1.0f); // Цвет неба
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ground.draw(shaderProgram, view, projection);
        tree.draw(shaderProgram, view, projection);
        airship.draw(shaderProgram, view, projection);

        for (auto& h : environment) h.draw(shaderProgram, view, projection);

         //Эффект молний для тучек
        for (auto& c : clouds) {
            c.isGlowing = (fmod(totalTime + c.position.x * 0.1f, 2.0f) > 1.8f); // Мерцание
            c.draw(shaderProgram, view, projection);
        }

        window.display();
    }

    return 0;
}