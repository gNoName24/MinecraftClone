#pragma once

#include <glm/glm.hpp>

#include <vector>

using namespace std;

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

    /*pair<vector<GLfloat>, vector<GLuint>> polyRectEBO(vector<GLfloat> v) {
        vector<GLfloat> vertices = v;
        vector<GLuint> indices = {
            0, 1, 2,
            2, 1, 3
        };
        return { vertices, indices };
    }*/
}