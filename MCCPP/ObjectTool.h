#pragma once

#include <glm/glm.hpp>

#include <vector>

using namespace std;

namespace ObjectTool {
    vector<float> polygon(glm::vec3 targetPos, glm::vec3 translate) {
        return vector<float> {
			       translate.x,      translate.y,        translate.z,
            1.0f + translate.x, 0.0f + translate.y, 0.0f + translate.z,
            0.0f + translate.x, 0.0f + translate.y, 1.0f + translate.z,

            translate.x + targetPos.x,translate.y + targetPos.y, translate.z + targetPos.z,

            1.0f + translate.x, 0.0f + translate.y, 0.0f + translate.z,
            0.0f + translate.x, 0.0f + translate.y, 1.0f + translate.z
        };
    }
}