#!/bin/bash

set -e # exit on error

if [ "$1" == "--help" ] || [ "$1" == "" ]; then
    echo "usage: $0 [--configure|--build|--clean]"
    echo "--configure: configure project"
    echo "--build: build elterclick"
    echo "--clean: clean build directory"
    echo "--clean-build: make clean then build"
    echo "--run: run the binary"
    exit 0
fi

PROJ_DIR="./"
BUILD_DIR="./build"

download_cpm() {
    # create cpm folder
    if [[ ! -d "./CPM" ]]; then
        mkdir -p "./CPM"
    fi

    # download the cpm file
    curl -L "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake" \
        -o "./CPM/CPM.cmake"
}

configure() {
    if [[ ! -f "./CPM/CPM.cmake" ]]; then
        download_cpm
    fi

    # make sure we are on the project path then get the repos
    cd "$PROJ_DIR" || exit
    git submodule update --init --recursive

    # create buil dir
    if [ ! -d "$BUILD_DIR" ]; then
        mkdir -p "$BUILD_DIR"
    fi

    echo "running cmake config"
    cmake \
        -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON \
        -S $PROJ_DIR -B $BUILD_DIR
    
    if [ $? -ne 0 ]; then
        echo "cmake configuration failed!"
        exit 1
    fi
    
    echo "configuration completed successfully"
    cd ..
}

build() {
    if [ ! -d "$BUILD_DIR" ]; then
        echo "build directory not found, configuring first..."
        configure
    fi

    echo "building project..."
    cd "$BUILD_DIR" || exit
    make -j 4
    
    if [ $? -ne 0 ]; then
        echo "build failed..."
        exit 1
    fi
    
    # Check if binary was created
    if [ ! -f "elterclick" ]; then
        echo "error: binary not found..."
        exit 1
    fi
    
    echo "build completed successfully."
    cd ..
}

clean() {
    rm -rf "$BUILD_DIR"
}

run() {
    if [ ! -d "$BUILD_DIR" ]; then
        echo "forgot to build?"
        exit 1
    fi

    if [ ! -f "$BUILD_DIR/elterclick" ]; then
        echo "binary not found. forgot to build?"
        exit 1
    fi

    cd "$BUILD_DIR" || exit
    ./elterclick
}

if [ "$1" == "--clean" ]; then
    clean
fi

if [ "$1" == "--configure" ]; then
    configure
fi

if [ "$1" == "--build" ]; then
    build
fi

if [ "$1" == "--clean-build" ]; then
    make clean
    build
fi

if [ "$1" == "--run" ]; then
    run
fi
