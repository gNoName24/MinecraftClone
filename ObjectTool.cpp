#include <glm/gtc/constants.hpp>
#include <vector>
#include "MCCPP/ObjectTool.h"
#include <glad/glad.h>

std::vector<GLfloat> ObjectTool::polygon(glm::vec3 targetPos, glm::vec3 translate) {
    return {
        translate.x + targetPos.x,
        translate.y + targetPos.y,
        translate.z + targetPos.z,
        1.0f + translate.x, 0.0f + translate.y, 0.0f + translate.z,
        0.0f + translate.x, 0.0f + translate.y, 1.0f + translate.z
    };
}

std::vector<GLfloat> ObjectTool::polyRect(glm::vec3 p1, glm::vec3 p2) {
    GLfloat minX = std::min(p1.x, p2.x);
    GLfloat maxX = std::max(p1.x, p2.x);
    GLfloat minY = std::min(p1.y, p2.y);
    GLfloat maxY = std::max(p1.y, p2.y);
    GLfloat minZ = std::min(p1.z, p2.z);
    GLfloat maxZ = std::max(p1.z, p2.z);
    return {
        minX, minY, minZ,
        maxX, minY, minZ,
        maxX, maxY, maxZ,
        maxX, maxY, maxZ,
        minX, maxY, maxZ,
        minX, minY, minZ
    };
}

std::vector<GLfloat> ObjectTool::cubeFace(glm::vec3 center, glm::vec3 normal, glm::vec3 color, float size) {
    glm::vec3 up, right;

    if (std::abs(normal.y) < 0.999f)
        up = glm::vec3(0, 1, 0);
    else
        up = glm::vec3(1, 0, 0);

    right = glm::normalize(glm::cross(up, normal));
    up = glm::normalize(glm::cross(normal, right));

    float half = size / 2.0f;
    glm::vec3 offset = normal * 0.5f;

    glm::vec3 v0 = center - right * half - up * half + offset;
    glm::vec3 v1 = center + right * half - up * half + offset;
    glm::vec3 v2 = center + right * half + up * half + offset;
    glm::vec3 v3 = center - right * half + up * half + offset;

    std::vector<GLfloat> data;

    auto pushVertex = [&](glm::vec3 v) {
        data.push_back(v.x); data.push_back(v.y); data.push_back(v.z);
        data.push_back(normal.x); data.push_back(normal.y); data.push_back(normal.z);
        data.push_back(color.r); data.push_back(color.g); data.push_back(color.b);
        };

    pushVertex(v0); pushVertex(v1); pushVertex(v2);
    pushVertex(v2); pushVertex(v3); pushVertex(v0);

    return data;
}
