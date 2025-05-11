#!/bin/bash

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

    cmake \
        -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON \
        -S $PROJ_DIR -B $BUILD_DIR
    cd ..
}

build() {

    if [ ! -d "$BUILD_DIR" ]; then
        configure
    fi

    cd "$BUILD_DIR" || exit
    make --parallel 4 -j 4
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
