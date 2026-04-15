#!/bin/bash
# Script to download tinygltf library

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TINYGLTF_DIR="$SCRIPT_DIR/lib/tinygltf"

echo "Downloading tinygltf library..."

# Create directory if it doesn't exist
mkdir -p "$TINYGLTF_DIR"

# Download tiny_gltf.h from GitHub
curl -L "https://raw.githubusercontent.com/syoyo/tinygltf/release/tiny_gltf.h" -o "$TINYGLTF_DIR/tiny_gltf.h"

if [ -f "$TINYGLTF_DIR/tiny_gltf.h" ]; then
    echo "✓ Successfully downloaded tiny_gltf.h"
else
    echo "✗ Failed to download tiny_gltf.h"
    exit 1
fi

echo "Setup complete!"
echo "tinygltf library is installed at: $TINYGLTF_DIR"
