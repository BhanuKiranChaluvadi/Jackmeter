#!/bin/bash
set -e

mkdir -p .build; pushd .build
cmake "$CMAKE_ARGS" -DCMAKE_BUILD_TYPE=Debug ..
make -j "$(nproc)"

popd
