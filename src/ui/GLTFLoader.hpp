#pragma once

#include <string>
#include <vector>
#include <map>
#include "Mesh.hpp"

// Forward declaration for tinygltf
namespace tinygltf {
    class Model;
}

class GLTFLoader {
public:
    GLTFLoader() = default;
    ~GLTFLoader() = default;

    GLTFLoader(const GLTFLoader&) = delete;
    GLTFLoader& operator=(const GLTFLoader&) = delete;

    // Load a glTF/glb file and extract mesh data
    // Returns vector of Mesh objects
    // For full glTF support, integrate tinygltf library properly
    static std::vector<Mesh> loadGLB(const std::string& filePath);

private:
    // Helper functions for processing glTF data (tinygltf integration)
    static std::vector<Vertex> extractVertices(const tinygltf::Model& model, int meshIndex);
    static std::vector<unsigned int> extractIndices(const tinygltf::Model& model, int meshIndex);
    static glm::vec3 getVec3FromAccessor(const tinygltf::Model& model, int accessorIndex, int elementIndex);
    static glm::vec2 getVec2FromAccessor(const tinygltf::Model& model, int accessorIndex, int elementIndex);
};
