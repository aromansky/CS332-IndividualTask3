#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

class ModelLoader {
public:
    static std::vector<Vertex> loadOBJ(const std::string& path) {
        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec2> temp_texCoords;
        std::vector<glm::vec3> temp_normals;
        std::vector<Vertex> out_vertices;

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open OBJ " << path << std::endl;
            return out_vertices;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                glm::vec3 v;
                ss >> v.x >> v.y >> v.z;
                temp_positions.push_back(v);
            }
            else if (prefix == "vt") {
                glm::vec2 vt;
                ss >> vt.x >> vt.y;
                vt.y = 1.0f - vt.y; // Инверсия Y для OpenGL
                temp_texCoords.push_back(vt);
            }
            else if (prefix == "vn") {
                glm::vec3 vn;
                ss >> vn.x >> vn.y >> vn.z;
                temp_normals.push_back(vn);
            }
            else if (prefix == "f") {
                std::vector<Vertex> faceVertices;
                std::string vertexStr;

                // Читаем все вершины одной грани (их может быть 3, 4 и более)
                while (ss >> vertexStr) {
                    size_t firstSlash = vertexStr.find('/');
                    size_t lastSlash = vertexStr.rfind('/');

                    int vIdx = std::stoi(vertexStr.substr(0, firstSlash));
                    int vtIdx = std::stoi(vertexStr.substr(firstSlash + 1, lastSlash - firstSlash - 1));
                    int vnIdx = std::stoi(vertexStr.substr(lastSlash + 1));

                    Vertex v;
                    v.position = temp_positions[vIdx - 1];
                    v.texCoords = temp_texCoords[vtIdx - 1];
                    v.normal = temp_normals[vnIdx - 1];
                    faceVertices.push_back(v);
                }

                // Триангуляция методом "Веера" (Triangle Fan)
                // Превращаем многоугольник (0, 1, 2, 3, 4...) в треугольники (0,1,2), (0,2,3), (0,3,4)...
                for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                    out_vertices.push_back(faceVertices[0]);
                    out_vertices.push_back(faceVertices[i]);
                    out_vertices.push_back(faceVertices[i + 1]);
                }
            }
        }
        return out_vertices;
    }
};