#include "GLTFLoader.hpp"
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

// tiny_gltf is implemented in TinyGLTFImpl.cpp
#include "tiny_gltf.h"

std::vector<Mesh> GLTFLoader::loadGLB(const std::string& filePath)
{
    // Check if file exists
    std::ifstream f(filePath);
    if (!f.good())
    {
        std::cerr << "File not found: " << filePath << std::endl;
        return {};
    }
    f.close();

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    // Load binary glTF (glb)
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filePath);

    if (!warn.empty())
        std::cerr << "glTF Warning: " << warn << std::endl;

    if (!ret)
    {
        std::cerr << "Failed to load glTF/glb file: " << filePath << std::endl;
        std::cerr << "Error: " << err << std::endl;
        return {};
    }

    std::cout << "✓ Loaded glTF model: " << filePath << std::endl;
    std::cout << "  Meshes: " << model.meshes.size() << std::endl;

    std::vector<Mesh> meshes;

    // Load all meshes from the model
    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        try
        {
            auto vertices = extractVertices(model, i);
            auto indices = extractIndices(model, i);

            if (vertices.empty() || indices.empty())
            {
                std::cerr << "  Warning: Mesh " << i << " has no valid geometry" << std::endl;
                continue;
            }

            Mesh mesh;
            mesh.uploadData(vertices, indices);
            meshes.push_back(std::move(mesh));

            std::cout << "  Mesh " << i << ": " << vertices.size() << " vertices, " 
                      << indices.size() << " indices" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "  Error loading mesh " << i << ": " << e.what() << std::endl;
        }
    }

    return meshes;
}

std::vector<Vertex> GLTFLoader::extractVertices(const tinygltf::Model& model, int meshIndex)
{
    std::vector<Vertex> vertices;

    if (meshIndex >= static_cast<int>(model.meshes.size()))
        return vertices;

    const auto& mesh = model.meshes[meshIndex];

    // Get position data from first primitive
    if (mesh.primitives.empty())
        return vertices;

    const auto& primitive = mesh.primitives[0];

    // Get POSITION accessor
    auto posIt = primitive.attributes.find("POSITION");
    if (posIt == primitive.attributes.end())
        return vertices;

    int posAccessorIdx = posIt->second;
    if (posAccessorIdx >= static_cast<int>(model.accessors.size()))
        return vertices;

    const auto& posAccessor = model.accessors[posAccessorIdx];
    int positionCount = posAccessor.count;

    // Get NORMAL accessor (optional)
    auto normalIt = primitive.attributes.find("NORMAL");
    int normalAccessorIdx = -1;
    if (normalIt != primitive.attributes.end())
    {
        normalAccessorIdx = normalIt->second;
        if (normalAccessorIdx >= static_cast<int>(model.accessors.size()))
            normalAccessorIdx = -1;
    }

    // Get TEXCOORD accessor (optional)
    auto texcoordIt = primitive.attributes.find("TEXCOORD_0");
    int texcoordAccessorIdx = -1;
    if (texcoordIt != primitive.attributes.end())
    {
        texcoordAccessorIdx = texcoordIt->second;
        if (texcoordAccessorIdx >= static_cast<int>(model.accessors.size()))
            texcoordAccessorIdx = -1;
    }

    vertices.resize(positionCount);

    for (int i = 0; i < positionCount; ++i)
    {
        // Get position
        vertices[i].position = getVec3FromAccessor(model, posAccessorIdx, i);

        // Get normal
        if (normalAccessorIdx >= 0)
            vertices[i].normal = getVec3FromAccessor(model, normalAccessorIdx, i);
        else
            vertices[i].normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default normal

        // Get texcoord
        if (texcoordAccessorIdx >= 0)
            vertices[i].texCoord = getVec2FromAccessor(model, texcoordAccessorIdx, i);
        else
            vertices[i].texCoord = glm::vec2(0.0f);
    }

    return vertices;
}

