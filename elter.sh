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

    cd "$PROJ_DIR" || exit
    git submodule update --init --recursive

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
        exit 1
    fi

    cd "$BUILD_DIR" || exit

    if [ "$1" == true ];
    then 
        make clean
    fi

    make -j4
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
    build false
fi

if [ "$1" == "--build-clean" ]; then
    build true
fi

if [ "$1" == "--run" ]; then
    run
fi
