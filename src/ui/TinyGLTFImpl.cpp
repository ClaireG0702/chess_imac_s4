// TinyGLTFImpl.cpp - Implementation of tinygltf
// Provides tinygltf and stb_image_write implementations
// Note: stb_image is handled by glimac/src/Image.cpp

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tiny_gltf.h"

// Stubs for advanced stb_image functions not provided by glimac
extern "C" {

// 16-bit image loading - stub (for tinygltf compatibility, usually not called for mesh data)
typedef unsigned short stbi__uint16;
stbi__uint16* stbi_load_16_from_memory(const unsigned char* buffer, int len, int* x, int* y, int* channels_in_file, int desired_channels) {
    (void)buffer; (void)len; (void)desired_channels;
    if (x) *x = 0;
    if (y) *y = 0;
    if (channels_in_file) *channels_in_file = 0;
    return nullptr;
}

// Check if image is 16-bit - stub
int stbi_is_16_bit_from_memory(const unsigned char* buffer, int len) {
    (void)buffer; (void)len;
    return 0;
}

}





