#include "../include/Entity.h"

Entity::Entity()
    : position(0.0f), rotation(0.0f), scale(1.0f),
    VAO(0), VBO(0), vertexCount(0), texturePtr(nullptr), isGlowing(false) {
}

Entity::~Entity() {
    //if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    //if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void Entity::setupMesh(const std::vector<Vertex>& vertices) {
    vertexCount = static_cast<int>(vertices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Атрибут 0: Позиция
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Атрибут 1: Текстурные координаты
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Атрибут 2: Нормали
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);
}

void Entity::setTexture(const sf::Texture& texture) {
    texturePtr = &texture;
}

glm::mat4 Entity::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    // Вращение по трем осям
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    model = glm::scale(model, scale);
    return model;
}

void Entity::draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    if (VAO == 0) return;

    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(getModelMatrix()));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(glGetUniformLocation(shaderProgram, "isGlowing"), isGlowing);

    if (texturePtr) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturePtr->getNativeHandle());
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}