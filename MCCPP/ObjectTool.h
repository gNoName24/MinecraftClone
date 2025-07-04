#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>  

namespace ObjectTool {
    std::vector<GLfloat> polygon(glm::vec3 targetPos, glm::vec3 translate);
    std::vector<GLfloat> polyRect(glm::vec3 p1, glm::vec3 p2);
    std::vector<GLfloat> cubeFace(glm::vec3 center, glm::vec3 normal, glm::vec3 color, float size = 1.0f);
}
