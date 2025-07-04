#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

class Chunk; // Вперёд объявляем класс, чтобы избежать циклической зависимости

// Пользовательский хешер для glm::ivec3
struct IVec3Hasher {
    std::size_t operator()(const glm::ivec3& v) const noexcept {
        std::size_t h1 = std::hash<int>()(v.x);
        std::size_t h2 = std::hash<int>()(v.y);
        std::size_t h3 = std::hash<int>()(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class World {
public:
    World(int worldWidth, int worldHeight, int worldDepth);

    void generate();
    void update();
    void render();

    void setBlock(int x, int y, int z, int type);
    int getBlock(int x, int y, int z) const;
    void updateBlockMesh(int x, int y, int z); // если ты реализуешь эту функцию позже

private:
    int worldWidth, worldHeight, worldDepth;
    std::unordered_map<glm::ivec3, Chunk*, IVec3Hasher> chunks;

    Chunk* getChunk(int x, int y, int z);
};
