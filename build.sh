#!/bin/bash
#
# Build script for Photo Scanner Kiosk
#
# Usage: ./build.sh [clean|debug|release]
#

BUILD_TYPE="${1:-release}"

case "$BUILD_TYPE" in
    clean)
        echo "Cleaning build directory..."
        rm -rf build
        echo "Clean complete!"
        exit 0
        ;;
    debug)
        CMAKE_BUILD_TYPE="Debug"
        ;;
    release)
        CMAKE_BUILD_TYPE="Release"
        ;;
    *)
        echo "Usage: $0 [clean|debug|release]"
        exit 1
        ;;
esac

echo "=========================================="
echo "Building Photo Scanner Kiosk"
echo "Build Type: $CMAKE_BUILD_TYPE"
echo "=========================================="
echo ""

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE ..

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: CMake configuration failed!"
    echo "Make sure all dependencies are installed."
    echo "Run ./install_rpi.sh to install dependencies."
    exit 1
fi

# Build
echo ""
echo "Building..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: Build failed!"
    exit 1
fi

echo ""
echo "=========================================="
echo "Build complete!"
echo "=========================================="
echo ""
echo "Executable: build/PhotoScannerKiosk"
echo ""
echo "To run:"
echo "  cd build"
echo "  ./PhotoScannerKiosk"
echo ""
echo "Testing Modes:"
echo "  Demo mode (mock scanner + payment):"
echo "    DEMO_MODE=true ./PhotoScannerKiosk --demo"
echo ""
echo "  Scanner-only mode (real scanner, skip payment):"
echo "    SCANNER_ONLY=true ./PhotoScannerKiosk --scanner-only"
echo ""
