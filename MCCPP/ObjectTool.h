#pragma once

#include <glm/glm.hpp>

#include <vector>

using namespace std;

template<typename T>
std::vector<T> concat(std::initializer_list<std::vector<T>> lists) {
    std::vector<T> result;
    for (const auto& l : lists)
        result.insert(result.end(), l.begin(), l.end());
    return result;
}

namespace ObjectTool {
    vector<GLfloat> polygon(glm::vec3 targetPos, glm::vec3 translate) {
        return
            vector<GLfloat> {
                translate.x + targetPos.x,
                translate.y + targetPos.y,
                translate.z + targetPos.z,
                1.0f + translate.x, 0.0f + translate.y, 0.0f + translate.z,
                0.0f + translate.x, 0.0f + translate.y, 1.0f + translate.z
            };
    }

    vector<GLfloat> polyRect(glm::vec3 p1, glm::vec3 p2) {
        GLfloat minX = min(p1.x, p2.x);
        GLfloat maxX = max(p1.x, p2.x);
        GLfloat minY = min(p1.y, p2.y);
        GLfloat maxY = max(p1.y, p2.y);
        GLfloat minZ = min(p1.z, p2.z);
        GLfloat maxZ = max(p1.z, p2.z);
        return {
            minX, minY, minZ,
            maxX, minY, minZ,
            maxX, maxY, maxZ,
            maxX, maxY, maxZ,
            minX, maxY, maxZ,
            minX, minY, minZ
        };
    }

    vector<GLfloat> polyCube(glm::vec3 position, glm::vec3 size) {
        return {
            // Нижняя грань (0, -1, 0)
            position.x, position.y, position.z,                             0.0f, -1.0f,  0.0f,
            position.x + size.x, position.y, position.z,                    0.0f, -1.0f,  0.0f,
            position.x, position.y, position.z + size.z,                    0.0f, -1.0f,  0.0f,
            position.x + size.x, position.y, position.z + size.z,           0.0f, -1.0f,  0.0f,
            position.x + size.x, position.y, position.z,                    0.0f, -1.0f,  0.0f,
            position.x, position.y, position.z + size.z,                    0.0f, -1.0f,  0.0f,

            // Верхняя грань (0, 1, 0)
            position.x, position.y + size.y, position.z,                    0.0f,  1.0f,  0.0f,
            position.x + size.x, position.y + size.y, position.z,           0.0f,  1.0f,  0.0f,
            position.x, position.y + size.y, position.z + size.z,           0.0f,  1.0f,  0.0f,
            position.x + size.x, position.y + size.y, position.z + size.z,  0.0f,  1.0f,  0.0f,
            position.x + size.x, position.y + size.y, position.z,           0.0f,  1.0f,  0.0f,
            position.x, position.y + size.y, position.z + size.z,           0.0f,  1.0f,  0.0f,

            // Правая грань (1, 0, 0)
            position.x + size.x, position.y, position.z,                    1.0f,  0.0f,  0.0f,
            position.x + size.x, position.y + size.y, position.z,           1.0f,  0.0f,  0.0f,
            position.x + size.x, position.y, position.z + size.z,           1.0f,  0.0f,  0.0f,
            position.x + size.x, position.y + size.y, position.z + size.z,  1.0f,  0.0f,  0.0f,
            position.x + size.x, position.y + size.y, position.z,           1.0f,  0.0f,  0.0f,
            position.x + size.x, position.y, position.z + size.z,           1.0f,  0.0f,  0.0f,

            // Левая грань (-1, 0, 0)
            position.x, position.y, position.z,                            -1.0f,  0.0f,  0.0f,
            position.x, position.y + size.y, position.z,                   -1.0f,  0.0f,  0.0f,
            position.x, position.y, position.z + size.z,                   -1.0f,  0.0f,  0.0f,
            position.x, position.y + size.y, position.z + size.z,          -1.0f,  0.0f,  0.0f,
            position.x, position.y + size.y, position.z,                   -1.0f,  0.0f,  0.0f,
            position.x, position.y, position.z + size.z,                   -1.0f,  0.0f,  0.0f,

            // Задняя грань (0, 0, 1)
            position.x, position.y, position.z + size.z,                    0.0f,  0.0f,  1.0f,
            position.x, position.y + size.y, position.z + size.z,           0.0f,  0.0f,  1.0f,
            position.x + size.x, position.y, position.z + size.z,           0.0f,  0.0f,  1.0f,
            position.x + size.x, position.y + size.y, position.z + size.z,  0.0f,  0.0f,  1.0f,
            position.x, position.y + size.y, position.z + size.z,           0.0f,  0.0f,  1.0f,
            position.x + size.x, position.y, position.z + size.z,           0.0f,  0.0f,  1.0f,

            // Передняя грань (0, 0, -1)
            position.x, position.y, position.z,                             0.0f,  0.0f, -1.0f,
            position.x, position.y + size.y, position.z,                    0.0f,  0.0f, -1.0f,
            position.x + size.x, position.y, position.z,                    0.0f,  0.0f, -1.0f,
            position.x + size.x, position.y + size.y, position.z,           0.0f,  0.0f, -1.0f,
            position.x, position.y + size.y, position.z,                    0.0f,  0.0f, -1.0f,
            position.x + size.x, position.y, position.z,                    0.0f,  0.0f, -1.0f,
        };
    }


    // Криво сделано, надо переделать функцию 
    /*pair<vector<GLfloat>, vector<GLuint>> polyRectEBO(vector<GLfloat> v) {
        vector<GLfloat> vertices = v;
        vector<GLuint> indices = {
            0, 1, 2,
            2, 1, 3
        };
        return { vertices, indices };
    }*/
}