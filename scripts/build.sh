#!/bin/bash
clear
set -euo pipefail

BINARY="server"
CC="gcc"
SRC="main.c"
BUILD_DIR="build"
BIN_PATH="$BUILD_DIR/$BINARY"
LOG_FILE="$BUILD_DIR/build.log"

# Compiler flags for SDL2 (C++17)
CCFLAGS="-Wall $(sdl2-config --cflags)"
CCLIBS="$(sdl2-config --libs)  -lX11 -lXtst"  # Add -lSDL2_ttf if you use SDL_ttf

# Remove old binary
if [ -f "$BIN_PATH" ]; then
    rm -f "$BIN_PATH"
    echo "Removed old binary"
fi

# Ensure build directory exists
mkdir -p "$BUILD_DIR"

# Compile and log
echo -e "\e[1;34mBuilding $BINARY...\e[0m"
if $CC $CCFLAGS -o "$BIN_PATH" "$SRC" $CCLIBS &> "$LOG_FILE"; then
    echo -e "\e[1;32mBuild complete: $BIN_PATH\e[0m"
else
    echo -e "\e[1;31mBuild failed. Check log: $LOG_FILE\e[0m"
    exit 1
fi

# Function to kill the binary if running
cleanup() {
    if pgrep -f "$BIN_PATH" >/dev/null; then
        pkill -f "$BIN_PATH" || true
        echo -e "\e[1;33m$BINARY killed\e[0m"
    fi
}

# Trap SIGINT and SIGTERM to kill the binary
trap cleanup SIGINT SIGTERM

# Run the binary
if [ -f "$BIN_PATH" ]; then
    "$BIN_PATH" :99 
fi
