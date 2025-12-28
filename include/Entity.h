#ifndef ENTITY_H
#define ENTITY_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// Структура вершины
struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;
    glm::vec3 normal;
};

class Entity {
public:
    glm::vec3 position;
    glm::vec3 rotation; // В градусах (pitch, yaw, roll)
    glm::vec3 scale;

    bool isGlowing; // Для эффекта сияния тучек

    Entity();
    ~Entity();

    // Инициализация меша (передаем вектор вершин)
    void setupMesh(const std::vector<Vertex>& vertices);

    // Установка текстуры из SFML
    void setTexture(const sf::Texture& texture);

    // Отрисовка объекта
    void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint VAO, VBO;
    int vertexCount;
    const sf::Texture* texturePtr;

    // Вспомогательная функция для расчета матрицы модели
    glm::mat4 getModelMatrix() const;
};

#endif