#pragma once
#include <vector>
#include <glm/glm.hpp>

class World;

class Chunk {
public:
    static const int SIZE = 16;

    Chunk(int chunkX, int chunkY, int chunkZ, World* world);

    void setBlock(int x, int y, int z, int type);
    int getBlock(int x, int y, int z) const;
    void updateMesh();
    void uploadToGPU();
    void render() const;

    bool dirty = false;

private:
    int chunkX, chunkY, chunkZ;
    int blocks[SIZE][SIZE][SIZE] = {};
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    World* world;
};
