#!/bin/bash

if [ "$1" == "--help" ] || [ "$1" == "" ]; then
    echo "usage: $0 [--configure|--build|--clean]"
    echo "--configure: configure project"
    echo "--build: build elterclick"
    echo "--clean: clean build directory"
    exit 0
fi

PROJ_DIR="./"
BUILD_DIR="./build"

configure() {

    echo "updating cmake"

    if [ ! -d "$BUILD_DIR" ]; then
        mkdir -p "$BUILD_DIR"
    fi

    cmake \
        -DSDL_AUDIO=OFF -DSDL_HAPTIC=OFF -DSDL_JOYSTICK=OFF -DSDL_POWER=OFF -DSDL_SENSOR=OFF \
        -DSDL_TESTS=OFF -DSDL_INSTALL=OFF -DSDL_STATIC=ON -DSDL_INSTALL_DOCS=OFF -DBUILD_STATIC_LIBS=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -S $PROJ_DIR -B $BUILD_DIR
    cd ..
}

build() {

    echo "building"

    if [ ! -d "$BUILD_DIR" ]; then
        echo "build directory does not exist. please run --configure first."
        exit 1
    fi

    cd "$BUILD_DIR" || exit

    if [ "$1" == true ];
    then 
        echo "cleaning..."
        make clean
    fi

    make -j4
    cd ..
}

clean() {
    echo "cleaning build directory"
    rm -rf "$BUILD_DIR"
}

run() {
    if [ ! -d "$BUILD_DIR" ]; then
        echo "build directory does not exist. please run --configure first."
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
    build false
fi

if [ "$1" == "--build-clean" ]; then
    build true
fi

if [ "$1" == "--run" ]; then
    run
fi
