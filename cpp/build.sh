#!/bin/bash

# Agent Zero C++ Backend Build Script

set -e

echo "=== Agent Zero C++ Backend Build Script ==="
echo ""

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
PYTHON_BINDINGS="ON"
NODE_BINDINGS="OFF"
TESTS="ON"
EXAMPLES="ON"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --no-python)
            PYTHON_BINDINGS="OFF"
            shift
            ;;
        --with-node)
            NODE_BINDINGS="ON"
            shift
            ;;
        --no-tests)
            TESTS="OFF"
            shift
            ;;
        --no-examples)
            EXAMPLES="OFF"
            shift
            ;;
        --clean)
            echo "Cleaning build directory..."
            rm -rf "$BUILD_DIR"
            shift
            ;;
        -j*)
            JOBS="${1#-j}"
            shift
            ;;
        --help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --debug          Build in Debug mode (default: Release)"
            echo "  --no-python      Disable Python bindings"
            echo "  --with-node      Enable Node.js bindings"
            echo "  --no-tests       Disable tests"
            echo "  --no-examples    Disable examples"
            echo "  --clean          Clean build directory before building"
            echo "  -j<N>            Use N parallel jobs (default: auto-detect)"
            echo "  --help           Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo "Build configuration:"
echo "  Build type: $BUILD_TYPE"
echo "  Build directory: $BUILD_DIR"
echo "  Python bindings: $PYTHON_BINDINGS"
echo "  Node.js bindings: $NODE_BINDINGS"
echo "  Tests: $TESTS"
echo "  Examples: $EXAMPLES"
echo "  Parallel jobs: $JOBS"
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "=== Configuring CMake ==="
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_PYTHON_BINDINGS="$PYTHON_BINDINGS" \
    -DBUILD_NODE_BINDINGS="$NODE_BINDINGS" \
    -DBUILD_TESTS="$TESTS" \
    -DBUILD_EXAMPLES="$EXAMPLES" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
echo ""
echo "=== Building ==="
cmake --build . --config "$BUILD_TYPE" -j "$JOBS"

# Run tests if enabled
if [ "$TESTS" = "ON" ]; then
    echo ""
    echo "=== Running Tests ==="
    ctest --output-on-failure --verbose
fi

echo ""
echo "=== Build Complete ==="
echo ""
echo "Build artifacts are in: $BUILD_DIR"

if [ "$EXAMPLES" = "ON" ]; then
    echo ""
    echo "To run examples:"
    echo "  ./build/examples/basic_usage"
    echo "  ./build/examples/multi_paradigm"
    echo "  ./build/examples/task_executor_example"
fi

if [ "$PYTHON_BINDINGS" = "ON" ]; then
    echo ""
    echo "Python bindings built. To use them, add to PYTHONPATH:"
    echo "  export PYTHONPATH=\$PYTHONPATH:$(pwd)/bindings/python"
fi
