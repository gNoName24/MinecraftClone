#include "World.h"
#include "Chunk.h"
#include <FastNoiseLite/FastNoiseLite.h>

Chunk* World::getChunk(int x, int y, int z) {
    glm::ivec3 key(x / Chunk::SIZE, y / Chunk::SIZE, z / Chunk::SIZE);
    auto it = chunks.find(key);
    if (it == chunks.end()) {
        chunks[key] = new Chunk(key.x, key.y, key.z, this);
    }
    return chunks[key];
}

World::World(int worldWidth, int worldHeight, int worldDepth)
    : worldWidth(worldWidth), worldHeight(worldHeight), worldDepth(worldDepth) {
}

void World::setBlock(int x, int y, int z, int type) {
    Chunk* c = getChunk(x, y, z);
    c->setBlock(x % Chunk::SIZE, y % Chunk::SIZE, z % Chunk::SIZE, type);
}

int World::getBlock(int x, int y, int z) const {
    glm::ivec3 key(x / Chunk::SIZE, y / Chunk::SIZE, z / Chunk::SIZE);
    auto it = chunks.find(key);
    if (it == chunks.end()) return 0;
    return it->second->getBlock(x % Chunk::SIZE, y % Chunk::SIZE, z % Chunk::SIZE);
}

void World::generate() {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);

    for (int x = 0; x < worldWidth; ++x) {
        for (int z = 0; z < worldDepth; ++z) {
            float fx = static_cast<float>(x);
            float fz = static_cast<float>(z);
            float heightNoise = noise.GetNoise(fx, fz);
            int h = static_cast<int>(heightNoise * 20 + worldHeight / 2); // Центр по высоте

            for (int y = 0; y <= h && y < worldHeight; ++y) {
                int type = (y == h) ? 2 : 1;
                setBlock(x, y, z, type);
            }
        }
    }
}

void World::update() {
    for (auto& [pos, chunk] : chunks)
        if (chunk->dirty)
            chunk->updateMesh();
}

void World::render() {
    for (auto& [pos, chunk] : chunks)
        chunk->render();
}

// Пустышка, если ты её вызовешь — не забудь реализовать
void World::updateBlockMesh(int x, int y, int z) {
    // Заглушка
}