std::vector<unsigned int> GLTFLoader::extractIndices(const tinygltf::Model& model, int meshIndex)
{
    std::vector<unsigned int> indices;

    if (meshIndex >= static_cast<int>(model.meshes.size()))
        return indices;

    const auto& mesh = model.meshes[meshIndex];

    if (mesh.primitives.empty())
        return indices;

    const auto& primitive = mesh.primitives[0];

    // Get indices accessor
    int indicesAccessorIdx = primitive.indices;
    if (indicesAccessorIdx >= static_cast<int>(model.accessors.size()))
        return indices;

    const auto& indexAccessor = model.accessors[indicesAccessorIdx];
    int indexCount = indexAccessor.count;

    if (indexAccessor.bufferView >= static_cast<int>(model.bufferViews.size()))
        return indices;

    const auto& bufferView = model.bufferViews[indexAccessor.bufferView];
    if (bufferView.buffer >= static_cast<int>(model.buffers.size()))
        return indices;

    const auto& buffer = model.buffers[bufferView.buffer];
    const unsigned char* bufferData = buffer.data.data();
    const unsigned char* indexData = bufferData + bufferView.byteOffset + indexAccessor.byteOffset;

    // Handle different index types
    if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
    {
        const unsigned int* typedData = reinterpret_cast<const unsigned int*>(indexData);
        indices.assign(typedData, typedData + indexCount);
    }
    else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
    {
        const unsigned short* typedData = reinterpret_cast<const unsigned short*>(indexData);
        for (int i = 0; i < indexCount; ++i)
            indices.push_back(static_cast<unsigned int>(typedData[i]));
    }
    else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
    {
        const unsigned char* typedData = reinterpret_cast<const unsigned char*>(indexData);
        for (int i = 0; i < indexCount; ++i)
            indices.push_back(static_cast<unsigned int>(typedData[i]));
    }

    return indices;
}

glm::vec3 GLTFLoader::getVec3FromAccessor(const tinygltf::Model& model, int accessorIndex, int elementIndex)
{
    if (accessorIndex >= static_cast<int>(model.accessors.size()))
        return glm::vec3(0.0f);

    const auto& accessor = model.accessors[accessorIndex];

    if (accessor.bufferView >= static_cast<int>(model.bufferViews.size()))
        return glm::vec3(0.0f);

    const auto& bufferView = model.bufferViews[accessor.bufferView];

    if (bufferView.buffer >= static_cast<int>(model.buffers.size()))
        return glm::vec3(0.0f);

    const auto& buffer = model.buffers[bufferView.buffer];
    const unsigned char* bufferData = buffer.data.data();
    const unsigned char* data = bufferData + bufferView.byteOffset + accessor.byteOffset;

    // Get stride
    int stride = accessor.ByteStride(bufferView);
    if (stride < 0)
        stride = 3 * sizeof(float); // Default stride for vec3

    // Assume float type
    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
    {
        const float* typedData = reinterpret_cast<const float*>(data);
        const float* element = typedData + (elementIndex * stride / sizeof(float));
        return glm::vec3(element[0], element[1], element[2]);
    }

    return glm::vec3(0.0f);
}

glm::vec2 GLTFLoader::getVec2FromAccessor(const tinygltf::Model& model, int accessorIndex, int elementIndex)
{
    if (accessorIndex >= static_cast<int>(model.accessors.size()))
        return glm::vec2(0.0f);

    const auto& accessor = model.accessors[accessorIndex];

    if (accessor.bufferView >= static_cast<int>(model.bufferViews.size()))
        return glm::vec2(0.0f);

    const auto& bufferView = model.bufferViews[accessor.bufferView];

    if (bufferView.buffer >= static_cast<int>(model.buffers.size()))
        return glm::vec2(0.0f);

    const auto& buffer = model.buffers[bufferView.buffer];
    const unsigned char* bufferData = buffer.data.data();
    const unsigned char* data = bufferData + bufferView.byteOffset + accessor.byteOffset;

    // Get stride
    int stride = accessor.ByteStride(bufferView);
    if (stride < 0)
        stride = 2 * sizeof(float); // Default stride for vec2

    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
    {
        const float* typedData = reinterpret_cast<const float*>(data);
        const float* element = typedData + (elementIndex * stride / sizeof(float));
        return glm::vec2(element[0], element[1]);
    }

    return glm::vec2(0.0f);
}
