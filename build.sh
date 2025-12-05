#!/bin/bash

_cleanup()
{
    cd - > /dev/null
}
set -e

CURR_DIR=$(pwd)

if [ "$#" -eq 0 ]; then
    echo "Usage build.sh <DAY>"
    exit 1
elif [ ! -f "day${1}/main.cpp" ]; then
    echo "Day not found!"
    exit 1
fi

ADVENT_DAY="day_$1"

if [ "$#" -ge 2 ]; then
    BUILD_TYPE="$2";
    echo "Building $BUILD_TYPE build";
else
    BUILD_TYPE="Debug";
fi

if [ -d "$CURR_DIR/build" ]; then
    rm -rf "build";
fi

mkdir "build";
cd build;
trap _cleanup EXIT;

echo "Building cmake target"
cmake $CURR_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE

echo "Building advent day $1"
cmake --build . --config $BUILD_TYPE --target $ADVENT_DAY

cd -;