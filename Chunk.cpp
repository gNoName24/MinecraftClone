#include "Chunk.h"
#include "World.h"
#include "MCCPP/ObjectTool.h"
#include <FastNoiseLite/FastNoiseLite.h>

Chunk::Chunk(int chunkX, int chunkY, int chunkZ, World* world)
    : chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ), world(world) {
}

void Chunk::setBlock(int x, int y, int z, int type) {
    blocks[x][y][z] = type;
    dirty = true;
}

int Chunk::getBlock(int x, int y, int z) const {
    return blocks[x][y][z];
}

void Chunk::updateMesh() {
    vertices.clear();
    indices.clear();
    GLuint currentIndex = 0;

    struct Face { glm::ivec3 offset; glm::vec3 normal; };
    std::vector<Face> faces = {
        {{1,0,0},{1,0,0}}, {{-1,0,0},{-1,0,0}},
        {{0,1,0},{0,1,0}}, {{0,-1,0},{0,-1,0}},
        {{0,0,1},{0,0,1}}, {{0,0,-1},{0,0,-1}}
    };

    for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++)
            for (int z = 0; z < SIZE; z++) {
                int type = blocks[x][y][z];
                if (type == 0) continue;

                glm::vec3 center(chunkX * SIZE + x + 0.5f, chunkY * SIZE + y + 0.5f, chunkZ * SIZE + z + 0.5f);
                for (auto& face : faces) {
                    int wx = chunkX * SIZE + x + face.offset.x;
                    int wy = chunkY * SIZE + y + face.offset.y;
                    int wz = chunkZ * SIZE + z + face.offset.z;
                    if (world->getBlock(wx, wy, wz) != 0) continue;

                    glm::vec3 color = (type == 1) ? glm::vec3(0.4, 0.25, 0.1) : (type == 2) ? glm::vec3(0.4, 0.6, 0.4) : glm::vec3(0.3);
                    auto faceVerts = ObjectTool::cubeFace(center, face.normal, color, 1.0f);
                    vertices.insert(vertices.end(), faceVerts.begin(), faceVerts.end());
                    for (int i = 0; i < 6; ++i) indices.push_back(currentIndex + i);
                    currentIndex += 6;
                }
            }

    uploadToGPU();
    dirty = false;
}

void Chunk::uploadToGPU() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Chunk::render() const {
    if (VAO == 0) return;
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

